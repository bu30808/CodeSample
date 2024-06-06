// Fill out your copyright notice in the Description page of Project Settings.


#include "92_Tools/JumpPad.h"

#include "NiagaraComponent.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/00_Player/01_Component/JumpMovementComponent.h"
#include "96_Library/MathHelperLibrary.h"
#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameplayTagContainer.h"
#include "GameFramework/CharacterMovementComponent.h"

#define LADDER_IGNORE_MOVE_TAG FGameplayTag::RequestGameplayTag("Common.PlayerInput.Ignore.Move.FromJumpPad")

// Sets default values
AJumpPad::AJumpPad()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootSceneComponent;

	JumpPoint_Top = CreateDefaultSubobject<UBillboardComponent>(TEXT("JumpPoint_TopComponent"));
	JumpPoint_Top->SetupAttachment(RootComponent);

	JumpPoint_Bottom = CreateDefaultSubobject<UBillboardComponent>(TEXT("JumpPoint_BottomComponent"));
	JumpPoint_Bottom->SetupAttachment(RootComponent);


	TopBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("TopBoxComponent"));
	TopBoxComponent->SetupAttachment(JumpPoint_Top);
	TopBoxComponent->SetCollisionProfileName("ForPlayerMovement");

	BottomBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BottomBoxComponent"));
	BottomBoxComponent->SetupAttachment(JumpPoint_Bottom);
	BottomBoxComponent->SetCollisionProfileName("ForPlayerMovement");

	ActivateMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ActivateMeshComponent"));
	ActivateMeshComponent->SetupAttachment(RootComponent);

	ActivateParticleComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ActivateParticleComponent"));
	ActivateParticleComponent->SetupAttachment(ActivateMeshComponent, "FireSocket");
	ActivateParticleComponent->SetRelativeScale3D(FVector(0.25, 0.25, 0.25));
	ActivateParticleComponent->SetVisibility(false);

	SetActorTickEnabled(false);
}

// Called when the game starts or when spawned
void AJumpPad::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
}

void AJumpPad::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (BottomBoxComponent)
	{
		BottomBoxComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AJumpPad::OnBoxComponentBeginOverlapEvent);
		BottomBoxComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &AJumpPad::OnBoxComponentEndOverlapEvent);
	}

	if (TopBoxComponent)
	{
		TopBoxComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AJumpPad::OnBoxComponentBeginOverlapEvent);
		TopBoxComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &AJumpPad::OnBoxComponentEndOverlapEvent);
	}
}

void AJumpPad::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (OverlappedPlayer)
	{
		const FVector& curLoc = OverlappedPlayer->GetActorLocation();
		switch (JumpMoveType)
		{
		case EJumpMoveType::UP:
			{
				UKismetSystemLibrary::PrintString(this, FString::SanitizeFloat(FVector::Dist(curLoc, MoveTargetPoint)));
				//상단 포인트에 도착했는지 확인합니다.
				if (JumpProcess == EJumpProcess::JUMPING)
				{
					if (FVector::Dist(curLoc, MoveTargetPoint) <= MoveCompleteCheckDistance)
					{
						UKismetSystemLibrary::PrintString(this,TEXT("멈춰!!!!!!!!!!!!!!!!!!!!!!!!!!!"), true, true,
						                                  FColor::Red);
						//도착했으면 즉시 멈추고,
						OverlappedPlayer->GetCharacterMovement()->StopMovementImmediately();
						SetActorTickEnabled(false);
						//착지를 시도합니다.
						JumpDown();
					}
				}

				if (JumpProcess == EJumpProcess::JUMP_LAND)
				{
					UKismetSystemLibrary::PrintString(
						this, FString::SanitizeFloat(FVector::Dist(curLoc, MoveTargetPoint)));
					if (FVector::Dist(curLoc, MoveTargetPoint) <= MoveCompleteCheckDistance)
					{
						//도착했으면 즉시 멈추고,
						OverlappedPlayer->GetCharacterMovement()->StopMovementImmediately();
						SetActorTickEnabled(false);
						JumpLand();
					}
				}
			}
			break;
		case EJumpMoveType::DOWN:
			{
				UKismetSystemLibrary::PrintString(this, FString::SanitizeFloat(FVector::Dist(curLoc, MoveTargetPoint)));
				//상단 포인트에 도착했는지 확인합니다.
				if (JumpProcess == EJumpProcess::JUMPING)
				{
					if (FVector::Dist(curLoc, MoveTargetPoint) <= MoveCompleteCheckDistance)
					{
						//도착했으면 즉시 멈추고,
						OverlappedPlayer->GetCharacterMovement()->StopMovementImmediately();
						SetActorTickEnabled(false);
						//착지를 시도합니다.
						JumpDown();
					}
				}

				if (JumpProcess == EJumpProcess::JUMP_LAND)
				{
					if (FVector::Dist(curLoc, MoveTargetPoint) <= MoveCompleteCheckDistance)
					{
						//도착했으면 즉시 멈추고,
						OverlappedPlayer->GetCharacterMovement()->StopMovementImmediately();
						SetActorTickEnabled(false);
						JumpLand();
					}
				}
			}

			break;
		case EJumpMoveType::NONE:
			{
				if (bIsActivated)
				{
					if (CurOverlappedComponent == TopBoxComponent)
					{
						if (UMathHelperLibrary::SameDirection(OverlappedPlayer->GetActorForwardVector(),
						                                      TopBoxComponent->GetForwardVector()))
						{
							ShowInteractionWidget()->SetWorldLocation(TopBoxComponent->GetComponentLocation());
						}
						else
						{
							HideInteractionWidget();
						}
					}

					if (CurOverlappedComponent == BottomBoxComponent)
					{
						if (!UMathHelperLibrary::SameDirection(OverlappedPlayer->GetActorForwardVector(),
						                                       BottomBoxComponent->GetForwardVector()))
						{
							ShowInteractionWidget()->SetWorldLocation(BottomBoxComponent->GetComponentLocation());
						}
						else
						{
							HideInteractionWidget();
						}
					}
				}
			}
			break;
		}
	}
}

