// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/00_Player/01_Component/LadderMovementComponent.h"

#include "92_Tools/Ladder.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameplayTagContainer.h"

#define LADDER_IGNORE_MOVE_TAG FGameplayTag::RequestGameplayTag("Common.PlayerInput.Ignore.Move.FromLadder")

// Sets default values for this component's properties
ULadderMovementComponent::ULadderMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void ULadderMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Pawn = GetOwner<ABaseCharacter>();
}

void ULadderMovementComponent::SetUseLadderMovement(bool Value)
{
	if(Pawn)
	{
		bUseLadderMovement = Value;
		Pawn->GetMesh()->GetAnimInstance()->OnMontageBlendingOut.AddUniqueDynamic(
						this, &ULadderMovementComponent::OnPlayMontageBlendOutEvent);
		if (bUseLadderMovement)
		{
			Pawn->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		}
		else
		{
			bIsMoving = false;
		}
	}
}

void ULadderMovementComponent::MoveFinish()
{
	bIsMoving = false;
}

FVector ULadderMovementComponent::GetClimbMoveLocation()
{
	if (Pawn)
	{
		for (auto i = 1; i <= 6; i++)
		{
			FVector startFirst;
			if (LadderMoveType == ELadderMove::DOWN)
			{
				startFirst = Pawn->GetActorLocation() + FVector(0, 0, 100.f) + FVector(0, 0, i * -10);
			}
			else
			{
				startFirst = Pawn->GetActorLocation() + FVector(0, 0, 120.f) + FVector(0, 0, i * 10);
			}

			const FVector& endFirst = startFirst + UKismetMathLibrary::GetForwardVector(Pawn->GetActorRotation()) *
				50.f;

			TArray<AActor*> ignoreActors;
			ignoreActors.Emplace(Pawn);

			FHitResult hitFirst;
			//플레이어 전방 뱡향으로 머리위에서 트레이스를 몇개 차곡차곡 그립니다.
			if (UKismetSystemLibrary::SphereTraceSingle(this, startFirst, endFirst, 5.f,
			                                            UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
			                                            ignoreActors, EDrawDebugTrace::ForDuration, hitFirst, true))
			{
				DrawDebugPoint(GetWorld(), hitFirst.ImpactPoint, 50.f, FColor::Emerald);
				if (hitFirst.ImpactNormal.Z < 0.1f)
				{
					const FVector& start = hitFirst.ImpactPoint + FVector(0, 0, 20);
					const FVector& end = start - FVector(0, 0, 25.f);

					FHitResult hit;
					//그린 트레이스중 적중한 위치로부터 다시 아래로 트레이스를 그려 최종 이동해야 할 좌표를 계산합니다.
					if (UKismetSystemLibrary::SphereTraceSingle(this, start, end, 5.f,
					                                            UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
					                                            ignoreActors, EDrawDebugTrace::None, hit, true))
					{
						UKismetSystemLibrary::PrintString(this,hit.GetActor()->GetActorNameOrLabel());
						if (!hit.bStartPenetrating)
						{
							FVector resultVector;

							resultVector = hit.ImpactPoint + UKismetMathLibrary::GetForwardVector(
								UKismetMathLibrary::MakeRotFromX(hitFirst.ImpactNormal)) * 30.f;
							resultVector.Z = hit.ImpactPoint.Z - LadderMoveOffset;

							return resultVector;
						}
					}
				}
			}
		}
	}

	return FVector::ZeroVector;
}

bool ULadderMovementComponent::CanDown()
{
	if (Pawn)
	{
		for (auto i = 1; i <= 6; i++)
		{
			const FVector& startFirst = Pawn->GetActorLocation() - FVector(0, 0, 100.f) + FVector(0, 0, i * -10);
			const FVector& endFirst = startFirst + UKismetMathLibrary::GetForwardVector(Pawn->GetActorRotation()) *
				50.f;

			TArray<AActor*> ignoreActors;
			ignoreActors.Emplace(Pawn);

			FHitResult hitFirst;
			//플레이어 전방 뱡향으로 아래 방향으로 트레이스를 몇개 차곡차곡 그립니다.
			if (UKismetSystemLibrary::SphereTraceSingle(this, startFirst, endFirst, 5.f,
			                                            UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
			                                            ignoreActors, EDrawDebugTrace::ForDuration, hitFirst, true))
			{
				if (hitFirst.GetActor() == LadderActor)
				{
					return true;
				}
			}
		}
	}

	return false;
}

