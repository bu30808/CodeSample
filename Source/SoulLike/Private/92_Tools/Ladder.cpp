// Fill out your copyright notice in the Description page of Project Settings.


#include "92_Tools/Ladder.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/00_Player/01_Component/LadderMovementComponent.h"
#include "96_Library/MathHelperLibrary.h"
#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
ALadder::ALadder()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	PoleRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PoleRootComponent"));
	PoleRootComponent->SetupAttachment(RootComponent);

	LadderPoleRightMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LadderPoleRightMeshComponent"));
	LadderPoleRightMeshComponent->SetupAttachment(PoleRootComponent);

	LadderPoleLeftMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LadderPoleLeftMeshComponent"));
	LadderPoleLeftMeshComponent->SetupAttachment(PoleRootComponent);

	TopBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("LadderTop"));
	TopBoxComponent->SetupAttachment(PoleRootComponent);
	TopBoxComponent->SetCollisionProfileName("ForPlayerMovement");

	BottomBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("LadderBottom"));
	BottomBoxComponent->SetupAttachment(PoleRootComponent);
	BottomBoxComponent->SetCollisionProfileName("ForPlayerMovement");

	BarInstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(
		TEXT("BarInstancedStaticMeshComponent"));
	BarInstancedStaticMeshComponent->SetupAttachment(RootComponent);

	ScrewInstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(
		TEXT("ScrewInstancedStaticMeshComponent"));
	ScrewInstancedStaticMeshComponent->SetupAttachment(RootComponent);


	LadderEnter_Bottom = CreateDefaultSubobject<UBillboardComponent>(TEXT("LadderEnter_Bottom"));
	LadderEnter_Bottom->SetupAttachment(RootComponent);
	LadderEnter_Top = CreateDefaultSubobject<UBillboardComponent>(TEXT("LadderEnter_Top"));
	LadderEnter_Top->SetupAttachment(RootComponent);


	LadderExit_Bottom = CreateDefaultSubobject<UBillboardComponent>(TEXT("LadderExit_Bottom"));
	LadderExit_Bottom->SetupAttachment(RootComponent);
	LadderExit_Top = CreateDefaultSubobject<UBillboardComponent>(TEXT("LadderExit_Top"));
	LadderExit_Top->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ALadder::BeginPlay()
{
	Super::BeginPlay();
}

void ALadder::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (BottomBoxComponent)
	{
		BottomBoxComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ALadder::OnBoxComponentBeginOverlapEvent);
		BottomBoxComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &ALadder::OnBoxComponentEndOverlapEvent);
	}

	if (BottomBoxComponent)
	{
		TopBoxComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ALadder::OnBoxComponentBeginOverlapEvent);
		TopBoxComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &ALadder::OnBoxComponentEndOverlapEvent);
	}
}


void ALadder::OnBoxComponentBeginOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                              const FHitResult& SweepResult)
{
	if (OtherActor->IsA<APlayerCharacter>())
	{
		OverlappedPlayer = Cast<APlayerCharacter>(OtherActor);
		if (OverlappedComponent == TopBoxComponent)
		{
			//UKismetSystemLibrary::PrintString(this, FString::SanitizeFloat(dot));
			if (UMathHelperLibrary::SameDirection(OverlappedPlayer->GetActorForwardVector(),
			                                      TopBoxComponent->GetForwardVector()))
			{
				LadderClimbType = ELadderClimbType::EnterFromTop;
				//같은 방향을 바라보는 경우
				if (auto widget = OverlappedPlayer->GetInteractionWidget(
					this, OverlappedPlayer->GetInputDataAsset()->InteractAction,
					Execute_GetActionName(this)))
				{
					widget->SetWorldLocation(TopBoxComponent->GetComponentLocation());
					widget->SetVisibility(true);
				}
			}
			else
			{
				OverlappedPlayer->HideInteractionWidget();
				LadderClimbType = ELadderClimbType::None;
			}
		}

		if (OverlappedComponent == BottomBoxComponent)
		{
			//UKismetSystemLibrary::PrintString(this, FString::SanitizeFloat(dot));
			if (UMathHelperLibrary::SameDirection(OverlappedPlayer->GetActorForwardVector(),
			                                      BottomBoxComponent->GetForwardVector()) == false)
			{
				LadderClimbType = ELadderClimbType::EnterFromBottom;
				//다른 방향을 바라보는 경우
				if (auto widget = OverlappedPlayer->GetInteractionWidget(
					this, OverlappedPlayer->GetInputDataAsset()->InteractAction,
					Execute_GetActionName(this)))
				{
					UKismetSystemLibrary::PrintString(this,TEXT("보임!!"));
					widget->SetWorldLocation(BottomBoxComponent->GetComponentLocation());
					widget->SetVisibility(true);
				}
			}
			else
			{
				OverlappedPlayer->HideInteractionWidget();
				LadderClimbType = ELadderClimbType::None;
			}
		}
	}
}

void ALadder::OnBoxComponentEndOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == OverlappedPlayer)
	{
		LadderClimbType = ELadderClimbType::None;
		OverlappedPlayer->HideInteractionWidget();
		OverlappedPlayer = nullptr;
	}
}

void ALadder::Interaction_Implementation(ABaseCharacter* Start)
{
	if (LadderClimbType == ELadderClimbType::EnterFromTop)
	{
		if (UMathHelperLibrary::SameDirection(
			OverlappedPlayer->GetActorForwardVector(),
			TopBoxComponent->GetForwardVector()) == false)
		{
			LadderClimbType = ELadderClimbType::None;
			return;
		}
	}

	if (LadderClimbType == ELadderClimbType::EnterFromBottom)
	{
		if (UMathHelperLibrary::SameDirection(
			OverlappedPlayer->GetActorForwardVector(),
			BottomBoxComponent->GetForwardVector()))
		{
			LadderClimbType = ELadderClimbType::None;
			return;
		}
	}

	InteractionPlayer = Cast<APlayerCharacter>(Start);
	if (InteractionPlayer)
	{
		BottomBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		TopBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//올라갈 것인가 타고 내려갈 것인가 결정합니다.
		auto climbType = InteractionPlayer->GetLadderMovementComponent()->GetClimbLadderType(this);
		switch (climbType)
		{
		case ELadderMove::UP:
			InteractionPlayer->GetLadderMovementComponent()->ReadyToLadderClimbUp(this);
			break;
		case ELadderMove::DOWN:
			InteractionPlayer->GetLadderMovementComponent()->ReadyToLadderClimbDown(this);
			break;
		}
	}
}

void ALadder::FinishInteraction_Implementation()
{
	if (InteractionPlayer)
	{
		InteractionPlayer->GetLadderMovementComponent()->SetUseLadderMovement(false);
		InteractionPlayer->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

		BottomBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		TopBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}
