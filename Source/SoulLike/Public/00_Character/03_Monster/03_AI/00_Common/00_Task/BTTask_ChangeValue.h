// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ChangeValue.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UBTTask_ChangeBoolValue : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	
	UBTTask_ChangeBoolValue();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere)
	bool bNewValue = false;
};
