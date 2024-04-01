// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "02_Ability/AbilityEffect.h"
#include "StatusEffect_AccBase.generated.h"

/**
 * 
 */
UCLASS(DefaultToInstanced)
class SOULLIKE_API UStatusEffect_AccBase : public UAbilityEffect
{
	GENERATED_BODY()

public:
	UStatusEffect_AccBase();

protected:
	virtual bool
	ApplyInstantEffect_Implementation(ABaseCharacter* Target, UObject* AdditionalInfo, float DeltaTime) override;
	virtual bool CanApplyEffect_Implementation(ABaseCharacter* Target, bool bShowLog) const override;

	//이 태그에 해당하는 상태이상이 이미 부여되어있는가 확인하기 위해 사용됩니다.
	UPROPERTY(EditAnywhere)
	FGameplayTag CheckStatusEffectTag;
};
