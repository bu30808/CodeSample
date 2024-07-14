// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "BTDecorator_IsInAttackRange.generated.h"

/**
 * 공격범위 안인가 확인하는 데코레이터입니다.
 */
UCLASS()
class SOULLIKE_API UBTDecorator_IsInAttackRange : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

protected:
	UBTDecorator_IsInAttackRange();

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector AttackRangeKey;

	//이 값만큼 기본 공격범위에 더해서 확인합니다.
	UPROPERTY(EditAnywhere)
	float OffsetDistance = 100.f;
	
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};

