// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "Wolf.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API AWolf : public ABaseMonster
{
	GENERATED_BODY()

	friend class AWolfBoss;

protected:

	/*virtual void
	TriggerHitAnimation_Implementation(UAbilityEffectAdditionalInformation* AdditionalInformation) override;
	UFUNCTION()
	void OnHitMontageBlendOutEvent(UAnimMontage* Montage, bool bInterrupted);*/
	
};
