// Fill out your copyright notice in the Description page of Project Settings.


#include "92_Tools/DoorActor.h"

#include "NiagaraComponent.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "96_Library/MathHelperLibrary.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ADoorActor::ADoorActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	DoorComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	DoorComponent->SetupAttachment(RootComponent);
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(DoorComponent);
	BoxComponent->SetCollisionProfileName(TEXT("Interaction"));

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Glow"));
	NiagaraComponent->SetupAttachment(DoorComponent);

	TimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("DoorMoveTimeline"));
}

void ADoorActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ClosedYawRotation = DoorComponent->GetRelativeRotation().Yaw;

	BoxComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ADoorActor::OnActorBeginOverlapEvent);
	BoxComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &ADoorActor::OnActorEndOverlapEvent);


	OnUpdateTransform.BindDynamic(this, &ADoorActor::OnUpdateDoorTransformEvent);


	OnFinishDoorMove.BindDynamic(this, &ADoorActor::OnFinishDoorMoveEvent);

	if (TimelineComponent)
	{
		TimelineComponent->SetTimelineLength(DoorMoveTime);
		TimelineComponent->AddInterpFloat(DoorMoveCurve, OnUpdateTransform);
		TimelineComponent->SetTimelineFinishedFunc(OnFinishDoorMove);
	}
}

// Called when the game starts or when spawned
void ADoorActor::BeginPlay()
{
	Super::BeginPlay();
}

void ADoorActor::OnUpdateDoorTransformEvent_Implementation(float Alpha)
{
	const float& curYaw = DoorComponent->GetRelativeRotation().Yaw;

	FRotator curRot = DoorComponent->GetRelativeRotation();

	if (IsOpen())
	{
		FRotator targetRot = DoorComponent->GetRelativeRotation();
		targetRot.Yaw = ClosedYawRotation;

		DoorComponent->SetRelativeRotation(
			UKismetMathLibrary::RLerp(curRot, targetRot, Alpha, true));
		return;
	}


	if (DoorInteractionPoint == EDoorInteractionPoint::FRONT)
	{
		FRotator targetRot = DoorComponent->GetRelativeRotation();
		targetRot.Yaw = ClosedYawRotation + FrontYawValue;
		DoorComponent->SetRelativeRotation(UKismetMathLibrary::RLerp(curRot, targetRot, Alpha, true));
	}
	else
	{
		FRotator targetRot = DoorComponent->GetRelativeRotation();
		targetRot.Yaw = ClosedYawRotation + BackYawValue;

		DoorComponent->SetRelativeRotation(
			UKismetMathLibrary::RLerp(curRot, targetRot, Alpha, true));
	}
}

void ADoorActor::OnFinishDoorMoveEvent()
{
	if (IsOpen())
	{
		DoorState = EDoorState::CLOSED;
	}
	else
	{
		DoorState = EDoorState::OPEN;
	}
	
	auto interactionWidget = Player->ShowInteractionWidget(this, Player->GetInteractionAction(), Execute_GetActionName(this));
	interactionWidget->SetWorldLocation(BoxComponent->GetComponentLocation());
	
	Player->EnableInput(Cast<APlayerController>(Player->GetController()));
	
}

/*
void ADoorActor::OnActorBeginOverlapEvent(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor->IsA<APlayerCharacter>())
	{
		if (auto player = Cast<APlayerCharacter>(OtherActor))
		{
			auto interactionWidget = player->ShowInteractionWidget(player, player->GetInteractionAction(),
			                                                       Execute_GetActionName(this));
			interactionWidget->SetWorldLocation(BoxComponent->GetComponentLocation());
		}
	}
}

void ADoorActor::OnActorEndOverlapEvent(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor->IsA<APlayerCharacter>())
	{
		auto player = Cast<APlayerCharacter>(OtherActor);
		player->HideInteractionWidget();
	}
}
*/

void ADoorActor::OnActorBeginOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<APlayerCharacter>())
	{
		if (auto player = Cast<APlayerCharacter>(OtherActor))
		{
			UE_LOGFMT(LogTemp,Log,"플레이어 오버렙");
			player->PauseFindInteractionTarget();
			auto interactionWidget = player->ShowInteractionWidget(this, player->GetInteractionAction(),
																   Execute_GetActionName(this));
			interactionWidget->SetWorldLocation(BoxComponent->GetComponentLocation());
		}
	}
}

void ADoorActor::OnActorEndOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA<APlayerCharacter>())
	{
		auto player = Cast<APlayerCharacter>(OtherActor);
		player->ResumeFindInteractionTarget();
		player->HideInteractionWidget();
	}
}
#if WITH_EDITOR

