// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/02_Animation/01_Notify/AnimNotify_EnableRootMotion.h"


void UAnimNotify_EnableRootMotion::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                          const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		if (auto instance = MeshComp->GetAnimInstance())
		{
			// 루트 모션 활성화
			instance->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
		}
	}
}
