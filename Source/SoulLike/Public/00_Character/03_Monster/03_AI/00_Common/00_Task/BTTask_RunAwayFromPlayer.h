// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_RunAwayFromPlayer.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UBTTask_RunAwayFromPlayer : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	/*UBTTask_RunAwayFromPlayer();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	                            EBTNodeResult::Type TaskResult) override;*/

public:
	UBTTask_RunAwayFromPlayer();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	FOnAIMoveComplete OnAIMoveComplete;
	
private:
	FVector GetRunAwayLocation(AActor* AIActor, AActor* PlayerActor) const;
	UFUNCTION()
	void OnMoveCompleteEvent(FAIRequestID RequestID, EPathFollowingResult::Type Result);
};
