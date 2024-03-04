// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_DodgeImmediately.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UAnimNotifyState_DodgeImmediately : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	bool bPressed = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UInputAction* InputAction = nullptr;
	UPROPERTY()
	TArray<FKey> Actionkeys;

	//기상회피 특성이 부여되어있는지 확인합니다.
	UPROPERTY()
	bool HasDodgeTalent;

	UAnimNotifyState_DodgeImmediately();

	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void
	BranchingPointNotifyTick(FBranchingPointNotifyPayload& BranchingPointPayload, float FrameDeltaTime) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;
};
