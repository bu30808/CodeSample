// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/02_Animation/01_Notify/AnimNotify_AddRelativeRotation.h"

#include "00_Character/BaseCharacter.h"
#include "Components/CapsuleComponent.h"

UAnimNotify_AddRelativeRotation::UAnimNotify_AddRelativeRotation()
{
	bIsNativeBranchingPoint = true;
}

void UAnimNotify_AddRelativeRotation::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotify(BranchingPointPayload);
	if (auto mesh = BranchingPointPayload.SkelMeshComponent)
	{
		if (mesh->GetOwner<ABaseCharacter>())
		{
			mesh->GetOwner<ABaseCharacter>()->GetCapsuleComponent()->AddRelativeRotation(AddRelativeRotation);
		}
	}
}