void ULadderMovementComponent::ReadyToLadderClimbUp(ALadder* Ladder)
{
	LadderActor = Ladder;
	LadderClimbType = ELadderClimbType::EnterFromBottom;

	Pawn->SetIgnoreMoveInput(true, Ladder,LADDER_IGNORE_MOVE_TAG);
	Pawn->GetCharacterMovement()->Velocity = FVector::Zero();
	Pawn->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

	Pawn->PlayAnimMontage(LadderEnter_BottomMontage);

	FLatentActionInfo latentInfo;
	latentInfo.Linkage = 1;
	latentInfo.CallbackTarget = this;
	latentInfo.ExecutionFunction = "ActivateLadderClimbMode";
	auto capsuleComp = Pawn->GetCapsuleComponent();
	capsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//타고 올라가는 설정입니다.
	FVector targetLocation = FVector(0, 0, capsuleComp->GetScaledCapsuleHalfHeight()) + Ladder->GetActorLocation();
	UKismetSystemLibrary::MoveComponentTo(capsuleComp, targetLocation, Ladder->GetActorRotation(), false, false,
	                                      ClimbTime, false, EMoveComponentAction::Move, latentInfo);
}

void ULadderMovementComponent::ReadyToLadderClimbDown(ALadder* Ladder)
{
	LadderActor = Ladder;
	LadderClimbType = ELadderClimbType::EnterFromTop;

	Pawn->SetIgnoreMoveInput(true, Ladder,LADDER_IGNORE_MOVE_TAG);
	Pawn->GetCharacterMovement()->Velocity = FVector::Zero();
	Pawn->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

	Pawn->PlayAnimMontage(LadderEnter_TopMontage);

	FLatentActionInfo latentInfo;
	latentInfo.Linkage = 1;
	latentInfo.CallbackTarget = this;
	latentInfo.ExecutionFunction = "ActivateLadderClimbMode";

	auto capsuleComp = Pawn->GetCapsuleComponent();
	capsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//위치를 맞추기 위해서 전방백터에서 일부를 곱해서 제거합니다.
	const FVector& topLoc = LadderActor->GetEnterTop()->GetComponentLocation() + Pawn->GetActorForwardVector() * -15.f;
	UKismetSystemLibrary::MoveComponentTo(capsuleComp, topLoc, Ladder->GetActorRotation(), false, false,
										  ClimbTime, false, EMoveComponentAction::Move, latentInfo);
}

ELadderMove ULadderMovementComponent::GetClimbLadderType(ALadder* Ladder)
{
	float btm = FVector::Distance(Ladder->GetEnterBottom()->GetComponentLocation(), Pawn->GetActorLocation());
	float top = FVector::Distance(Ladder->GetEnterTop()->GetComponentLocation(), Pawn->GetActorLocation());

	//아래에서 올라가려고 하는 경우
	if (btm < top)
	{
		return ELadderMove::UP;
	}
	//위에서 내려가려고 하는 경우
	return ELadderMove::DOWN;
}

