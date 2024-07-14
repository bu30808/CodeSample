// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "GameplayTagContainer.h"
#include "ToolMenuEntry.h"
#include "ToolMenuEntryScript.h"
#include "04_Item/ItemActor.h"
#include "UObject/ConstructorHelpers.h"
#include "ItemCreatorWidget.generated.h"

UCLASS(Transient)
class SOULLIKEEDITOR_API UItemInformationBase : public UObject
{
	GENERATED_BODY()

public:
	virtual FGameplayTag GetItemTag() { return FGameplayTag::EmptyTag; }
};

UCLASS(Transient)
class UCommonInfo : public UItemInformationBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Transient)
	FItemInformation ItemInformation;

	virtual FGameplayTag GetItemTag() override { return ItemInformation.Item_Tag; }
};


UCLASS(Transient)
class USpiritInfo : public UItemInformationBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Transient)
	FWeaponInformation WeaponInformation;

	virtual FGameplayTag GetItemTag() override { return WeaponInformation.Item_Tag; }
};

UCLASS(Transient)
class UEquipInfo : public UItemInformationBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Transient)
	FEquipmentInformation EquipInformation;

	virtual FGameplayTag GetItemTag() override { return EquipInformation.Item_Tag; }
};

UCLASS(Transient)
class UConsumeInfo : public UItemInformationBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Transient)
	FConsumeInformation ConsumeInformation;

	virtual FGameplayTag GetItemTag() override { return ConsumeInformation.Item_Tag; }
};


UCLASS(Transient)
class UCoreInfo : public UItemInformationBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Transient)
	FOrbCoreInformation OrbCoreInformation;

	virtual FGameplayTag GetItemTag() override { return OrbCoreInformation.Item_Tag; }
};

UCLASS(Transient)
class UFragmentInfo : public UItemInformationBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Transient)
	FOrbFragmentInformation OrbFragmentInformation;

	virtual FGameplayTag GetItemTag() override { return OrbFragmentInformation.Item_Tag; }
};


/**
 * 
 */
UCLASS()
class SOULLIKEEDITOR_API UItemCreatorWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

	UItemCreatorWidget();

protected:
	UPROPERTY()
	class UDataTable* SpiritTable;
	UPROPERTY()
	class UDataTable* SpiritAbilityTable;


	UPROPERTY()
	class UDataTable* ArmorTable;
	UPROPERTY()
	class UDataTable* ArmorAbilityTable;

	UPROPERTY()
	class UDataTable* RingTable;
	UPROPERTY()
	class UDataTable* RingAbilityTable;

	UPROPERTY()
	class UDataTable* CoreTable;
	UPROPERTY()
	class UDataTable* CoreAbilityTable;

	UPROPERTY()
	class UDataTable* ConsumeTable;
	UPROPERTY()
	class UDataTable* ConsumeAbilityTable;

	UPROPERTY()
	UDataTable* EnhancementItemTable;


	UPROPERTY(Transient)
	FString SelectedClassName;
	
	UPROPERTY(Transient)
	TObjectPtr<USpiritInfo> SpiritHandler;
	UPROPERTY(Transient)
	TObjectPtr<UEquipInfo> ArmorHandler;
	UPROPERTY(Transient)
	TObjectPtr<UEquipInfo> RingHandler;
	UPROPERTY(Transient)
	TObjectPtr<UCoreInfo> CoreHandler;
	UPROPERTY(Transient)
	TObjectPtr<UFragmentInfo> FragmentHandler;
	UPROPERTY(Transient)
	TObjectPtr<UConsumeInfo> ConsumeHandler;
	UPROPERTY(Transient)
	TObjectPtr<UCommonInfo> CommonHandler;

public:
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_Class;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_SelectPath;

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_SelectPath;

	UPROPERTY(meta=(BindWidget))
	class UEditableTextBox* EditableTextBox_BlueprintName;

	UPROPERTY(meta=(BindWidget))
	class UDetailsView* DetailsView_Item;

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Create;
	
	//이 아이템을 사용했을 때, 바로 사용할 어빌리티
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UAbilityBase>> Abilities;


	//이 위젯으로 생성 가능한 클래스 모음
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class AItemActor>> CreatableItemActorClass;
	UPROPERTY()
	TMap<FString, TSubclassOf<class AItemActor>> CreatableItemActorMap;

	UFUNCTION()
	void OnClickedPathButton();

	UFUNCTION()
	void OnSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	//선택된 클래스에 따라 데이터테이블에 아이템 정보를 추가합니다.
	void AddTableInfo(UBlueprint* ItemBlueprint);
	UFUNCTION()
	void OnClickedCreateButton();

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Menu;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Label;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ToolTip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FToolMenuEntry MenuEntry;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FScriptSlateIcon Icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Section;
	/*UFUNCTION(BlueprintCallable)
	void HookToolBar();*/
	void DestroyAllHandler();
};
