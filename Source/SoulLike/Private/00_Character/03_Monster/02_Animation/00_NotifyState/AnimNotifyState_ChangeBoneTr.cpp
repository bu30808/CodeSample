// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/02_Animation/00_NotifyState/AnimNotifyState_ChangeBoneTr.h"

#include "AIController.h"
#include "00_Character/BaseCharacter.h"
#include "96_Library/MathHelperLibrary.h"
#include "97_Interface/BoneTransformInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Logging/StructuredLog.h"

UAnimNotifyState_ChangeBoneTr::UAnimNotifyState_ChangeBoneTr()
{
	bIsNativeBranchingPoint = true;
}

void UAnimNotifyState_ChangeBoneTr::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (auto mesh = BranchingPointPayload.SkelMeshComponent)
	{
		if (auto monster = mesh->GetOwner<ABaseCharacter>())
		{
			if (auto bbComp = monster->GetController<AAIController>()->GetBlackboardComponent())
			{
				auto target = Cast<AActor>(bbComp->GetValueAsObject("Target"));

				if (auto interface = Cast<IBoneTransformInterface>(monster))
				{
					TargetYaw = UMathHelperLibrary::CalculateDegree(monster, target->GetActorLocation());
				}
			}
		}
	}
}

void UAnimNotifyState_ChangeBoneTr::BranchingPointNotifyTick(FBranchingPointNotifyPayload& BranchingPointPayload,
                                                             float FrameDeltaTime)
{
	if (auto mesh = BranchingPointPayload.SkelMeshComponent)
	{
		if (auto monster = mesh->GetOwner<ABaseCharacter>())
		{
			if (auto bbComp = monster->GetController<AAIController>()->GetBlackboardComponent())
			{
				auto target = Cast<AActor>(bbComp->GetValueAsObject("Target"));

				if (auto interface = Cast<IBoneTransformInterface>(monster))
				{
					float yaw = UMathHelperLibrary::CalculateDegree(monster, target->GetActorLocation());

					if (FMath::IsNearlyEqual(yaw, TargetYaw))
					{
						return;
					}
					IBoneTransformInterface::Execute_OnUpdateBoneTransform(monster, FTransform(FRotator(0, yaw, 0)));
				}
			}
		}
	}
}

void UAnimNotifyState_ChangeBoneTr::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (auto mesh = BranchingPointPayload.SkelMeshComponent)
	{
		if (auto monster = mesh->GetOwner<ABaseCharacter>())
		{
			if (auto interface = Cast<IBoneTransformInterface>(monster))
			{
				IBoneTransformInterface::Execute_OnUpdateBoneTransform(monster, FTransform::Identity);
			}
		}
	}
}
