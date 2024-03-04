// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_DisableInput.generated.h"


UENUM(BlueprintType)
enum class EDisableInputType : uint8
{
	DisableLookInput,
	DisableMoveInput,
	Both
};

/**
 * 
 */
UCLASS()
class SOULLIKE_API UAnimNotifyState_DisableInput : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	EDisableInputType DisableInputType = EDisableInputType::Both;


	UAnimNotifyState_DisableInput();

	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;
};
