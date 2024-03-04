// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ClearAIFocus.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UAnimNotify_ClearAIFocus : public UAnimNotify
{
	GENERATED_BODY()

protected:
	UAnimNotify_ClearAIFocus();
	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload) override;
};
