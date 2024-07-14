// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "00_Character/04_NPC/99_Component/MerchantComponent.h"
#include "02_Ability/AbilityBase.h"
#include "03_Widget/06_Enhancement/EnhancementWidget.h"
#include "03_Widget/99_Drag/DropTargetWidget.h"
#include "Blueprint/UserWidget.h"
#include "ItemListWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerBuyItemFromNPC, const FMerchandiseItem&, MerchandiseItem);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerBuyAbilityFromNPC, const FMerchandiseAbility&, MerchandiseAbility);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddListViewWidget, UUserWidget*, Widget);


UCLASS()
class UInventoryData : public UObject
{
	GENERATED_BODY()

public:
	TWeakObjectPtr<class UItemButtonWidget> OwnItemButtonWidget;
};


UCLASS()
class UItemData : public UInventoryData
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	FInventoryItem InventoryItem;
	FOnPlayerBuyItemFromNPC OnPlayerBuyItemFromNPC;
};


UCLASS()
class UAbilityData : public UInventoryData
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	FAbilityInformation AbilityInformation;
};

UENUM(BlueprintType)
enum class EItemListType : uint8
{
	INVENTORY,
	EQUIPMENT,
	MERCHANT,
	ENHANCEMENT
};

/**
 * 
 */
UCLASS()
class SOULLIKE_API UItemListWidget : public UDropTargetWidget
{
	GENERATED_BODY()

	friend class UEquipWidget;

protected:
	UPROPERTY(meta=(BindWidget))
	class UScaleBox* ScaleBox_Filter;

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_All;
	UPROPERTY(meta=(BindWidget))
	UButton* Button_Consume;
	UPROPERTY(meta=(BindWidget))
	UButton* Button_Spirit;
	UPROPERTY(meta=(BindWidget))
	UButton* Button_Armor;
	UPROPERTY(meta=(BindWidget))
	UButton* Button_Ring;
	UPROPERTY(meta=(BindWidget))
	UButton* Button_Enhancement;
	UPROPERTY(meta=(BindWidget))
	UButton* Button_Ability;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Key;

	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	class UListView* ListView_Item;

	UPROPERTY(meta=(BindWidget))
	UButton* Button_Close;

	UPROPERTY()
	TWeakObjectPtr<APlayerCharacter> Owner;

	UPROPERTY()
	TMap<FGuid, class UItemData*> ListViewItems;
	UPROPERTY()
	TMap<FGameplayTag, class UAbilityData*> ListViewAbilities;


	void OnEntryWidgetGeneratedEvent(UUserWidget& UserWidget);

	virtual void NativeConstruct() override;
	UFUNCTION()
	void OnClickedCloseButton();

	virtual void OnVisibilityChangedEvent(ESlateVisibility InVisibility) override;

public:
	UFUNCTION()
	void CreateItemList_All();
	UFUNCTION()
	void CreateItemList_Consume();
	UFUNCTION()
	void CreateItemList_Sprite();
	UFUNCTION()
	void CreateItemList_Armor();
	UFUNCTION()
	void CreateItemList_Ring();
	UFUNCTION()
	void CreateItemList_Ability();
	UFUNCTION()
	void CreateItemList_Enhancement();
	UFUNCTION()
	void CreateItemList_Key();

	//아이템 목록과 어빌리티 목록을 읽어와 리스트를 구성합니다.
	void CreateInventoryItemList();
	//리스트뷰가 뒤늦게 갱신되는 문제를 우회하기 위해 사용하는 함수입니다. EnhancementWidget을 참고하세요.
	UFUNCTION()
	void HideWidgetIfNotSpirit(UUserWidget* Widget);
	UFUNCTION()
	void HideWidgetIfNotArmor(UUserWidget* Widget);

	//아이템 정보에 해당하는 리스트뷰의 위젯을 찾아서 정보를 새로고침합니다.
	void RefreshFromInventoryItem(const FInventoryItem& Item);
	//생성된 아이템 버튼이 호버되었을 때 호출됩니다.
	UFUNCTION()
	void OnItemButtonHovered(class UInventoryData* Data);
	//특정 유니크 아이디를 가진 아이템을 표시하는 버튼을 찾아서 새로고칩니다.
	void RefreshItemButton(const FGuid& ItemUniqueID);

protected:
	//어빌리티 종류를 정부 숨깁니다.
	void HiddenAbility() const;
	//모든 아이템을 숨깁니다.
	void HiddenItem() const;
	//소비 아이템을 전부 숨깁니다
	void HiddenConsumeItem() const;
	//장비를 숨깁니다.
	void HiddenEquipment() const;

	UFUNCTION()
	void OnAddItemEvent(class ABaseCharacter* UsedBy, const FInventoryItem& ItemInfo, AItemActor* ItemActor);
	UFUNCTION()
	void OnRemoveItemEvent(ABaseCharacter* Player, const FGuid& ItemUniqueID);
	UFUNCTION()
	void OnInventoryUpdateEvent(const FGuid& ItemUniqueID, const int32& NewCount);


	UFUNCTION()
	void OnAddAbilityEvent(const FAbilityInformation& AbilityInformation);
	UFUNCTION()
	void OnRemoveAbilityEvent(const FGameplayTag& AbilityTag);

	bool IsAlreadyContain(const FInventoryItem& ItemInfo) const;


	bool IsOrbItem(const FInventoryItem& ItemInfo) const;

	UPROPERTY()
	EItemListType ItemListType;

public:
	EItemListType GetItemListType() const { return ItemListType; }

	UPROPERTY()
	TWeakObjectPtr<UUserWidget> ParentsUMG;

	//퀵슬롯 세팅창에서 이용할 설정으로 변경합니다.
	void QuickSlotSetting() const;
	//장비변경창에서 이용할 설정으로 변경합니다.
	void EquipSetting(class UEquipWidget* ParentsWidget);
	//상점창용 설정
	void MerchantSetting();
	//인벤토리 설정
	void InventorySetting(class UInventoryWidget* ParentsWidget);
	//강화창용 설정
	void EnhancementSetting(UEnhancementWidget* ParentsWidget);

	//플레이어가 npc에게 아이템을 구입하면 호출되는 델리게이트입니다.
	FOnPlayerBuyItemFromNPC OnPlayerBuyItemFromNPC;
	FOnPlayerBuyAbilityFromNPC OnPlayerBuyAbilityFromNPC;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	                          UDragDropOperation* InOperation) override;

	void HideFilter();
	void HideCloseButton();

	//리스트뷰에 아이템이 추가될 때 사용됩니다.
	FOnAddListViewWidget OnAddListViewWidget;
};
