// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "02_Ability/AbilityEffect.h"
#include "02_Ability/AbilityEffect_Linetrace.h"
#include "Logging/StructuredLog.h"
#include "AbilityEffectHandlerWidget.generated.h"


enum class EAttackType : uint8;
class UAbilityEffect;

UCLASS(Transient)
class UAbilityEffectHandler : public UObject
{
	GENERATED_BODY()

public:

	UAbilityEffectHandler();
	
	UPROPERTY(EditAnywhere,Transient)
	TObjectPtr<UAbilityEffect> AbilityEffect;
	
};

UCLASS(Transient)
class UAbilityEffectHandler_Linetrace : public UAbilityEffectHandler
{
	GENERATED_BODY()

public:

	UAbilityEffectHandler_Linetrace();
	
};
/**
 * 
 */
UCLASS()
class SOULLIKEEDITOR_API UAbilityEffectHandlerWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox_Player;
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox_Monster;
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox_Magic;
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox_Physic;
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox_Cost;

	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* HorizontalBox_Filter1;
	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* VerticalBox_Filter2;
	
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_Effect;
	UPROPERTY(meta=(BindWidget))
	class UDetailsView* DetailsView_Effect;

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Apply;

	UPROPERTY(Transient)
	TObjectPtr<UAbilityEffectHandler> EffectHandler;
	
	UPROPERTY()
	TMap<FString,TSubclassOf<class UAbilityEffect>> AbilityEffects;

	UPROPERTY()
	TArray<FString> SearchTag;

	TArray<TTuple<FString,TSubclassOf<class UAbilityEffect>>> CharacterFilteredEffect;

	UPROPERTY()
	TSubclassOf<UAbilityEffect> SelectedEffect;

	virtual void NativeConstruct() override;
	void UpdateOptionWithSearchTag();

	UFUNCTION()
	void OnPlayerCheckStateChanged(bool bIsChecked);
	UFUNCTION()
	void OnMonsterCheckStateChanged(bool bIsChecked);
	UFUNCTION()
	void OnMagicCheckStateChanged(bool bIsChecked);
	UFUNCTION()
	void OnPhysicCheckStateChanged(bool bIsChecked);
	UFUNCTION()
	void OnCostCheckStateChanged(bool bIsChecked);
	UFUNCTION()
	void OnSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnClickedApplyButton();

};

