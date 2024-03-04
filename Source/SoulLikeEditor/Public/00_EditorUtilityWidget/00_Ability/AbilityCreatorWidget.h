// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "02_Ability/AbilityBase.h"
#include "UObject/ConstructorHelpers.h"
#include "AbilityCreatorWidget.generated.h"

UCLASS()
class SOULLIKEEDITOR_API UAbilityTableInfo : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FAbilityInformation AbilityInformation;
	//어빌리티 사용시 플레이할 몽타주 정보입니다.
	UPROPERTY(EditAnywhere)
	TArray<class UAnimMontage*> AnimMontages;
};

/**
 * 
 */
UCLASS()
class SOULLIKEEDITOR_API UAbilityCreatorWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UAbilityCreatorWidget();

	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_Class;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_SelectPath;

	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_TableList;

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_SelectPath;

	UPROPERTY(meta=(BindWidget))
	class UEditableTextBox* EditableTextBox_BlueprintName;

	UPROPERTY(meta=(BindWidget))
	class UAbilityCreatorWidget_Req* E_UMG_AbilityCreator_Req;

	UPROPERTY(meta=(BindWidget))
	class UDetailsView* DetailsView_AbilityInfo;

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Create;

	//이 위젯으로 생성 가능한 클래스 모음
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UAbilityBase>> CreatableAbilityClass;
	UPROPERTY()
	TMap<FString, TSubclassOf<class UAbilityBase>> CreatableAbilityMap;

	FString SelectedClassName;

	
	UPROPERTY()
	class UDataTable* CommonTable;
	UPROPERTY()
	UDataTable* ConsumeTable;
	UPROPERTY()
	UDataTable* ArmorTable;
	UPROPERTY()
	UDataTable* SpiritTable;
	UPROPERTY()
	UDataTable* RingTable;
	UPROPERTY()
	UDataTable* CoreTable;
	UPROPERTY()
	UDataTable* FragmentTable;

	UFUNCTION()
	void OnSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnClickedPathButton();


	UFUNCTION()
	void OnClickedCreateButton();

	virtual void NativePreConstruct() override;
	
	void AddToDataTable(FName AbilityTag, const FAbilityInformation& AbilityInformation);
};
