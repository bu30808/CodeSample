// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/02_Animation/MonsterAnimInstance.h"

#include "BrainComponent.h"
#include "KismetAnimationLibrary.h"
#include "00_Character/BaseCharacter.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "96_Library/MathHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Logging/StructuredLog.h"

void UMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Character.IsValid())
	{
		if (auto monster = Cast<ABaseMonster>(Character))
		{
			bIsDead = monster->IsDead();
			if (bIsDead == false)
			{
				if (const auto aiCon = monster->GetController<AAIController>())
				{
					bIsFocus = (aiCon->GetFocusActor() != nullptr);
					if (aiCon->GetBlackboardComponent() != nullptr)
					{
						bIsFindTarget = aiCon->GetBlackboardComponent()->GetValueAsObject("Target") ? true : false;
					}
				}

				MonsterState = monster->GetMonsterState();
			}
		}
	}
}

void UMonsterAnimInstance::AnimNotify_OnHitEnter_Implementation()
{
	Super::AnimNotify_OnHitEnter_Implementation();
}

void UMonsterAnimInstance::AnimNotify_OnHitExit_Implementation()
{
	Super::AnimNotify_OnHitExit_Implementation();

	if (auto monster = Cast<ABaseMonster>(Character))
	{
		if (bIsDead == false)
		{
			if (const auto aiCon = monster->GetController<AMonsterAIController>())
			{
				if (!aiCon->IsBehaviorTreeRunning())
				{
					UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s : 애님 인스턴스 비헤이비어 트리 실행"),*GetNameSafe(Character.Get())));
					aiCon->StartBehavior();
				}
			}
		}
	}
}

void UMonsterAnimInstance::ChangeBoneTransform_Implementation(float DeltaTime)
{
	Super::ChangeBoneTransform_Implementation(DeltaTime);

	if (Character.IsValid() && bIsDead == false)
	{
		if (auto bbComp = UAIBlueprintHelperLibrary::GetBlackboard(Character.Get()))
		{
			if (auto target = Cast<AActor>(bbComp->GetValueAsObject("Target")))
			{
				/*auto targetRot = UKismetMathLibrary::FindLookAtRotation(Character->GetActorLocation(),target->GetActorLocation());
				targetRot.Yaw = UKismetAnimationLibrary::CalculateDirection(UKismetMathLibrary::GetForwardVector(targetRot),Character->GetActorRotation());
				NeckRotation = FMath::RInterpTo(NeckRotation,targetRot,DeltaTime,BoneTransformLerpSpeed);*/

				auto targetRot = UKismetMathLibrary::FindLookAtRotation(Character->GetActorLocation(),
				                                                        Cast<AActor>(target)->GetActorLocation());
				targetRot.Yaw = FMath::Clamp(
					UKismetAnimationLibrary::CalculateDirection(UKismetMathLibrary::GetForwardVector(targetRot),
					                                            Character->GetActorRotation()), -80, 80);
				NeckRotation = FMath::RInterpTo(NeckRotation, targetRot, DeltaTime, BoneTransformLerpSpeed);
			}
			else
			{
				NeckRotation = FRotator::ZeroRotator;
			}
		}
	}
}

void UMonsterAnimInstance::ClearBoneTransform_Implementation(float DeltaTime)
{
	Super::ClearBoneTransform_Implementation(DeltaTime);
	NeckRotation = FMath::RInterpTo(NeckRotation, FRotator::ZeroRotator, DeltaTime, BoneTransformLerpSpeed);
}