void ADoorActor::UpdateBoxExtent()
{
	FVector origin;
	FVector extent;
	float radius;
	UKismetSystemLibrary::GetComponentBounds(DoorComponent, origin, extent, radius);

	extent*=0.5f;
	
	extent += GetActorForwardVector() * 80.f;
	BoxComponent->SetBoxExtent(extent, false);
}

void ADoorActor::SetBoxLocationZ()
{
	FVector origin;
	FVector extent;
	float radius;
	UKismetSystemLibrary::GetComponentBounds(DoorComponent, origin, extent, radius);

	BoxComponent->SetRelativeLocation(FVector(0, 0, extent.Z));
}

void ADoorActor::AddXLocation_Right()
{
	FVector origin;
	FVector extent;
	float radius;
	UKismetSystemLibrary::GetComponentBounds(DoorComponent, origin, extent, radius);

	BoxComponent->AddRelativeLocation(FVector(extent.X, 0, 0));
}

void ADoorActor::AddXLocation_Left()
{
	FVector origin;
	FVector extent;
	float radius;
	UKismetSystemLibrary::GetComponentBounds(DoorComponent, origin, extent, radius);

	BoxComponent->AddRelativeLocation(FVector(extent.X * -1, 0, 0));
}

void ADoorActor::AddYLocation_Right()
{
	FVector origin;
	FVector extent;
	float radius;
	UKismetSystemLibrary::GetComponentBounds(DoorComponent, origin, extent, radius);

	BoxComponent->AddRelativeLocation(FVector(0, extent.Y, 0));
}

void ADoorActor::AddYLocation_Left()
{
	FVector origin;
	FVector extent;
	float radius;
	UKismetSystemLibrary::GetComponentBounds(DoorComponent, origin, extent, radius);

	BoxComponent->AddRelativeLocation(FVector(0, extent.Y * -1, 0));
}
#endif

void ADoorActor::Interaction_Implementation(ABaseCharacter* Start)
{
	if (Start->IsA<APlayerCharacter>())
	{
		Player = Cast<APlayerCharacter>(Start);
		auto pc = Cast<APlayerController>(Player->GetController());
		Player->DisableInput(pc);

		UGameplayStatics::PlaySoundAtLocation(this, DoorOpenSound, GetActorLocation());

		//앞에서 상호작용했니? 아니면 뒤에서 상호작용 했니?
		if (UMathHelperLibrary::SameDirection(Player->GetActorForwardVector(), GetActorForwardVector()))
		{
			UE_LOGFMT(LogTemp,Log,"문 뒤에서 상호작용 합니다.");
			//뒤
			DoorInteractionPoint = EDoorInteractionPoint::BACK;
		}
		else
		{
			UE_LOGFMT(LogTemp,Log,"문 정면에서 상호작용 합니다.");
			//앞
			DoorInteractionPoint = EDoorInteractionPoint::FRONT;
		}


		TimelineComponent->PlayFromStart();
	}
}

ADoorActor_Slide::ADoorActor_Slide()
{
	PrimaryActorTick.bCanEverTick = false;

	SecondDoorComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondDoorComponent"));
	SecondDoorComponent->SetupAttachment(RootComponent);

	NiagaraComponent->SetupAttachment(SecondDoorComponent);

	DoorType = EDoorType::SLIDE;
}

void ADoorActor_Slide::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ClosedRelativeLocation = SecondDoorComponent->GetRelativeLocation();
}

void ADoorActor_Slide::OnUpdateDoorTransformEvent_Implementation(float Alpha)
{
	if (!IsOpen())
	{
		SecondDoorComponent->SetRelativeLocation(
			UKismetMathLibrary::VLerp(ClosedRelativeLocation, FVector::ZeroVector, Alpha));
	}
	else
	{
		SecondDoorComponent->SetRelativeLocation(
			UKismetMathLibrary::VLerp(SecondDoorComponent->GetRelativeLocation(), ClosedRelativeLocation, Alpha));
	}
}

#if WITH_EDITOR
void ADoorActor_Slide::UpdateBoxExtent()
{
	if (BoxComponent)
	{
		FVector resultExtent;
		{
			FVector origin;
			FVector extent;
			float radius;
			UKismetSystemLibrary::GetComponentBounds(DoorComponent, origin, extent, radius);
			resultExtent += extent;
		}
		resultExtent += GetActorForwardVector() * 50.f;
		BoxComponent->SetBoxExtent(resultExtent, false);
	}
}
#endif
void ADoorActor_Slide::Interaction_Implementation(ABaseCharacter* Start)
{
	if (Start->IsA<APlayerCharacter>())
	{
		Player = Cast<APlayerCharacter>(Start);
		auto pc = Cast<APlayerController>(Player->GetController());
		Player->DisableInput(pc);
		UGameplayStatics::PlaySoundAtLocation(this, DoorOpenSound, GetActorLocation());
		TimelineComponent->PlayFromStart();
	}
}
