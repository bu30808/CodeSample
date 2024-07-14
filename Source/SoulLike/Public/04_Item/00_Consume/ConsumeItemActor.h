// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "04_Item/ItemActor.h"
#include "ConsumeItemActor.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API AConsumeItemActor : public AItemActor
{
	GENERATED_BODY()

	virtual const FItemInformation* GetItemInformation() override;

public:
	//소비 아이템의 효과 값을 가져옵니다.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetEffectValue();


	virtual bool UseItem_Implementation(AActor* Target, const FGuid& ThisItemsUniqueID) override;
	virtual void OnEndAbilityEvent_Implementation() override;
};

UCLASS()
class SOULLIKE_API ASoulItemActor : public AConsumeItemActor
{
	GENERATED_BODY()

public:
	virtual void OnOverrideSelfEffectEvent_Implementation(const TArray<UAbilityEffect*>& SelfEffect, AActor* Target) override;
	virtual FText GetFormattedDescription_Implementation() override;
};