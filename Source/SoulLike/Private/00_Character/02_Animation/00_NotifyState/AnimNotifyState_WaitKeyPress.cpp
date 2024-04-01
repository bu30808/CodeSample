// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/02_Animation/00_NotifyState/AnimNotifyState_WaitKeyPress.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/00_Player/00_Controller/UserController.h"
#include "00_Character/00_Player/00_Controller/00_Component/InputHandlerComponent.h"
#include "Logging/StructuredLog.h"

UAnimNotifyState_WaitKeyPress::UAnimNotifyState_WaitKeyPress()
{
	bIsNativeBranchingPoint = true;
#if WITH_EDITOR
	bShouldFireInEditor = false;
#endif
}

void UAnimNotifyState_WaitKeyPress::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyBegin(BranchingPointPayload);

	if (BranchingPointPayload.SkelMeshComponent != nullptr)
	{
		if (auto character = BranchingPointPayload.SkelMeshComponent->GetOwner<ABaseCharacter>())
		{
			if (auto pc = character->GetController<AUserController>())
			{
				pc->GetInputHandlerComponent()->StartWaitAction(WaitAction, bTriggerImmediately);
			}
		}
	}
}

void UAnimNotifyState_WaitKeyPress::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyEnd(BranchingPointPayload);
	if (BranchingPointPayload.SkelMeshComponent != nullptr)
	{
		if (auto character = BranchingPointPayload.SkelMeshComponent->GetOwner<ABaseCharacter>())
		{
			if (auto pc = character->GetController<AUserController>())
			{
				//UE_LOGFMT(LogTemp,Error,"{0} {1}",__FUNCTION__,__LINE__);

				pc->GetInputHandlerComponent()->EndWaitAction(WaitAction);
			}
		}
	}
}
