// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "02_Ability/AbilityEffect.h"
#include "02_Ability/03_StatusEffect/StatusEffect_AccBase.h"
#include "StatusEffect_Chill.generated.h"

class ABaseCharacter;
/**
 * 
 */
UCLASS()
class SOULLIKE_API UStatusEffect_Chill : public UAbilityEffect
{
	GENERATED_BODY()
protected:
	UStatusEffect_Chill();

	//얼마만큼 느리게 만들 것인가
	UPROPERTY()
	float SlowRate = 0.25f;
	UPROPERTY()
	float OriginalPlayRate = 0;

	virtual void OverrideAttributeEffects_Implementation(ABaseCharacter* Target, AActor* EffectBy) override;
	virtual void RestoreAttribute_Implementation(ABaseCharacter* Target) override;
	virtual bool ApplyInstantEffect_Implementation(ABaseCharacter* Target, UObject* AdditionalInfo, float DeltaTime) override;
};

UCLASS()
class SOULLIKE_API UStatusEffect_ChillAcc : public UStatusEffect_AccBase
{
	GENERATED_BODY()
protected:
	UStatusEffect_ChillAcc();
};



UCLASS()
class SOULLIKE_API UStatusEffect_Freeze : public UAbilityEffect
{
	GENERATED_BODY()
protected:
	UStatusEffect_Freeze();
	
	virtual void ProcessEffect_Implementation(ABaseCharacter* Target, AActor* EffectBy, UAbilityBase* From, UObject* AdditionalData = nullptr) override;
	virtual void EndEffect_Implementation(ABaseCharacter* Target) override;
	virtual void OverrideTime_Implementation(ABaseCharacter* Target) override;

	UPROPERTY()
	TObjectPtr<AActor> AccruedBy;
};