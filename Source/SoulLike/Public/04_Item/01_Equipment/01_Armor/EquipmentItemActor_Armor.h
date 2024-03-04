// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "04_Item/01_Equipment/EquipmentItemActor.h"
#include "EquipmentItemActor_Armor.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API AEquipmentItemActor_Armor : public AEquipmentItemActor
{
	GENERATED_BODY()

	virtual const FItemInformation* GetItemInformation() override;
};
