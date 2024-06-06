// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "Blueprint/AIAsyncTaskBlueprintProxy.h"
#include "SoulLike/SoulLike.h"
#include "BTTask_MoveToAttackRange.generated.h"

/**
 * 
 */
/*
UCLASS()
class SOULLIKE_API UBTTask_MoveToAttackRange : public UBTTask_MoveTo
{
	GENERATED_BODY()

protected:
	UBTTask_MoveToAttackRange();


	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector AttackRangeKey;
	//이 값만큼 더 가까이 이동합니다.
	UPROPERTY(EditAnywhere)
	float ErrorMargin = 100.f;
};
*/

UCLASS()
class SOULLIKE_API UBTTask_MoveToAttackRange : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

protected:
	UBTTask_MoveToAttackRange();


	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	UFUNCTION()
	void OnMoveFinished(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector AttackRangeKey;
	//목표가 이 값 이하면 다 이동한것으로 합니다.
	UPROPERTY()
	float AcceptableRadius;
	//이 값만큼 더 가까이 이동합니다. 음수일경우 더 멀리서 멈춥니다.
	UPROPERTY(EditAnywhere)
	float OffsetDistance = 100.f;
	
	UPROPERTY(Transient)
	TWeakObjectPtr<class AAIController> AICon;
};
