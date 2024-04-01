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

	//공격범위에서 이 값을 뺀 범위를 최소범위로 봅니다.
	UPROPERTY(EditAnywhere)
	float InMargin = 50.f;
	//공격범위에서 이 값을 더한 범위를 최대범위로 봅니다.
	UPROPERTY(EditAnywhere)
	float OutMargin = 50.f;
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};

/*
 * 공격가능한 범위보다 더 가까운지 확인하는 데코레이터입니다.
 */
UCLASS()
class SOULLIKE_API UBTDecorator_IsCloserThanAttackRange : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

protected:
	UBTDecorator_IsCloserThanAttackRange();

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector AttackRangeKey;

	//항상 양수값이어야 합니다.
	UPROPERTY(EditAnywhere)
	float ErrorMargin = 50.f;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};

/*
 * 공격범위보다 멀리 있는지 확인합니다.
 */
UCLASS()
class SOULLIKE_API UBTDecorator_IsFartherThanAttackRange : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

protected:
	UBTDecorator_IsFartherThanAttackRange();

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector AttackRangeKey;

	//항상 양수값이어야 합니다.
	UPROPERTY(EditAnywhere)
	float ErrorMargin = 50.f;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