FVector ULadderMovementComponent::GetNextHandLocation()
{
	if (Pawn)
	{
		for (auto i = 1; i <= 6; i++)
		{
			FVector startFirst;
			if (LadderHandOrder == ELadderHandOrder::LEFT)
			{
				if (LadderMoveType == ELadderMove::DOWN)
				{
					startFirst = Pawn->GetMesh()->GetSocketLocation("hand_l") + FVector(
						0, 0, -1 * FindNextDownHandLocation_ZOffset) + FVector(0, 0, i * -10);
				}
				else
				{
					startFirst = Pawn->GetMesh()->GetSocketLocation("hand_l") + FVector(
						0, 0, FindNextDownHandLocation_ZOffset) + FVector(0, 0, i * 10);
				}
			}
			else
			{
				if (LadderMoveType == ELadderMove::DOWN)
				{
					startFirst = Pawn->GetMesh()->GetSocketLocation("hand_r") + FVector(
						0, 0, -1 * FindNextUpHandLocation_ZOffset) + FVector(0, 0, i * -10);
				}
				else
				{
					startFirst = Pawn->GetMesh()->GetSocketLocation("hand_r") + FVector(
						0, 0, FindNextUpHandLocation_ZOffset) + FVector(0, 0, i * 10);
				}
			}

			const FVector& endFirst = startFirst + UKismetMathLibrary::GetForwardVector(Pawn->GetActorRotation()) *
				50.f;

			TArray<AActor*> ignoreActors;
			ignoreActors.Emplace(Pawn);

			FHitResult hitFirst;
			//플레이어 전방 뱡향으로 머리위에서 트레이스를 몇개 차곡차곡 그립니다.
			if (UKismetSystemLibrary::SphereTraceSingle(this, startFirst, endFirst, 5.f,
			                                            UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
			                                            ignoreActors, EDrawDebugTrace::ForDuration, hitFirst, true))
			{
				DrawDebugPoint(GetWorld(), hitFirst.ImpactPoint, 25.f, FColor::Emerald, true, 5.f);
				return hitFirst.ImpactPoint + HandIKOffset;
			}
		}
	}

	return FVector::ZeroVector;
}

void ULadderMovementComponent::AddLadderMovementInput(float ScaleValue)
{
	if (ScaleValue == 0)
	{
		return;
	}

	if (Pawn->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		return;
	}

	if (!bIsMoving)
	{
		bIsMoving = true;

		FLatentActionInfo latentInfo;
		latentInfo.CallbackTarget = this;
		latentInfo.ExecutionFunction = "MoveFinish";
		latentInfo.Linkage = 0;

		FVector moveLocation;
		if (ScaleValue > 0)
		{
			LadderMoveType = ELadderMove::UP;
			moveLocation = GetClimbMoveLocation();
		}
		else
		{
			LadderMoveType = ELadderMove::DOWN;
			moveLocation = GetClimbMoveLocation();

			//더 이상 내려갈 수 없으면 탈출합니다.
			if (!CanDown())
			{
				LadderClimbType = ELadderClimbType::EscapeFromBottom;
				Pawn->PlayAnimMontage(LadderExit_BottomMontage);
				return;
			}
		}

		if(GetNextHandLocation() == FVector::ZeroVector)
		{
			LadderClimbType = ELadderClimbType::EscapeFromTop;
			Pawn->PlayAnimMontage(LadderExit_TopMontage);
			return;
		}

		if (moveLocation == FVector::ZeroVector)
		{
			LadderClimbType = ELadderClimbType::EscapeFromTop;
			Pawn->PlayAnimMontage(LadderExit_TopMontage);
			return;
		}


		UKismetSystemLibrary::MoveComponentTo(Pawn->GetCapsuleComponent(), moveLocation,
		                                      LadderActor->GetActorRotation(), false, false, MoveTime, false,
		                                      EMoveComponentAction::Move, latentInfo);
	}
}

void ULadderMovementComponent::OnPlayMontageBlendOutEvent(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == LadderExit_TopMontage || Montage == LadderExit_BottomMontage)
	{
		if (LadderActor)
		{
			auto capsuleComp = Pawn->GetCapsuleComponent();
			capsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			
			Pawn->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			IInteractionInterface::Execute_FinishInteraction(LadderActor);
			LadderActor = nullptr;
		}

		Pawn->GetMesh()->GetAnimInstance()->OnMontageBlendingOut.RemoveAll(this);
	}
}

void ULadderMovementComponent::ActivateLadderClimbMode()
{
	Pawn->SetIgnoreMoveInput(false, LadderActor,LADDER_IGNORE_MOVE_TAG);
	SetUseLadderMovement(true);
}
