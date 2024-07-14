// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/03_Monster/03_AI/00_Common/01_Decorator/BTDecorator_CheckCharacterState.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ChangeCharacterState.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UBTTask_ChangeCharacterState : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere)
	ECharacterState NewState;

	UBTTask_ChangeCharacterState();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