void AJumpPad::TryActivatePad()
{
	if (OverlappedPlayer)
	{
		USceneComponent* targetComp = nullptr;
		if (TopBoxComponent->IsOverlappingActor(OverlappedPlayer))
		{
			targetComp = TopBoxComponent;
			//윗쪽이랑 석상이랑 가깝나요?
			if (!IsNearActivateMesh(TopBoxComponent, BottomBoxComponent))
			{
				return;
			}
		}
		else
		{
			targetComp = BottomBoxComponent;

			//아랫쪽이랑 석상이 가깝나요?
			if (!IsNearActivateMesh(BottomBoxComponent, TopBoxComponent))
			{
				return;
			}
		}

		//가까운쪽에 있다면, 활성화 합니다.
		if (targetComp != nullptr)
		{
			FLatentActionInfo latentInfo;
			latentInfo.CallbackTarget = this;
			latentInfo.Linkage = 2;
			latentInfo.ExecutionFunction = "StartActivatePad";

			auto capsuleComp = OverlappedPlayer->GetCapsuleComponent();
			//스테틱 메시로 회전할 회전값
			auto rot = UKismetMathLibrary::FindLookAtRotation(OverlappedPlayer->GetActorLocation(),
			                                                  ActivateMeshComponent->GetComponentLocation());

			UKismetSystemLibrary::MoveComponentTo(capsuleComp,
			                                      FVector(targetComp->GetComponentLocation().X,
			                                              targetComp->GetComponentLocation().Y,
			                                              OverlappedPlayer->GetActorLocation().Z),
			                                      FRotator(0, rot.Yaw, 0), false, false,
			                                      0.2f, false, EMoveComponentAction::Move, latentInfo);
		}
	}
}

void AJumpPad::JumpMove()
{
	if (OverlappedPlayer)
	{
		JumpProcess = EJumpProcess::JUMPING;
		UKismetSystemLibrary::PrintString(this,TEXT("점프시작"));

		switch (JumpMoveType)
		{
		case EJumpMoveType::UP:
			{
				//착지할 상단 위치에서 1/3지점을 구합니다.
				FVector locA = FVector(OverlappedPlayer->GetActorLocation().X, OverlappedPlayer->GetActorLocation().Y,
				                       TopBoxComponent->GetComponentLocation().Z);
				FVector locB = TopBoxComponent->GetComponentLocation() + FVector(0, 0, 200.f);
				FVector AToB = locB - locA;
				MoveTargetPoint = locB - AToB / 3.f;
			}
			break;
		case EJumpMoveType::DOWN:
			{
				FVector locA = FVector(BottomBoxComponent->GetComponentLocation().X,
				                       BottomBoxComponent->GetComponentLocation().Y,
				                       OverlappedPlayer->GetActorLocation().Z);
				FVector locB = TopBoxComponent->GetComponentLocation() + FVector(0, 0, 200.f);
				FVector AToB = locB - locA;
				MoveTargetPoint = locB - AToB * 2 / 3.f;
			}
			break;
		case EJumpMoveType::NONE:
			break;
		default: ;
		}


		//캐릭터가 이동할 방향을 구합니다.
		FVector launchVelocity = (MoveTargetPoint - OverlappedPlayer->GetActorLocation()).GetSafeNormal();
		launchVelocity *= MoveSpeed;

		//상단 포인트에 도착했는지 확인하기 위해 틱을 활성화 합니다.
		SetActorTickEnabled(true);
		//중력을 끄고,
		OverlappedPlayer->GetCharacterMovement()->GravityScale = 0;
		//발사합니다.
		OverlappedPlayer->LaunchCharacter(launchVelocity, true, true);
	}
	else
	{
		UKismetSystemLibrary::PrintString(this,TEXT("플레이어캐릭터가 널포인터입니다."));
	}
}

