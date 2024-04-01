// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "04_Item/ItemActor.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EnhancementSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UEnhancementSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	class UDataTable* EnhancementMaterialTable;
	UPROPERTY()
	class UDataTable* AttributeValuePerEnhancementTable;

	UPROPERTY()
	class UDataTable* PotionMaterialTable;

public:
	UEnhancementSubsystem();

	TArray<FEnhancementMaterial*> GetEquipmentEnhancementMaterial() const;
	TArray<FAttributeValuePerEnhancement*> GetAttributeValuePerEnhancement() const;

	TArray<FEnhancementMaterial*> GetPotionEnhancementMaterial() const;

	float GetAttributeValuePerEnhancement(EAttributeType Key);
};
