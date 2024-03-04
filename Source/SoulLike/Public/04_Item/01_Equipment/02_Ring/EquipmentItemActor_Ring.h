// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "04_Item/01_Equipment/EquipmentItemActor.h"
#include "EquipmentItemActor_Ring.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API AEquipmentItemActor_Ring : public AEquipmentItemActor
{
	GENERATED_BODY()
	virtual const FItemInformation* GetItemInformation() override;
	//덮어쓰지 않으면, 부모함수가 호출되어 AttributeEffect 배열이 대체됩니다.
	//virtual void OnOverrideSelfEffectEvent_Implementation(const TArray<UAbilityEffect*>& SelfEffect, AActor* Target) override;
};
