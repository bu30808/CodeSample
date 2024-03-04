// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "02_Ability/03_StatusEffect/StatusEffect_AccBase.h"
#include "02_Ability/03_StatusEffect/StatusEffect_TickBase.h"
#include "StatusEffect_Poison.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UStatusEffect_Poison : public UStatusEffect_TickBase
{
	GENERATED_BODY()
public:
	UStatusEffect_Poison();
protected:
};

UCLASS()
class SOULLIKE_API UStatusEffect_PoisonAcc : public UStatusEffect_AccBase
{
	GENERATED_BODY()

public:
	UStatusEffect_PoisonAcc();

protected:
	/*virtual bool ApplyInstantEffect_Implementation(ABaseCharacter* Target, UObject* AdditionalInfo, float DeltaTime) override;
	virtual bool CanApplyEffect_Implementation(ABaseCharacter* Target, bool bShowLog) const override;*/
};
