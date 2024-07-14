// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "97_Interface/BossMonsterInterface.h"
#include "Troll.generated.h"
/*
*
*
*/
UCLASS()
class SOULLIKE_API ATroll : public ABaseMonster
{
	GENERATED_BODY()

protected:
	ATroll();
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UStaticMeshComponent* WeaponComponent;

public:
	virtual UPrimitiveComponent* GetPrimitiveComponentForLineTrace_Implementation() override;
	
};


UCLASS()
class SOULLIKE_API ATrollBoss : public ATroll, public IBossMonsterInterface
{
	GENERATED_BODY()

protected:
	

	UFUNCTION(BlueprintCallable)
	void ChangePhaseMaterial(class UMaterialInstance* Body, class UMaterialInstance* Weapon);

	UFUNCTION(BlueprintCallable)
	void ApplyPoisonAccToTarget(class ABaseCharacter* Target, TSubclassOf<UAbilityEffect> PoisonAccEffect);
};

