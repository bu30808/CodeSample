// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ExecutionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UExecutionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SOULLIKE_API IExecutionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.


public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool GetIsExecutionable();
	virtual bool GetIsExecutionable_Implementation() { return false; }

	//처형당할 몽타주를 가져옵니다.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	class UAnimMontage* GetExecutedMontage();
	virtual class UAnimMontage* GetExecutedMontage_Implementation() { return nullptr; }

	/*UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	class UAnimMontage* GetExecutedEndMontage();
	virtual class UAnimMontage* GetExecutedEndMontage_Implementation() { return nullptr; }*/
};
