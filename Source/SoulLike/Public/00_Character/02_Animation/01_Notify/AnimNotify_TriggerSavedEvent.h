// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_TriggerSavedEvent.generated.h"

/**
 * 애니메이션의 특정 구간에 저장한 어빌리티 이벤트를 발동합니다.
 * 태그정보로 구별합니다.
 */
UCLASS()
class SOULLIKE_API UAnimNotify_TriggerSavedEvent : public UAnimNotify
{
	GENERATED_BODY()

protected:
	UAnimNotify_TriggerSavedEvent();
	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload) override;


	//트리거할 어빌리티 태그
	UPROPERTY(EditAnywhere)
	FGameplayTag AbilityTag;
};
