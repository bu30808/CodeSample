// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_AddRelativeRotation.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UAnimNotify_AddRelativeRotation : public UAnimNotify
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	FRotator AddRelativeRotation;

	UAnimNotify_AddRelativeRotation();
	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload) override;
};
