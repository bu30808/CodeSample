// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "04_Item/ItemActor.h"
#include "EnhancementItemActor.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API AEnhancementItemActor : public AItemActor
{
	GENERATED_BODY()

	virtual const FItemInformation* GetItemInformation() override;
	virtual bool UseItem_Implementation(AActor* Target, const FGuid& ThisItemsUniqueID) override;
};
