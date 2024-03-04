// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/02_Animation/00_NotifyState/AnimNotifyState_MontageSpeed.h"

UAnimNotifyState_MontageSpeed::UAnimNotifyState_MontageSpeed()
{
}

void UAnimNotifyState_MontageSpeed::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (auto instance = MeshComp->GetAnimInstance())
	{
		auto curMontage = instance->GetCurrentActiveMontage();
		OriginalPlayRate = instance->Montage_GetPlayRate(curMontage);
		instance->Montage_SetPlayRate(curMontage, PlayRateToChange);
	}
}

void UAnimNotifyState_MontageSpeed::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                              const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (auto instance = MeshComp->GetAnimInstance())
	{
		auto curMontage = instance->GetCurrentActiveMontage();
		instance->Montage_SetPlayRate(curMontage, OriginalPlayRate);
	}
}
