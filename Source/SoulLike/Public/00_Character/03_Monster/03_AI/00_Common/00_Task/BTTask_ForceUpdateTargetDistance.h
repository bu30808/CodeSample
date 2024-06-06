// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ForceUpdateTargetDistance.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UBTTask_ForceUpdateTargetDistance : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UBTTask_ForceUpdateTargetDistance();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
