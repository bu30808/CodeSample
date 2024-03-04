// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/02_Animation/01_Notify/AnimNotify_PauseMontage.h"

#include "00_Character/BaseCharacter.h"
#include "Animation/AnimInstance.h"

UAnimNotify_PauseMontage::UAnimNotify_PauseMontage()
{
	bIsNativeBranchingPoint = true;
}

void UAnimNotify_PauseMontage::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotify(BranchingPointPayload);
	if (auto character = BranchingPointPayload.SkelMeshComponent->GetOwner<ABaseCharacter>())
	{
		if (auto instance = character->GetMesh()->GetAnimInstance())
		{
			instance->Montage_Pause();
			FTimerHandle ResumeTimerHandle;
			FTimerDelegate ResumeTimerDel = FTimerDelegate::CreateLambda([instance]()
			{
				instance->Montage_Resume(nullptr);
			});

			character->GetWorldTimerManager().SetTimer(ResumeTimerHandle, ResumeTimerDel, PauseTime, false);
		}
	}
}
