// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/02_Animation/00_NotifyState/AnimNotifyState_AddLocalRotation.h"

void UAnimNotifyState_AddLocalRotation::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                   float FrameDeltaTime)
{
	if (!MeshComp)
	{
		return;
	}

	// 로컬 로테이션을 가져옵니다.
	FRotator LocalRotation = MeshComp->GetRelativeRotation();

	// 로컬 로테이션의 Yaw 값을 360만큼 더합니다.
	LocalRotation.Yaw += 360.0f * FrameDeltaTime; // FrameDeltaTime을 사용하여 각도를 더해줍니다.

	// 수정된 로컬 로테이션을 설정합니다.
	MeshComp->SetRelativeRotation(LocalRotation);
}
