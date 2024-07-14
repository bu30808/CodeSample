// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/02_Animation/BaseAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "00_Character/BaseCharacter.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/01_Component/AnimationHelperComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Logging/StructuredLog.h"


void UBaseAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (const auto character = Cast<ABaseCharacter>(TryGetPawnOwner()))
	{
		Character = character;
	}
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Character.IsValid())
	{
		//떨어지는 중인가 저장합니다.
		IsFalling = Character->GetMovementComponent()->IsFalling();
		//캐릭터의 움직이는 속도값을 가져옵니다.
		Speed = Character->GetVelocity().Length();

		Velocity = Character->GetVelocity();
		bShouldMove = Speed > 3.f && Character->GetCharacterMovement()->GetCurrentAcceleration() != FVector::ZeroVector;


		//Roll Pitch Yaw값을 설정합니다.
		{
			//플레이어의 시야 방향을 가져옵니다.
			const auto& animRotation = Character->GetBaseAimRotation();
			//캐릭터의 회전을 가져옵니다.
			const auto& characterRotation = Character->GetActorRotation();
			//정규화된 델타값을 가져옵니다.
			const auto& normalizedRot = UKismetMathLibrary::NormalizedDeltaRotator(animRotation, characterRotation);

			Roll = normalizedRot.Roll;
			Pitch = normalizedRot.Pitch;
			Yaw = normalizedRot.Yaw;
		}


		//Setting Yaw Delta for Leans
		//기울기에 사용할 Yaw Delta값을 구합니다.
		{
			const auto& characterRotation = Character->GetActorRotation();
			Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, characterRotation);
			//float target = UKismetMathLibrary::NormalizedDeltaRotator(RotationLastTick, characterRotation).Yaw /	DeltaSeconds / 7.f;
			//YawDelta = FMath::FInterpTo(YawDelta, target, DeltaSeconds, 6.f);
			//RotationLastTick = characterRotation;
		}
		/*
		
				//가속 여부를 설정합니다.
				{
					bIsAcclerating = Character->GetCharacterMovement()->GetCurrentAcceleration().Length() > 0;
				}
		
				//풀바디 여부를 설정합니다.
				{
					const auto value = GetCurveValue("FullBody");
					bFullBody = value > 0;
				}*/

		{
			CharacterState = Character->GetCharacterState();
			HitDegree = Character->GetAnimationHelperComponent()->HitDegree;
			bModifyBoneTransform = Character->GetAnimationHelperComponent()->bModifySkeletonTransform;

			if (bModifyBoneTransform)
			{
				ChangeBoneTransform(DeltaSeconds);
			}
			else
			{
				ClearBoneTransform(DeltaSeconds);
			}
		}
	}
}

void UBaseAnimInstance::AnimNotify_OnHitEnter_Implementation()
{
	UE_LOGFMT(LogTemp, Log, "AnimNotify_OnHitEnter 111111111111");
	if (Character.IsValid())
	{
		if (Character->IsA<APlayerCharacter>())
		{
		}
	}
}



void UBaseAnimInstance::AnimNotify_OnHitExit_Implementation()
{
	if (Character.IsValid())
	{
		if (Character->GetAnimationHelperComponent()->GetIsTriggeredHitAnimationExitEvent() == false)
		{
			UE_LOGFMT(LogAbility, Error, "{0} {1} : 히트 애니메이션 빠져나감 이벤트가 호출되지 않았기 때문에 강제로 호출합니다.", __FUNCTION__,
			          __LINE__);
			Character->GetAnimationHelperComponent()->OnTriggerHitAnimationExit.Broadcast(Character.Get(), nullptr);
		}
	}
}

