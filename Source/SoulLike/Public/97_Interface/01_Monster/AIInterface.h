// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AIInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAIInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SOULLIKE_API IAIInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GetAttackRange(float& Range);
public:
	//ai 크기에 맞춰 스케일링한 뒤, 캡슐반지름을 더한 값을 리턴합니다.
	UFUNCTION()
	virtual float GetScaledAttackRange(float Original);
	//virtual float GetScaledAttackRange_Implementation(float Original);
};
