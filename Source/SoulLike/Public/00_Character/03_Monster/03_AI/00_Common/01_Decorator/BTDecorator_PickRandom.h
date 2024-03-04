// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "BTDecorator_PickRandom.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UBTDecorator_PickRandom : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
	UBTDecorator_PickRandom();
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
