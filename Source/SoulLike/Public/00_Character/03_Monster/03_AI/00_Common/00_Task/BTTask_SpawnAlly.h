// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_SpawnAlly.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UBTTask_SpawnAlly : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

protected:
	UBTTask_SpawnAlly();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere)
	int32 SpawnCount = 0;

	FOnFinishSpawnAlly OnFinishSpawnAlly;

	UFUNCTION()
	void FinishTaskEvent(UBehaviorTreeComponent* OwnerComp, EBTNodeResult::Type Result);
};
