// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "02_Ability/AbilityBase.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "AttackChainSubsystem.generated.h"

USTRUCT()
struct FAbilityChainData
{
	GENERATED_BODY()
public:
	UPROPERTY(Transient)
	float ChainValue = 0;
	UPROPERTY(Transient)
	TWeakObjectPtr<class UAbilityBase> ChainFrom = nullptr;

	FAbilityChainData(){ }
	
	FAbilityChainData(float Value, class UAbilityBase* From):ChainValue(Value),ChainFrom(From)
	{
		
	}
};

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
	TMap<FGameplayTag, FAbilityChainData> AttackChainMap;

public:
	//배율을 저장합니다.
	//체인 태그로 저장합니다.
	UFUNCTION(BlueprintCallable)
	void AddChainValue(FGameplayTag ChainTag, float Value, UAbilityBase* From);
	//저장된 배율을 꺼내옵니다. 꺼내면 저장된 값은 사라집니다.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetChainValue(FGameplayTag ChainTag);

};
