// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "02_Ability/03_StatusEffect/StatusEffect_AccBase.h"
#include "StatusEffect_Bleed.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UStatusEffect_Bleed : public UAbilityEffect
{
	GENERATED_BODY()
protected:
	UStatusEffect_Bleed();
	virtual void ProcessEffect_Implementation(ABaseCharacter* Target, AActor* EffectBy, UAbilityBase* From, UObject* AdditionalData = nullptr) override;
	virtual void EndEffect_Implementation(ABaseCharacter* Target) override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAbilityEffect> BleedInstantObject;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAbilityEffect> BleedDurationObject;
};
UCLASS()
class SOULLIKE_API UStatusEffect_BleedAcc : public UStatusEffect_AccBase
{
	GENERATED_BODY()
protected:
	UStatusEffect_BleedAcc();
};

//출혈 적용시 주는 즉발 피해
UCLASS()
class SOULLIKE_API UStatusEffect_BleedInstant : public UAbilityEffect
{
	GENERATED_BODY()
protected:
	UStatusEffect_BleedInstant();
	virtual void OverrideAttributeEffects_Implementation(ABaseCharacter* Target, AActor* EffectBy) override;
	UPROPERTY(EditAnywhere)
	float Amount =0.2f;
};

//출혈 적용시 n초간 주는 지속 피해
UCLASS()
class SOULLIKE_API UStatusEffect_BleedDuration : public UAbilityEffect
{
	GENERATED_BODY()
protected:
	UStatusEffect_BleedDuration();
	virtual void OverrideAttributeEffects_Implementation(ABaseCharacter* Target, AActor* EffectBy) override;
	UPROPERTY(EditAnywhere)
	float Amount =0.01f;
};