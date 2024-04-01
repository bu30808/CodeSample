// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "02_Ability/AbilityEffect.h"
#include "StatusEffect_TickBase.generated.h"

/**
 * 틱마다 무엇인가 하는 상태이상
 * 주로 최대체력 비례 피해
 */
UCLASS(DefaultToInstanced)
class SOULLIKE_API UStatusEffect_TickBase : public UAbilityEffect
{
	GENERATED_BODY()

protected:
	UStatusEffect_TickBase();

	UPROPERTY(EditAnywhere)
	float Amount = 0.001f;

	//계산 후 최종 증가할 수치
	UPROPERTY()
	float ResultAmount;

	virtual void OverrideAttributeEffects_Implementation(ABaseCharacter* Target, AActor* EffectBy) override;
};
