// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "BTDecorator_IsValidTarget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UBTDecorator_IsValidTarget : public UBTDecorator_Blackboard
{
	GENERATED_BODY()

protected:
	UBTDecorator_IsValidTarget();
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
