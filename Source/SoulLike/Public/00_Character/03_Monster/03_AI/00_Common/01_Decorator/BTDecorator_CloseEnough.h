// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "BTDecorator_CloseEnough.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UBTDecorator_CloseEnough : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

public:
	//이 거리보다 가까운지 확인합니다
	UPROPERTY(EditAnywhere)
	float Distance = 0.f;

	UBTDecorator_CloseEnough();
	virtual FString GetStaticDescription() const override;
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
