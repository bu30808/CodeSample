// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_GetDistance.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UBTService_GetDistance : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	UBTService_GetDistance();
	virtual FString GetStaticDescription() const override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
