// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Engine/DataAsset.h"
#include "AbilityTagHandlerWidget.generated.h"

UCLASS()
class UTagTableAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	class UDataTable* CommonTags;
	
	UPROPERTY(EditAnywhere)
	class UDataTable* PlayerCharacterTypeTags;
	UPROPERTY(EditAnywhere)
	class UDataTable* PlayerCharacterAbilityTags;
	UPROPERTY(EditAnywhere)
	class UDataTable* PlayerCharacterAbilityCueTags;
	
	UPROPERTY(EditAnywhere)
	class UDataTable* MonsterTags;
	UPROPERTY(EditAnywhere)
	class UDataTable* MonsterCueTags;

	UPROPERTY(EditAnywhere)
	class UDataTable* ConsumeItemTags;
	UPROPERTY(EditAnywhere)
	class UDataTable* ConsumeItemCueTags;

	UPROPERTY(EditAnywhere)
	class UDataTable* EquipmentItemTags;
	UPROPERTY(EditAnywhere)
	class UDataTable* EquipmentItemCueTags;

	UPROPERTY(EditAnywhere)
	class UDataTable* OrbCoreTags;
	UPROPERTY(EditAnywhere)
	class UDataTable* FragmentTags;
	UPROPERTY(EditAnywhere)
	class UDataTable* FragmentCueTags;
	UPROPERTY(EditAnywhere)
	class UDataTable* OrbLineCompleteRewardTags;
};

/**
 * 
 */
UCLASS()
class SOULLIKEEDITOR_API UAbilityTagHandlerWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()
protected:
	
	UPROPERTY(EditAnywhere)
	class UTagTableAsset* TagTableAsset;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTagRowElementWidget> TagRowElementWidgetObject;



	
	UPROPERTY(meta=(BindWidget))
	class UEditableTextBox* EditableTextBox_NewTag;
	UPROPERTY(meta=(BindWidget))
	class UEditableTextBox* EditableTextBox_NewTagComment;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_AddTag;
	
	UPROPERTY()
	class UDataTable* CurDataTable;

	
	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* VerticalBox_ComboBox;
	UPROPERTY()
	TArray<class UWidget*> ComboBoxes;
	
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_CommonFilter;
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_PlayerCharacterFilter;
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_MonsterFilter;
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_ItemCommonFilter;
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_ItemConsumeFilter;
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_ItemEquipmentFilter;
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_ItemCoreAndFragmentFilter;

	UPROPERTY(meta=(BindWidget))
	class UScrollBox* ScrollBox_TagList;

	UPROPERTY(EditAnywhere)
	TArray<FString> CommonFilter = {TEXT("선택"),TEXT("공통"),TEXT("플레이어"),TEXT("몬스터"),TEXT("아이템")};
	UPROPERTY(EditAnywhere)
	TArray<FString> PlayerCharacterFilter = {TEXT("선택"),TEXT("캐릭터 타입"),TEXT("캐릭터 어빌리티 태그"),TEXT("어빌리티 큐")};
	UPROPERTY(EditAnywhere)
	TArray<FString> MonsterFilter = {TEXT("선택"),TEXT("몬스터 타입 및 어빌리티"),TEXT("몬스터 큐")};
	UPROPERTY(EditAnywhere)
	TArray<FString> ItemCommonFilter = {TEXT("선택"),TEXT("소비 아이템"),TEXT("장비 아이템")};
	UPROPERTY(EditAnywhere)
	TArray<FString> ItemConsumeFilter = {TEXT("선택"),TEXT("아이템 태그"),TEXT("어빌리티 큐")};
	UPROPERTY(EditAnywhere)
	TArray<FString> ItemEquipmentFilter = {TEXT("선택"),TEXT("아이템 태그"),TEXT("어빌리티 큐"),TEXT("코어 및 파편")};
	UPROPERTY(EditAnywhere)
	TArray<FString> ItemCoreAndFragmentFilter = {TEXT("선택"),TEXT("코어"),TEXT("파편"),TEXT("파편 큐"),TEXT("라인 완성 보너스")};


	virtual void NativeConstruct() override;
	UFUNCTION()
	void OnComboBoxString_CommonFilter(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnComboBoxString_PlayerCharacterFilter(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnComboBoxString_MonsterFilter(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnComboBoxString_ItemCommonFilter(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnComboBoxString_ItemConsumeFilter(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnComboBoxString_ItemEquipmentFilter(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnComboBoxString_ItemCoreAndFragmentFilter(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnNewTagCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void OnClickedAdd();

	void AddOptions(class UComboBoxString* ComboBox,const TArray<FString>& Filter);
	void CreateTagList(UDataTable* DataTable);
	void SetVisibleButOtherHidden(TArray<UComboBoxString*> ComboBoxStringArr);

	UFUNCTION()
	void OnRefresh(UDataTable* DataTable);
};
