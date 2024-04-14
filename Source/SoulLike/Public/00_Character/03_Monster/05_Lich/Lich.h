// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "Lich.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API ALich : public ABaseMonster
{
	GENERATED_BODY()

public:
	ALich();
	void PlayIdleCue();

	virtual void BeginPlay() override;
	UFUNCTION()
	void ApplyIdleCueEvent();

	UPROPERTY(EditAnywhere)
	FAbilityCueInformation IdleCueInformation;
	UPROPERTY()
	FTimerHandle IdleCueTimerHandle;

	virtual void Activate() override;
	virtual void Deactivate() override;
	
	virtual void RestoreComponentAttachment()const override;
	virtual void OnDeadMontageEndedEvent(UAnimMontage* Montage, bool bInterrupted) override;
};
