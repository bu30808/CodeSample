// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "03_Widget/02_Monster/HealthBarWidget.h"
#include "BossHealthBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UBossHealthBarWidget : public UHealthBarWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Name;

public:
	void SetInfo(class ABaseMonster* Boss);

	UFUNCTION()
	void OnChangeHPValueEvent(float Value, float MaxValue);
};
