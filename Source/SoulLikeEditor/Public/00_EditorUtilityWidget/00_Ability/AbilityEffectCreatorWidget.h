// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "GameplayTagContainer.h"
#include "02_Ability/AbilityEffect.h"
#include "02_Ability/AbilityEffect_Linetrace.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilityEffectCreatorWidget.generated.h"

UCLASS()
class SOULLIKEEDITOR_API UNormalEffectInfo : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Transient)
	UAbilityEffect* Effect = NewObject<UAbilityEffect>();
};

UCLASS()
class SOULLIKEEDITOR_API ULinetraceEffectInfo : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Transient)
	UAbilityEffect_Linetrace* Effect = NewObject<UAbilityEffect_Linetrace>();
};

/**
 * 
 */
UCLASS()
class SOULLIKEEDITOR_API UAbilityEffectCreatorWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_EffectType;

	UPROPERTY(meta=(BindWidget))
	class UDetailsView* DetailsView_Effect;


	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_SelectPath;

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_SelectPath;

	UPROPERTY(meta=(BindWidget))
	class UEditableTextBox* EditableTextBox_BlueprintName;


	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Create;

	FString SelectedType;

	UFUNCTION()
	void OnSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnClicked();

	UFUNCTION()
	void OnClickedPath();

	virtual void NativePreConstruct() override;
};
