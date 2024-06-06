// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_Strafe.generated.h"

UENUM(BlueprintType)
enum class EStrafeDirection : uint8
{
	LEFT,
	RIGHT,
	RANDOM
};

/**
 * 
 */
UCLASS(Blueprintable)
class SOULLIKE_API UEnvQueryContext_Strafe : public UEnvQueryContext
{
	GENERATED_BODY()
protected:


	UPROPERTY(EditAnywhere)
	EStrafeDirection StrafeDirection;
	//쿼리할 최대 거리
	UPROPERTY(EditAnywhere)
	float Distance = 750.f;

	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
	
};
