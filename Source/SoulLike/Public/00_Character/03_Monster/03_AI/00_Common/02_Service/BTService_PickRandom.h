// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_PickRandom.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UBTService_PickRandom : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	//
	UPROPERTY(EditAnywhere)
	int32 MaxRange = 0;

	UBTService_PickRandom();
};
