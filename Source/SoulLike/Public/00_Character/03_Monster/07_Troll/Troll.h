// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "97_Interface/BossMonsterInterface.h"
#include "Troll.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API ATroll : public ABaseMonster, public IBossMonsterInterface
{
	GENERATED_BODY()
};
