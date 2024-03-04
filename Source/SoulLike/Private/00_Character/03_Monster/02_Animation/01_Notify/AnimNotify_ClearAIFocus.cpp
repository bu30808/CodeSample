// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/02_Animation/01_Notify/AnimNotify_ClearAIFocus.h"

#include "AIController.h"
#include "00_Character/03_Monster/BaseMonster.h"

UAnimNotify_ClearAIFocus::UAnimNotify_ClearAIFocus()
{
	bIsNativeBranchingPoint = true;
}

void UAnimNotify_ClearAIFocus::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotify(BranchingPointPayload);
	if (auto monster = BranchingPointPayload.SkelMeshComponent->GetOwner<ABaseMonster>())
	{
		if (auto aiCon = Cast<AAIController>(monster->GetController()))
		{
			monster->bUseControllerRotationYaw = false;
			aiCon->K2_ClearFocus();
		}
	}
}
