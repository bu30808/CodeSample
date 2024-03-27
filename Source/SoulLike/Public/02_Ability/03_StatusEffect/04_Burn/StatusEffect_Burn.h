// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "02_Ability/03_StatusEffect/StatusEffect_AccBase.h"
#include "02_Ability/03_StatusEffect/StatusEffect_TickBase.h"
#include "StatusEffect_Burn.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UStatusEffect_Burn : public UStatusEffect_TickBase
{
	GENERATED_BODY()
protected:
	UStatusEffect_Burn();
	
};

UCLASS()
class SOULLIKE_API UStatusEffect_BurnAcc : public UStatusEffect_AccBase
{
	GENERATED_BODY()
protected:
	UStatusEffect_BurnAcc();
	
};

UCLASS()
class SOULLIKE_API UStatusEffect_DodgeBurnDecrease: public UAbilityEffect
{
	GENERATED_BODY()
protected:
	UStatusEffect_DodgeBurnDecrease();
	virtual void ProcessEffect_Implementation(ABaseCharacter* Target, AActor* EffectBy, UAbilityBase* From, UObject* AdditionalData = nullptr) override;
};
