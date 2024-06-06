// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ChangeMovementState.generated.h"

/**
 * 몬스터의 걷기 혹은 달리기 상태를 변경합니다.
 */
UCLASS()
class SOULLIKE_API UBTTask_ChangeMovementState : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_ChangeMovementState();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	                            EBTNodeResult::Type TaskResult) override;
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere)
	EMovementState NewMovementState;
	UPROPERTY(EditAnywhere)
	bool bChangeSpeedMultiplier = false;
	UPROPERTY(EditAnywhere, meta=(EditCondition="bChangeSpeedMultiplier"))
	float SpeedMultiplier = 1.f;
};
