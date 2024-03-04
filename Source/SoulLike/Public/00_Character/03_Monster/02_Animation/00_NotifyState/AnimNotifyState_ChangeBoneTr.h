// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_ChangeBoneTr.generated.h"

/**
 * 애니메이션 특정 구간에 본 트렌스폼을 변경하려면 사용하세요.
 */
UCLASS()
class SOULLIKE_API UAnimNotifyState_ChangeBoneTr : public UAnimNotifyState
{
	GENERATED_BODY()
	UAnimNotifyState_ChangeBoneTr();

	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void
	BranchingPointNotifyTick(FBranchingPointNotifyPayload& BranchingPointPayload, float FrameDeltaTime) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	float TargetYaw;
};
