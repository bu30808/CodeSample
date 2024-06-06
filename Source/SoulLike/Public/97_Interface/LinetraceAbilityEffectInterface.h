// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LinetraceAbilityEffectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULinetraceAbilityEffectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SOULLIKE_API ILinetraceAbilityEffectInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**************************라인트레이스*************************/
	//라인트레이스 이팩트를 사용할 때, 기본 MeshComponent 이외의 MeshComponent를 사용할 때, 덮어쓰세요.
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	UPrimitiveComponent* GetPrimitiveComponentForLineTrace();
	virtual UPrimitiveComponent* GetPrimitiveComponentForLineTrace_Implementation() { return nullptr; }
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	UPrimitiveComponent* GetPrimitiveComponentForAITurnTrace();
	virtual UPrimitiveComponent* GetPrimitiveComponentForAITurnTrace_Implementation() { return nullptr; }
};
