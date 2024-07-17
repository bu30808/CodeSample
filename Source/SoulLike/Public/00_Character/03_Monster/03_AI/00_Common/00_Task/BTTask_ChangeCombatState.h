// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ChangeCombatState.generated.h"

enum class ECombatState : uint8;
/**
 * 
 */
UCLASS()
class SOULLIKE_API UBTTask_ChangeCombatState : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	ECombatState NewCombatState;

	UBTTask_ChangeCombatState();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
