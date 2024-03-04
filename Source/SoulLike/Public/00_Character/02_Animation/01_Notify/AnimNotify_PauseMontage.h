// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_PauseMontage.generated.h"

/**
 * PauseTime만큼 잠깐 몽타주를 일시정지 합니다.
 */
UCLASS()
class SOULLIKE_API UAnimNotify_PauseMontage : public UAnimNotify
{
	GENERATED_BODY()

protected:
	//몽타주를 일시정지할 시간
	UPROPERTY(EditAnywhere)
	float PauseTime = 0;

	UAnimNotify_PauseMontage();
	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload) override;
};
