// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "02_Ability/AbilityEffect.h"
#include "02_Ability/03_StatusEffect/StatusEffect_AccBase.h"
#include "StatusEffect_Petrifaction.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UStatusEffect_Petrifaction : public UAbilityEffect
{
	GENERATED_BODY()
protected:
	UStatusEffect_Petrifaction();
	virtual void ProcessEffect_Implementation(ABaseCharacter* Target, AActor* EffectBy, UAbilityBase* From, UObject* AdditionalData = nullptr) override;
	virtual bool CanApplyEffect_Implementation(ABaseCharacter* Target, bool bShowLog) const override;
	
	UFUNCTION()
	void KillTarget(ABaseCharacter* Target,AActor* EffectBy);
};

UCLASS()
class SOULLIKE_API UStatusEffect_PetrifactionAcc: public UStatusEffect_AccBase
{
	GENERATED_BODY()
protected:
	UStatusEffect_PetrifactionAcc();
	virtual bool ApplyInstantEffect_Implementation(ABaseCharacter* Target, UObject* AdditionalInfo, float DeltaTime) override;
};
