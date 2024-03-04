// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/00_Player/01_Component/LadderMovementComponent.h"

#include "92_Tools/Ladder.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
ULadderMovementComponent::ULadderMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULadderMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Pawn = GetOwner<ABaseCharacter>();
}


// Called every frame
void ULadderMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 틱으로 캐릭터의 위치를 체크해서 사다리에서 언제 나올지 체크합니다.
}

void ULadderMovementComponent::SetUseLadderMovement(bool Value, ALadder* LadderToUse)
{
	
	bUseLadderMovement = Value;
	LadderActor = LadderToUse;

	if (bUseLadderMovement)
	{
		SetComponentTickEnabled(true);
		//사다리를 오르려고 하나요?(첫 상호작용을 하면 오르는 것입니다. 캐릭터의 위치와, 사다리의 아래, 위 진입포인트를 비교합니다.)
		float btm = FVector::Distance(LadderActor->GetEnterBottom()->GetComponentLocation(), Pawn->GetActorLocation());
		float top = FVector::Distance(LadderActor->GetEnterTop()->GetComponentLocation(), Pawn->GetActorLocation());

		Pawn->GetMesh()->GetAnimInstance()->OnMontageBlendingOut.AddUniqueDynamic(this,&ULadderMovementComponent::OnPlayMontageBlendOutEvent);
		//아래에서 올라가려고 하는 경우
		if (btm < top)
		{
			Pawn->PlayAnimMontage(LadderEnter_BottomMontage);
		}
		//위에서 내려가려고 하는 경우
		else
		{
			Pawn->PlayAnimMontage(LadderEnter_TopMontage);
		}
	}
	else
	{
		SetComponentTickEnabled(false);
		bIsMoving = false;
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
			if(LadderMoveType == ELadderMove::DOWN)
			{
				startFirst = Pawn->GetActorLocation() + FVector(0, 0, 100.f) + FVector(0, 0, i * -10);
			}else
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
				DrawDebugPoint(GetWorld(),hitFirst.ImpactPoint,50.f,FColor::Emerald);
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

FVector ULadderMovementComponent::GetNextHandLocation()
{
	if (Pawn)
	{
		for (auto i = 1; i <= 6; i++)
		{
			
			FVector startFirst;
			if(LadderHandOrder == ELadderHandOrder::LEFT)
			{
				if(LadderMoveType == ELadderMove::DOWN)
				{
					startFirst = Pawn->GetMesh()->GetSocketLocation("hand_l") + FVector(0, 0, -1 * FindNextDownHandLocation_ZOffset) + FVector(0, 0, i * -10);
				}else
				{
					startFirst = Pawn->GetMesh()->GetSocketLocation("hand_l") + FVector(0, 0, FindNextDownHandLocation_ZOffset) + FVector(0, 0, i * 10);
				}
			}else
			{
				if(LadderMoveType == ELadderMove::DOWN)
				{
					startFirst = Pawn->GetMesh()->GetSocketLocation("hand_r") + FVector(0, 0, -1 * FindNextUpHandLocation_ZOffset) + FVector(0, 0, i * -10);
				}else
				{
					startFirst = Pawn->GetMesh()->GetSocketLocation("hand_r") + FVector(0, 0, FindNextUpHandLocation_ZOffset) + FVector(0, 0, i * 10);
				}
				
			}
			
			const FVector& endFirst = startFirst + UKismetMathLibrary::GetForwardVector(Pawn->GetActorRotation()) * 50.f;

			TArray<AActor*> ignoreActors;
			ignoreActors.Emplace(Pawn);

			FHitResult hitFirst;
			//플레이어 전방 뱡향으로 머리위에서 트레이스를 몇개 차곡차곡 그립니다.
			if (UKismetSystemLibrary::SphereTraceSingle(this, startFirst, endFirst, 5.f,
														UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
														ignoreActors, EDrawDebugTrace::ForDuration, hitFirst, true))
			{
				DrawDebugPoint(GetWorld(),hitFirst.ImpactPoint,25.f,FColor::Emerald,true,5.f);
				return hitFirst.ImpactPoint + HandIKOffset;
			}
		}
	}

	return FVector::ZeroVector;
}

void ULadderMovementComponent::AddLadderMovementInput(float ScaleValue)
{
	if(ScaleValue == 0)
	{
		return;
	}

	if(Pawn->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
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

		UKismetSystemLibrary::PrintString(this,FString::Printf(TEXT("사용중인 손 : %s"),*StaticEnum<ELadderHandOrder>()->GetValueAsString(LadderHandOrder)));
		FVector moveLocation;
		if(ScaleValue>0)
		{
			LadderMoveType = ELadderMove::UP;
			moveLocation = GetClimbMoveLocation();
		}else
		{
			LadderMoveType = ELadderMove::DOWN;
			moveLocation = GetClimbMoveLocation();

			//더 이상 내려갈 수 없으면 탈출합니다.
		}


		if(moveLocation == FVector::ZeroVector)
		{
			//마지막까지 올라간것이 됩니다.
			Pawn->GetMesh()->GetAnimInstance()->OnMontageBlendingOut.AddUniqueDynamic(this,&ULadderMovementComponent::OnPlayMontageBlendOutEvent);
			Pawn->PlayAnimMontage(LadderExit_TopMontage);
			return;
		}

		
		UKismetSystemLibrary::MoveComponentTo(Pawn->GetCapsuleComponent(), moveLocation,
											  LadderActor->GetActorRotation(), false, false, MoveTime , false,
											  EMoveComponentAction::Move, latentInfo);
	}
}

void ULadderMovementComponent::OnPlayMontageBlendOutEvent(UAnimMontage* Montage, bool bInterrupted)
{
	if(Montage == LadderEnter_BottomMontage)
	{
		Pawn->GetMesh()->GetAnimInstance()->OnMontageBlendingOut.RemoveDynamic(this,&ULadderMovementComponent::OnPlayMontageBlendOutEvent);
	}
	
	if(Montage == LadderExit_TopMontage)
	{
	
		if(LadderActor)
		{
			IInteractionInterface::Execute_FinishInteraction(LadderActor);
			LadderActor = nullptr;
		}
		Pawn->GetMesh()->GetAnimInstance()->OnMontageBlendingOut.RemoveDynamic(this,&ULadderMovementComponent::OnPlayMontageBlendOutEvent);
		
	}
}

float ULadderMovementComponent::GetLadderGap()
{
	if(LadderActor!=nullptr)
	{
		return LadderActor->LadderGap;
	}

	return 0;
}
