// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_AddLocalRotation.generated.h"

/**
 * 로컬 회전이 필요하다면 사용합니다.
 */
UCLASS()
class SOULLIKE_API UAnimNotifyState_AddLocalRotation : public UAnimNotifyState
{
	GENERATED_BODY()

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                        float FrameDeltaTime) override;
};
