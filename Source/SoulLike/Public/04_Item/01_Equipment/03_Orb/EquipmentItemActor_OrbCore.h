// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "04_Item/01_Equipment/EquipmentItemActor.h"
#include "EquipmentItemActor_OrbCore.generated.h"


/**
 * 
 */
UCLASS()
class SOULLIKE_API AEquipmentItemActor_OrbCore : public AEquipmentItemActor
{
	GENERATED_BODY()

protected:
	AEquipmentItemActor_OrbCore();
	void SetCoreMatrixPtr();
	virtual void PostInitializeComponents() override;
	virtual const FItemInformation* GetItemInformation() override;
	virtual bool UseItem_Implementation(AActor* Target, const FGuid& ThisItemsUniqueID) override;
	virtual void UnEquip_Implementation(AActor* Target, const FGuid& ThisItemsUniqueID) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Destroyed() override;
};
