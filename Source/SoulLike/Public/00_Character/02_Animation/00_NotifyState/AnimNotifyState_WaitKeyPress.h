// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/00_Player/00_Controller/00_Component/InputHandlerComponent.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_WaitKeyPress.generated.h"

/**
 * 특정 구간에 키 입력을 기다려야 하는 경우 사용합니다.
 * 구간이 끝날 때, 입력이 있었다면, 트리거됩니다.
 */
UCLASS()
class SOULLIKE_API UAnimNotifyState_WaitKeyPress : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UAnimNotifyState_WaitKeyPress();

protected:
	//어떤 액션을 기다리나요?
	UPROPERTY(EditAnywhere)
	TArray<FKeyPressedInfo> WaitAction;
	UPROPERTY(EditAnywhere)
	bool bTriggerImmediately = false;

	UPROPERTY()
	TWeakObjectPtr<class APlayerCharacter> Player;


	UFUNCTION(BlueprintCosmetic)
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	UFUNCTION(BlueprintCosmetic)
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;
};
