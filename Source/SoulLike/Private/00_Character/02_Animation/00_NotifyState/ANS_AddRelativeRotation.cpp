// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/02_Animation/00_NotifyState/ANS_AddRelativeRotation.h"

UANS_AddRelativeRotation::UANS_AddRelativeRotation()
{
}

void UANS_AddRelativeRotation::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	TotalNotifyTime = TotalDuration;

	if (MeshComp)
	{
		OriginalRelativeRot = MeshComp->GetRelativeRotation();
	}
}

void UANS_AddRelativeRotation::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                          float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (MeshComp)
	{
		FRotator RotationPerSecond = FRotator(RelativeRotationToWantTotalAdd.Pitch / TotalNotifyTime,
		                                      RelativeRotationToWantTotalAdd.Yaw / TotalNotifyTime,
		                                      RelativeRotationToWantTotalAdd.Roll / TotalNotifyTime);
		// 주어진 시간 동안 회전량 계산
		FRotator RotationThisFrame = RotationPerSecond * FrameDeltaTime;

		MeshComp->AddRelativeRotation(RotationThisFrame);
	}
}

void UANS_AddRelativeRotation::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (MeshComp)
	{
		MeshComp->SetRelativeRotation(OriginalRelativeRot);
	}
}
