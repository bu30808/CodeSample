// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ClearFocus.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UBTTask_ClearFocus : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

protected:
	UBTTask_ClearFocus();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