void AJumpPad::JumpDown()
{
	if (OverlappedPlayer)
	{
		JumpProcess = EJumpProcess::JUMP_LAND;

		switch (JumpMoveType)
		{
		case EJumpMoveType::UP:
			{
				MoveTargetPoint = TopBoxComponent->GetComponentLocation();
				SetActorTickEnabled(true);
				const FVector& launchVelocity = (TopBoxComponent->GetComponentLocation() - OverlappedPlayer->
					GetActorLocation()).GetSafeNormal() * MoveSpeed;
				OverlappedPlayer->LaunchCharacter(launchVelocity, true, true);
			}
			break;
		case EJumpMoveType::DOWN:
			{
				MoveTargetPoint = BottomBoxComponent->GetComponentLocation();
				SetActorTickEnabled(true);
				const FVector& launchVelocity = (BottomBoxComponent->GetComponentLocation() - OverlappedPlayer->
					GetActorLocation()).GetSafeNormal() * MoveSpeed;
				OverlappedPlayer->LaunchCharacter(launchVelocity, true, true);
			}
			break;
		case EJumpMoveType::NONE:
			break;
		default: ;
		}
	}
	else
	{
		UKismetSystemLibrary::PrintString(this,TEXT("플레이어캐릭터가 널포인터입니다."));
	}
}

void AJumpPad::JumpLand()
{
	JumpProcess = EJumpProcess::JUMP_END;
	JumpMoveType = EJumpMoveType::NONE;

	OverlappedPlayer->GetCharacterMovement()->GravityScale = 1.f;
	OverlappedPlayer->GetCharacterMovement()->StopMovementImmediately();
	OverlappedPlayer->GetJumpMovementComponent()->PlayJumpLandMontage();

	OverlappedPlayer->SetIgnoreMoveInput(false, this,LADDER_IGNORE_MOVE_TAG);
	UKismetSystemLibrary::PrintString(this,TEXT("점프 종료"));

	Execute_FinishInteraction(this);
}

void AJumpPad::Interaction_Implementation(ABaseCharacter* Start)
{
	if (!bIsActivated)
	{
		TryActivatePad();
	}
	else
	{
		if (OverlappedPlayer)
		{
			JumpProcess = EJumpProcess::JUMP_START;
			OverlappedPlayer->SetIgnoreMoveInput(true, this,LADDER_IGNORE_MOVE_TAG);
			auto capsuleComp = OverlappedPlayer->GetCapsuleComponent();

			FLatentActionInfo latentInfo;
			latentInfo.CallbackTarget = this;
			latentInfo.Linkage = 1;
			latentInfo.ExecutionFunction = "PlayJumpStartAnimation";

			//아래면 위 컴포넌트를 바라보도록
			if (CurOverlappedComponent == BottomBoxComponent)
			{
				UKismetSystemLibrary::PrintString(this,TEXT("UP"));
				JumpMoveType = EJumpMoveType::UP;
				float yaw = UKismetMathLibrary::FindLookAtRotation(OverlappedPlayer->GetActorLocation(),
				                                                   TopBoxComponent->GetComponentLocation()).Yaw;
				UKismetSystemLibrary::MoveComponentTo(capsuleComp,
				                                      FVector(BottomBoxComponent->GetComponentLocation().X,
				                                              BottomBoxComponent->GetComponentLocation().Y,
				                                              OverlappedPlayer->GetActorLocation().Z),
				                                      FRotator(0, yaw, 0), false, false,
				                                      0.2f, false, EMoveComponentAction::Move, latentInfo);
			}

			//위라면 아래 컴포넌트를 바라보도록
			if (CurOverlappedComponent == TopBoxComponent)
			{
				JumpMoveType = EJumpMoveType::DOWN;
				UKismetSystemLibrary::PrintString(this,TEXT("DOWN"));
				float yaw = UKismetMathLibrary::FindLookAtRotation(OverlappedPlayer->GetActorLocation(),
				                                                   BottomBoxComponent->GetComponentLocation()).Yaw;
				UKismetSystemLibrary::MoveComponentTo(capsuleComp,
				                                      FVector(TopBoxComponent->GetComponentLocation().X,
				                                              TopBoxComponent->GetComponentLocation().Y,
				                                              OverlappedPlayer->GetActorLocation().Z),
				                                      FRotator(0, yaw, 0), false, false,
				                                      0.2f, false, EMoveComponentAction::Move, latentInfo);
			}
		}
	}
}

