// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "AttributeHandlerWidget.generated.h"


UCLASS()
class UAttributePerPointHandler : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Transient)
	FAttributePerPoint AttributePerPoint;
};


UCLASS()
class UAttributeInitHandler : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Transient)
	FAttributeInit AttributeInit;
	UPROPERTY()
	class UDataTable* TablePointer;
	UPROPERTY()
	TWeakObjectPtr<class ABaseCharacter> CharacterPointer;
};
/**
 * 
 */
UCLASS()
class SOULLIKEEDITOR_API UAttributeHandlerWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

	
public:

	UPROPERTY(EditAnywhere)
	class UDataTable* AttributePerPointTable;

	UPROPERTY(Transient)
	TObjectPtr<UAttributeInitHandler> InitHandler;
	UPROPERTY(Transient)
	TObjectPtr<UAttributePerPointHandler> PerPointHandler;

	
	UPROPERTY()
	TMap<FString,TSubclassOf<class ABaseCharacter>> BaseCharacters;

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_AttPerPoint;
	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* VerticalBox_AttPerPoint;
	UPROPERTY(meta=(BindWidget))
	class UDetailsView* DetailsView_AttPerPoint;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_SaveAttPerPoint;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_AttInit;
	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* VerticalBox_AttInit;
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_AttInit;
	UPROPERTY(meta=(BindWidget))
	class UDetailsView* DetailsView_AttInit;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_SaveAttInit;

	//void LoadBlueprintClass();
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnClickedAttPerPoint();
	UFUNCTION()
	void OnClickedAttInit();
	UFUNCTION()
	void OnSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnClickedSaveAttInit();
	UFUNCTION()
	void OnClickedSaveAttPerPoint();

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
};
