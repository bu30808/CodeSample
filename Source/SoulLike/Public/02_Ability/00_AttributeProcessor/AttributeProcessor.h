// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "02_Ability/AbilityEffect.h"
#include "AttributeProcessor.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UAttributeProcessor : public UObject
{
	GENERATED_BODY()

public:
	//적용된 값을 리턴합니다.
	UFUNCTION(BlueprintCallable)
	virtual float ProcessAttributeEffect(class UAttributeComponent* AttributeComponent, const FAttributeEffect& Effect,
	                                     float DeltaTime = 1, UObject* AdditionalInformation = nullptr)
	{
		return 0;
	}

	void UpdateCharacterInfoToWidget(const FAttributeEffect& Effect, UAttributeComponent* AttributeComponent);
};

UCLASS()
class UAttributeProcessor_Add : public UAttributeProcessor
{
	GENERATED_BODY()

public:
	//적용된 값을 리턴합니다.
	virtual float ProcessAttributeEffect(class UAttributeComponent* AttributeComponent, const FAttributeEffect& Effect,
	                                     float DeltaTime = 1, UObject* AdditionalInformation = nullptr) override;
};


UCLASS()
class UAttributeProcessor_Remove : public UAttributeProcessor
{
	GENERATED_BODY()

public:
	//적용된 값을 리턴합니다.
	virtual float ProcessAttributeEffect(class UAttributeComponent* AttributeComponent, const FAttributeEffect& Effect,
	                                     float DeltaTime = 1, UObject* AdditionalInformation = nullptr) override;
};

UCLASS()
class UAttributeProcessor_Multiplication : public UAttributeProcessor
{
	GENERATED_BODY()

public:
	//적용된 값을 리턴합니다.
	virtual float ProcessAttributeEffect(class UAttributeComponent* AttributeComponent, const FAttributeEffect& Effect,
	                                     float DeltaTime = 1, UObject* AdditionalInformation = nullptr) override;
};

UCLASS()
class UAttributeProcessor_Division : public UAttributeProcessor
{
	GENERATED_BODY()

public:
	//적용된 값을 리턴합니다.
	virtual float ProcessAttributeEffect(class UAttributeComponent* AttributeComponent, const FAttributeEffect& Effect,
	                                     float DeltaTime = 1, UObject* AdditionalInformation = nullptr) override;
};
