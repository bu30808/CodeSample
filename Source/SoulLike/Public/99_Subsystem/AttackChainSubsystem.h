// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "AttackChainSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UAttackChainSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	//공격 배율을 저장할 맵입니다.
	//체인 태그로 구분합니다.
	UPROPERTY(VisibleAnywhere)
	TMap<FGameplayTag, float> AttackChainMap;

public:

	
	//배율을 저장합니다.
	//체인 태그로 저장합니다.
	UFUNCTION(BlueprintCallable)
	void AddChainValue(FGameplayTag ChainTag, float Value);
	//저장된 배율을 꺼내옵니다. 꺼내면 저장된 값은 사라집니다.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetChainValue(FGameplayTag ChainTag);
};
