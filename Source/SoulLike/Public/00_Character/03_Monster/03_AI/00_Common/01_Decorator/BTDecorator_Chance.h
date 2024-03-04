// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "BTDecorator_Chance.generated.h"

/**
 * 노드가 실행될 확률을 정합니다.
 */
UCLASS()
class SOULLIKE_API UBTDecorator_Chance : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTDecorator_Chance();

	//0~1사이의 확률을 입력해 주세요.
	UPROPERTY(EditAnywhere, meta=(ClampMax = 1.f, ClampMim = 0.f))
	float ActiveChance = 0.f;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;
};
