// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "02_Ability/AbilityBase.h"
#include "02_Ability/AbilityCue.h"
#include "Blueprint/UserWidget.h"
#include "AbilityDebugWidget.generated.h"


UCLASS()
class UAbilityDebuggingData : public UObject
{
	GENERATED_BODY()

public:
	FAbilityInformation AbilityInformation;
};

UCLASS()
class UAbilityEffectDebuggingData : public UObject
{
	GENERATED_BODY()

public:
	FGameplayTag AbilityEffectTag;
};

UCLASS()
class UAbilityCueDebuggingData : public UObject
{
	GENERATED_BODY()

public:
	FGameplayTag CueTag;
};

UCLASS()
class UIgnoreMoveInputDebuggingData : public UObject
{
	GENERATED_BODY()

public:
	
	TWeakObjectPtr<AActor> By;
	FGameplayTag Tag;
	int32 Count;
};



/**
 * 
 */
UCLASS()
class SOULLIKE_API UAbilityDebugWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_PlayerState;

	
	UPROPERTY(meta=(BindWidget))
	class UListView* AvailableAbilitiesList;
	UPROPERTY(meta=(BindWidget))
	class UListView* ActivatedAbilitiesList;
	UPROPERTY(meta=(BindWidget))
	class UListView* AppliedEffectsList;
	UPROPERTY(meta=(BindWidget))
	class UListView* AppliedCueList;
	UPROPERTY(meta=(BindWidget))
	class UListView* AppliedIgnoreInputList;

	
	virtual void NativeConstruct() override;

	UPROPERTY()
	TMap<FGameplayTag, class UAbilityDebuggingData*> AvailableAbilities;
	UPROPERTY()
	TMap<FGameplayTag, class UAbilityDebuggingData*> ActivatedAbilities;
	UPROPERTY()
	TMap<FGameplayTag, class UAbilityEffectDebuggingData*> AppliedEffects;
	UPROPERTY()
	TMap<FGameplayTag, class UAbilityCueDebuggingData*> AddedCues;

	UFUNCTION()
	void OnAddAbilityEvent(const FAbilityInformation& AbilityInformation);
	UFUNCTION()
	void OnRemoveAbilityEvent(const FGameplayTag& AbilityTag);

	UFUNCTION()
	void OnActivatedAbilityEvent(const FAbilityInformation& AbilityInformation);
	UFUNCTION()
	void OnEndAbilityEvent();

	UFUNCTION()
	void OnApplyEffectEvent(const FGameplayTagContainer& EffectTag);
	UFUNCTION()
	void OnEndEffectEvent(const FGameplayTagContainer& EffectTag);


	UFUNCTION()
	void OnAddCueEvent(const FGameplayTag& CueTag);
	UFUNCTION()
	void OnRemoveCueEvent(const FGameplayTag& CueTag);
	
	UFUNCTION()
	void OnTriggerIgnoreMoveInputEvent(const TMap<FIgnoreInputHandler, uint8>& IgnoreMoveInputMap);

	UFUNCTION()
	void OnChangePlayerStateEvent(EPlayerCharacterState State);
	
};
