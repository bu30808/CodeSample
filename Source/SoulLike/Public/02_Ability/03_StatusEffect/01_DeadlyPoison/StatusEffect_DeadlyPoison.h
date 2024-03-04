// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "02_Ability/03_StatusEffect/StatusEffect_AccBase.h"
#include "02_Ability/03_StatusEffect/StatusEffect_TickBase.h"
#include "StatusEffect_DeadlyPoison.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UStatusEffect_DeadlyPoison : public UStatusEffect_TickBase
{
	GENERATED_BODY()
protected:
	UStatusEffect_DeadlyPoison();
	
};

UCLASS()
class SOULLIKE_API UStatusEffect_DeadlyPoisonAcc : public UStatusEffect_AccBase
{
	GENERATED_BODY()
protected:
	UStatusEffect_DeadlyPoisonAcc();
};
