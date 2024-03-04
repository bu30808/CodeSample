// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ApplyEffect.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UAnimNotify_ApplyEffect : public UAnimNotify
{
	GENERATED_BODY()

protected:
	UAnimNotify_ApplyEffect();

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UAbilityEffect>> EffectObjects;

	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload) override;

	virtual FString GetEditorComment() override { return TEXT("어빌리티 이팩트를 적용합니다."); }
};
