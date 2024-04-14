// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "03_Widget/01_Menu/00_Inventory/ItemListWidget.h"
#include "Blueprint/UserWidget.h"
#include "EquipWidget.generated.h"


/**
 *
 */
UCLASS()
class SOULLIKE_API UEquipWidget : public UPopUpBasedWidget
{
	GENERATED_BODY()
	friend class UDEPRECATED_MainMenuWidget;
	friend class UEquipButtonWidget;

protected:
	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* VerticalBox;

	UPROPERTY(meta=(BindWidget))
	class UEquipButtonWidget* UMG_EquipSpirit;
	UPROPERTY(meta=(BindWidget))
	class UEquipButtonWidget* UMG_EquipArmor;

	//기본적으로 열려있는 장신구 슬롯입니다.
	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* HorizontalBox_Ring1;
	//2번 슬롯은 특성으로 열립니다.
	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* HorizontalBox_Ring2;

	UPROPERTY()
	TArray<class UEquipButtonWidget*> AllEquipSlots;
	UPROPERTY()
	TArray<class UEquipButtonWidget*> RingSlots;

	UPROPERTY(meta=(BindWidget))
	class UItemListWidget* UMG_ItemList;
	UPROPERTY(meta=(BindWidget))
	class UItemInfoWidget* UMG_ItemInfo;

	UPROPERTY(meta=(BindWidget))
	class UUniformGridPanel* UniformGridPanel_Consume;
	UPROPERTY(meta=(BindWidget))
	class UUniformGridPanel* UniformGridPanel_Ability;


	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;


	virtual void OnVisibilityChangedEvent(ESlateVisibility InVisibility) override;

public:
	UFUNCTION(BlueprintCallable)
	void AddRingSlot();
	UFUNCTION(BlueprintCallable)
	void RemoveRingSlot();

	int32 FindIndexByID(FGuid UniqueID);
	class UEquipButtonWidget* GetEquipButtonByIndex(int32 FindIndex);

	void ShowItemInformation(class UInventoryData* Data);

	void ForceUpdateSpirit(const FInventoryItem& ItemData);
	class UEquipButtonWidget* GetRingSlotByUniqueID(const FGuid& UniqueID, int32& OutIndex);

	TArray<class UWidget*> GetAllConsumeQuickSlots();
	TArray<class UWidget*> GetAllAbilityQuickSlots();
	void LoadConsumeQuickSlots(const TMap<int32, FGuid>& ConsumeQuick);
	void LoadAbilityQuickSlots(const TMap<int32, FGameplayTag>& AbilityQuick);

protected:
	UFUNCTION(BlueprintCallable)
	void OnRemoveAlreadyRegisteredSlotEvent(UInventoryData* Data);
};
