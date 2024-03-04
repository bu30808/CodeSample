// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTDecorator_CanSpawnMoreAlly.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UBTDecorator_CanSpawnMoreAlly : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
	//SpawnMonsters.Num() >= AllySpawnCount
	UBTDecorator_CanSpawnMoreAlly();
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
