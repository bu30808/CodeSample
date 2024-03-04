// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "AttributeProcessSubsystem.generated.h"

enum class EAttackType : uint8;
enum class EAttributeApplyMethod : uint8;
/**
 * 
 */
UCLASS()
class SOULLIKE_API UAttributeProcessSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<EAttributeApplyMethod, class UAttributeProcessor*> Processor;

public:
	UAttributeProcessSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAttributeProcessor* GetProcessor(EAttributeApplyMethod Method);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAttributeProcessor* GetReverseProcessor(EAttributeApplyMethod Method);

	TArray<FAttributeEffect> UpdateAttributeEffectsAffectedByOwnersAttribute(
		const class UAttributeComponent* AttributeComponent, TArray<FAttributeEffect> AttributeEffects,
		EAttackType AttackType);

};
