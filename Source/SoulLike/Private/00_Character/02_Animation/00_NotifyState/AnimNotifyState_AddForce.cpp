// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/02_Animation/00_NotifyState/AnimNotifyState_AddForce.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/00_Player/00_Controller/UserController.h"
#include "00_Character/00_Player/00_Controller/00_Component/InputHandlerComponent.h"
#include "00_Character/00_Player/01_Component/AbilityTalentComponent.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "96_Library/AbilityHelperLibrary.h"
#include "96_Library/MathHelperLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Logging/StructuredLog.h"
#include "SoulLike/SoulLike.h"


UAnimNotifyState_AddForce::UAnimNotifyState_AddForce()
{
	bIsNativeBranchingPoint = false;
#if WITH_EDITOR
	bShouldFireInEditor = false;
#endif
}


void UAnimNotifyState_AddForce::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                            float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (MeshComp != nullptr)
	{
		Owner = MeshComp->GetOwner<ABaseCharacter>();
		if (Owner.IsValid())
		{
			if (bDisableMoveInput)
			{
				if (Owner->IsA<APlayerCharacter>())
				{
					UE_LOGFMT(LogTemp, Warning, "{0} {1} : 플레이어 입력을 막습니다, 힘 추가 노티파이 ", __FUNCTION__, __LINE__);
					Owner->SetIgnoreMoveInput(true, Owner.Get(),
					                          FGameplayTag::RequestGameplayTag(
						                          "Common.Passive.IgnoreMoveInput.Animation.NotifyState.AddForce"));
				}
			}

			if (IsFallOfTheCliff())
			{
				Owner->GetCharacterMovement()->StopMovementImmediately();
			}
			else
			{
				switch (ForceApplicationMode)
				{
				case EForceApplicationMode::InputDirection:
					SaveStartControllerInput();
					break;
				case EForceApplicationMode::CustomDirectionSetting:
					if (!bUseControllerDir)
					{
						if (ForceDirection == EDirection::Up)
						{
							Owner->GetCharacterMovement()->GravityScale = 0;
						}
					}
					SaveMoveDirection();
					break;
				default: ;
				}

				/*
				if (IsPlayerCharacter())
				{
				
			
				}
				else
				{
					SaveMoveDirection();
				}*/
			}
		}
	}
}

void UAnimNotifyState_AddForce::StopMovementWhenFalling()
{
	//아래로 트레이스를 그려 뭔가 없으면 중지하도록 합시다.
	const float& halfHeight = Owner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float& radius = Owner->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector& targetLoc = Owner->GetActorLocation() - FVector(0, 0, halfHeight);

	FHitResult hit;
	if (!UKismetSystemLibrary::CapsuleTraceSingle(Owner.Get(), targetLoc, targetLoc, radius, halfHeight,
	                                              UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
	                                              TArray<AActor*>(),
	                                              EDrawDebugTrace::ForOneFrame, hit, true))
	{
		Owner->GetCharacterMovement()->StopMovementImmediately();
	}
}

void UAnimNotifyState_AddForce::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (Owner.IsValid() && Owner->GetCharacterMovement())
	{
		if (IsFallOfTheCliff())
		{
			Owner->GetCharacterMovement()->StopMovementImmediately();
		}
		else
		{
			//특정 상태이상에 걸려서 움직이지 못 하는 상황인지 확인합니다.
			if (UAbilityHelperLibrary::IsMovementBlockedByStatusEffect(Owner.Get()) == false)
			{
				switch (ForceApplicationMode)
				{
				case EForceApplicationMode::InputDirection:
					ApplyForceByInputDirection(FrameDeltaTime);
					break;
				case EForceApplicationMode::CustomDirectionSetting:
					ApplyForceByDirectionValue(FrameDeltaTime);
					break;
				default: ;
				}

				StopMovementWhenFalling();
			}
		}
	}
}

void UAnimNotifyState_AddForce::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                          const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (Owner.IsValid())
	{
		if (bDisableMoveInput)
		{
			if (Owner->IsA<APlayerCharacter>())
			{
				UE_LOGFMT(LogTemp, Log, "{0} {1} : 플레이어 입력을 되돌립니다, 힘 추가 노티파이 ", __FUNCTION__, __LINE__);
				Owner->SetIgnoreMoveInput(false, Owner.Get(),
				                          FGameplayTag::RequestGameplayTag(
					                          "Common.Passive.IgnoreMoveInput.Animation.NotifyState.AddForce"));
			}
		}
	}
}

bool UAnimNotifyState_AddForce::IsFallOfTheCliff() const
{
	if (bCheckCliff == false)
	{
		return false;
	}

	if (Owner.IsValid())
	{
		const float& halfHeight = Owner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		const float& radius = Owner->GetCapsuleComponent()->GetScaledCapsuleRadius();

		auto forwardLoc = Owner->GetActorLocation() + (Owner->GetActorForwardVector() * radius * 2);
		auto targetLoc = forwardLoc - FVector(0, 0, halfHeight / 2);


		TArray<TEnumAsByte<EObjectTypeQuery>> typeToQuery;
		typeToQuery.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

		TArray<AActor*> ignoreActors;
		FHitResult hit;
		//캐릭터 바로 앞 지점에 캡슐 트레이스를 생성하고, 캡슐 반의 반 높이만큼 내린다.
		if (!UKismetSystemLibrary::CapsuleTraceSingleForObjects(Owner.Get(), targetLoc, targetLoc, radius, halfHeight,
		                                                        typeToQuery, false, ignoreActors,
		                                                        EDrawDebugTrace::ForOneFrame, hit, true))
		{
			UE_LOGFMT(LogTemp, Log, "낭떠러지 감지");
			return true;
		}
		//바닥에 걸리는게 아무것도 없다면, 낭떨어지 판정
	}
	return false;
}