void AJumpPad::FinishInteraction_Implementation()
{
}

void AJumpPad::OnBoxComponentBeginOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                               const FHitResult& SweepResult)
{
	if (OtherActor->IsA<APlayerCharacter>())
	{
		UKismetSystemLibrary::PrintString(this,TEXT("겹침"));
		CurOverlappedComponent = OverlappedComponent;
		OverlappedPlayer = Cast<APlayerCharacter>(OtherActor);
		SetActorTickEnabled(true);

		if (OverlappedComponent == TopBoxComponent)
		{
			if (!UMathHelperLibrary::SameDirection(OverlappedPlayer->GetActorForwardVector(),
			                                       TopBoxComponent->GetForwardVector()))
			{
				UKismetSystemLibrary::PrintString(this,TEXT("다른 방향"));
				if (!bIsActivated)
				{
					//이 위치가 활성화 스테틱 매시 근처인지 확인합니다.
					if (IsNearActivateMesh(TopBoxComponent, BottomBoxComponent))
					{
						ShowInteractionWidget()->SetWorldLocation(TopBoxComponent->GetComponentLocation());
					}
				}
				else
				{
					ShowInteractionWidget()->SetWorldLocation(TopBoxComponent->GetComponentLocation());
				}
			}
		}

		if (OverlappedComponent == BottomBoxComponent)
		{
			if (!UMathHelperLibrary::SameDirection(OverlappedPlayer->GetActorForwardVector(),
			                                       BottomBoxComponent->GetForwardVector()))
			{
				UKismetSystemLibrary::PrintString(this,TEXT("다른 방향"));
				if (!bIsActivated)
				{
					//이 위치가 활성화 스테틱 매시 근처인지 확인합니다.
					if (IsNearActivateMesh(BottomBoxComponent, TopBoxComponent))
					{
						ShowInteractionWidget()->SetWorldLocation(BottomBoxComponent->GetComponentLocation());
					}
				}
				else
				{
					ShowInteractionWidget()->SetWorldLocation(BottomBoxComponent->GetComponentLocation());
				}
			}
		}
	}
}

void AJumpPad::OnBoxComponentEndOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == OverlappedPlayer)
	{
		if (JumpProcess == EJumpProcess::NONE)
		{
			UKismetSystemLibrary::PrintString(this,TEXT("플레이어 포인터 비움!!!!"));
			CurOverlappedComponent = nullptr;
			HideInteractionWidget();
			OverlappedPlayer = nullptr;
			SetActorTickEnabled(false);
		}
	}
}

UWidgetComponent* AJumpPad::ShowInteractionWidget()
{
	if (OverlappedPlayer)
	{
		return OverlappedPlayer->ShowInteractionWidget(this, OverlappedPlayer->GetInputDataAsset()->InteractAction,
		                                               Execute_GetActionName(this));
	}

	return nullptr;
}

void AJumpPad::HideInteractionWidget()
{
	if (OverlappedPlayer)
	{
		OverlappedPlayer->HideInteractionWidget();
	}
}

bool AJumpPad::IsNearActivateMesh(UPrimitiveComponent* CheckTargetComponent, UPrimitiveComponent* CompareComponent)
{
	if (OverlappedPlayer)
	{
		const float& targetDistance = (ActivateMeshComponent->GetComponentLocation() - CheckTargetComponent->
			GetComponentLocation()).Length();
		const float& compDistance = (ActivateMeshComponent->GetComponentLocation() - CompareComponent->
			GetComponentLocation()).Length();

		return targetDistance <= compDistance;
	}

	return false;
}

void AJumpPad::PlayJumpStartAnimation()
{
	if (OverlappedPlayer)
	{
		OverlappedPlayer->GetJumpMovementComponent()->PlayJumpStartMontage(this);
	}
}

void AJumpPad::StartActivatePad()
{
	if (OverlappedPlayer)
	{
		OverlappedPlayer->GetJumpMovementComponent()->ActivateJumpPad(this);
	}
}

void AJumpPad::SetActivate()
{
	bIsActivated = true;
	ActivateParticleComponent->SetVisibility(true);
	ShowInteractionWidget();
}
