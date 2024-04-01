// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_GetMoveLocation.generated.h"

enum class EDirection : uint8;
/**
 * MoveTo Task로 이동할 좌표를 구해서 블랙보드에 저장합니다.
 */
UCLASS()
class SOULLIKE_API UBTTask_GetMoveLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	EDirection Direction;
	UPROPERTY(EditAnywhere)
	float Distance;

	UBTTask_GetMoveLocation();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