void UAnimNotifyState_AddForce::SaveStartControllerInput()
{
	if (Owner.IsValid() && Owner->IsA<APlayerCharacter>())
	{
		auto player = Cast<APlayerCharacter>(Owner);

		if (player->GetAbilityComponent()->IsAlreadyActivated(LockOnTag))
		{
			if (player->GetPressMove())
			{
				MoveDirectionVector = UMathHelperLibrary::GetControllerInputDir(Owner->GetControlRotation(),
					player->
					MovementInputVector);
			}
			else
			{
				auto rot = Owner->GetControlRotation();

				MoveDirectionVector = UKismetMathLibrary::GetForwardVector(FRotator(0, rot.Yaw, rot.Roll)) * -1;
			}
		}
		else
		{
			if (player->MovementInputVector.IsZero())
			{
				MoveDirectionVector = player->GetActorForwardVector();
			}
			else
			{
				MoveDirectionVector = UMathHelperLibrary::GetControllerInputDir(Owner->GetControlRotation(),
					player->MovementInputVector);
			}
		}
	}
}


void UAnimNotifyState_AddForce::ApplyForceByInputDirection(float DeltaTime) const
{
	auto power = ApplyTalent() * DeltaTime;
	Owner->GetCharacterMovement()->AddImpulse(MoveDirectionVector * power, bVelocityChange);
}

void UAnimNotifyState_AddForce::ApplyForceByDirectionValue(float DeltaTime) const
{
	auto power = ApplyTalent() * DeltaTime;
	Owner->GetCharacterMovement()->AddImpulse(MoveDirectionVector * power, bVelocityChange);
	UKismetSystemLibrary::DrawDebugArrow(Owner->GetWorld(), Owner->GetActorLocation(),
	                                     MoveDirectionVector * 800.f + Owner->GetActorLocation(), 300.f,
	                                     FLinearColor::Yellow);
}

float UAnimNotifyState_AddForce::ApplyTalent() const
{
	if (Owner.IsValid() && Owner->IsA<APlayerCharacter>())
	{
		if (auto atComp = Cast<APlayerCharacter>(Owner)->GetAbilityTalentComponent())
		{
			return Power + atComp->CalculateModifiedDodgeDistanceWithTraits(Power);
		}
	}

	return Power;
}

bool UAnimNotifyState_AddForce::IsPlayerCharacter() const
{
	if (Owner.IsValid() && Owner->IsA<APlayerCharacter>())
	{
		return true;
	}
	return false;
}

FVector UAnimNotifyState_AddForce::GetMoveDirFromForceDirectionValue()
{
	if (bUseControllerDir)
	{
		switch (ForceDirection)
		{
		case EDirection::Front:
			return UKismetMathLibrary::GetForwardVector(Owner->GetControlRotation());
		case EDirection::Right:
			return UKismetMathLibrary::GetRightVector(Owner->GetControlRotation());
		case EDirection::Left:
			return UKismetMathLibrary::GetRightVector(Owner->GetControlRotation()) * -1;
		case EDirection::Back:
			return UKismetMathLibrary::GetForwardVector(Owner->GetControlRotation()) * -1;
		case EDirection::FrontRight:
		case EDirection::FrontLeft:
		case EDirection::BackRight:
		case EDirection::BackLeft:
		case EDirection::Up:
		case EDirection::Down:
			UE_LOGFMT(LogTemp, Error, "{0} {1}, 구현이 필요한 부분입니다!!", __FUNCTION__, __LINE__);
			break;
		case EDirection::MAX:
			break;
		default: ;
		}
	}
	else
	{
		switch (ForceDirection)
		{
		case EDirection::Front:
			return Owner->GetActorForwardVector();
		case EDirection::Right:
			return Owner->GetActorRightVector();
		case EDirection::Left:
			return Owner->GetActorRightVector() * -1;
		case EDirection::Back:
			return Owner->GetActorForwardVector() * -1;
		case EDirection::FrontRight:
		case EDirection::FrontLeft:
		case EDirection::BackRight:
		case EDirection::BackLeft:
			UE_LOGFMT(LogTemp, Warning, "구현이 필요한 부분입니다 {0} {1}", __FUNCTION__, __LINE__);
			break;
		case EDirection::Down:
			return Owner->GetActorUpVector() * -1;
		case EDirection::Up:
			return Owner->GetActorUpVector();
		case EDirection::MAX:
			break;
		default: ;
		}
	}
	return FVector::Zero();
}

void UAnimNotifyState_AddForce::SaveMoveDirection()
{
	switch (ForceApplicationMode)
	{
	case EForceApplicationMode::CustomDirectionSetting:
		MoveDirectionVector = GetMoveDirFromForceDirectionValue();
		break;
	}
}
