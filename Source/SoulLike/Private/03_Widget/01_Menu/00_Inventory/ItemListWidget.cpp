// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/00_Inventory/ItemListWidget.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "03_Widget/01_Menu/00_Inventory/InventoryWidget.h"
#include "03_Widget/01_Menu/00_Inventory/ItemButtonWidget.h"
#include "03_Widget/01_Menu/03_Equipment/EquipWidget.h"
#include "03_Widget/04_Merchant/MerchantButtonWidget.h"
#include "04_Item/ItemActor.h"
#include "96_Library/ItemHelperLibrary.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Components/ScaleBox.h"
#include "Logging/StructuredLog.h"

void UItemListWidget::OnEntryWidgetGeneratedEvent(UUserWidget& UserWidget)
{
	OnAddListViewWidget.Broadcast(&UserWidget);
	ListView_Item->SetScrollbarVisibility(ESlateVisibility::Visible);

	if (UserWidget.IsA<UItemButtonWidget>())
	{
		if (auto button = Cast<UItemButtonWidget>(&UserWidget))
		{
			if (button->GetInventoryData()->IsA<UItemData>())
			{
				button->RefreshItemData(this);
			}
		}
	}
}

void UItemListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ListView_Item->ClearListItems();
	ListView_Item->OnEntryWidgetGenerated().AddUObject(this, &UItemListWidget::OnEntryWidgetGeneratedEvent);

	Button_All->OnClicked.AddUniqueDynamic(this, &UItemListWidget::CreateItemList_All);
	Button_Close->OnClicked.AddUniqueDynamic(this, &UItemListWidget::OnClickedCloseButton);
	Button_Consume->OnClicked.AddUniqueDynamic(this, &UItemListWidget::CreateItemList_Consume);
	Button_Spirit->OnClicked.AddUniqueDynamic(this, &UItemListWidget::CreateItemList_Sprite);
	Button_Armor->OnClicked.AddUniqueDynamic(this, &UItemListWidget::CreateItemList_Armor);
	Button_Ring->OnClicked.AddUniqueDynamic(this, &UItemListWidget::CreateItemList_Ring);
	Button_Ability->OnClicked.AddUniqueDynamic(this, &UItemListWidget::CreateItemList_Ability);
	Button_Enhancement->OnClicked.AddUniqueDynamic(this, &UItemListWidget::CreateItemList_Enhancement);
	Button_Key->OnClicked.AddUniqueDynamic(this,&UItemListWidget::CreateItemList_Key);

	OnVisibilityChanged.AddUniqueDynamic(this, &UItemListWidget::OnVisibilityChangedEvent);

	Owner = GetOwningPlayerPawn<APlayerCharacter>();
	if (Owner.IsValid())
	{
		if (auto invenComp = Owner->GetInventoryComponent())
		{
			invenComp->OnAddItem.AddUniqueDynamic(this, &UItemListWidget::OnAddItemEvent);
			invenComp->OnAddAbilityItem.AddUniqueDynamic(this, &UItemListWidget::OnAddAbilityItemEvent);
			invenComp->OnRemoveItem.AddUniqueDynamic(this, &UItemListWidget::OnRemoveItemEvent);
			invenComp->OnInventoryWidgetUpdate.AddUniqueDynamic(this, &UItemListWidget::OnInventoryUpdateEvent);
			OnAddListViewWidget.AddUniqueDynamic(invenComp, &UInventoryComponent::OnItemButtonWidgetGeneratedEvent);
		}

		if (auto abComp = Owner->GetAbilityComponent())
		{
			abComp->OnAddAbility.AddUniqueDynamic(this, &UItemListWidget::OnAddAbilityEvent);
			abComp->OnRemoveAbility.AddUniqueDynamic(this, &UItemListWidget::OnRemoveAbilityEvent);
		}
	}

	HiddenItem();
	HiddenAbility();
}

void UItemListWidget::OnClickedCloseButton()
{
	SetVisibility(ESlateVisibility::Collapsed);
}


void UItemListWidget::OnVisibilityChangedEvent(ESlateVisibility InVisibility)
{
#if !WITH_EDITOR
	if (IsVisible())
	{
		UE_LOGFMT(LogTemp, Log, "OnVisibilityChangedEvent");
		CreateItemList_All();
	}
#endif
}

void UItemListWidget::CreateItemList_All()
{
	ListView_Item->ClearListItems();
	for (auto iter : ListViewAbilities)
	{
		ListView_Item->AddItem(iter.Value);
	}

	for (auto iter : ListViewItems)
	{
		ListView_Item->AddItem(iter.Value);
	}

	ListView_Item->SetScrollbarVisibility(ESlateVisibility::Visible);
	ListView_Item->ScrollToTop();
}

void UItemListWidget::CreateItemList_Consume()
{
	ListView_Item->ClearListItems();
	for (auto iter : ListViewItems)
	{
		if (UItemHelperLibrary::IsConsume(iter.Value->InventoryItem))
		{
			ListView_Item->AddItem(iter.Value);
		}
	}

	ListView_Item->SetScrollbarVisibility(ESlateVisibility::Visible);
	ListView_Item->ScrollToTop();
}

void UItemListWidget::CreateItemList_Sprite()
{
	ListView_Item->ClearListItems();
	for (auto iter : ListViewItems)
	{
		if (UItemHelperLibrary::IsSprite(iter.Value->InventoryItem))
		{
			ListView_Item->AddItem(iter.Value);
		}
	}

	ListView_Item->SetScrollbarVisibility(ESlateVisibility::Visible);
	ListView_Item->ScrollToTop();
}

void UItemListWidget::CreateItemList_Armor()
{
	ListView_Item->ClearListItems();
	for (auto iter : ListViewItems)
	{
		if (UItemHelperLibrary::IsArmor(iter.Value->InventoryItem))
		{
			ListView_Item->AddItem(iter.Value);
		}
	}

	ListView_Item->SetScrollbarVisibility(ESlateVisibility::Visible);
	ListView_Item->ScrollToTop();
}

void UItemListWidget::CreateItemList_Ring()
{
	ListView_Item->ClearListItems();
	for (auto iter : ListViewItems)
	{
		if (UItemHelperLibrary::IsRing(iter.Value->InventoryItem))
		{
			ListView_Item->AddItem(iter.Value);
		}
	}

	ListView_Item->SetScrollbarVisibility(ESlateVisibility::Visible);
	ListView_Item->ScrollToTop();
}

void UItemListWidget::CreateItemList_Ability()
{
	UE_LOGFMT(LogTemp,Log,"CreateItemList_Ability");
	ListView_Item->ClearListItems();
	for (auto iter : ListViewAbilities)
	{
		UE_LOGFMT(LogTemp,Log,"어빌리티 리스트 생성 : {0}",iter.Key.ToString());
		ListView_Item->AddItem(iter.Value);
	}

	ListView_Item->SetScrollbarVisibility(ESlateVisibility::Visible);
	ListView_Item->ScrollToTop();
}

void UItemListWidget::CreateItemList_Enhancement()
{
	ListView_Item->ClearListItems();
	for (auto iter : ListViewItems)
	{
		if (UItemHelperLibrary::IsEnhancement(iter.Value->InventoryItem))
		{
			ListView_Item->AddItem(iter.Value);
		}
	}

	ListView_Item->SetScrollbarVisibility(ESlateVisibility::Visible);
	ListView_Item->ScrollToTop();
}

void UItemListWidget::CreateItemList_Key()
{
	ListView_Item->ClearListItems();
	for (auto iter : ListViewItems)
	{
		if (UItemHelperLibrary::IsKey(iter.Value->InventoryItem))
		{
			ListView_Item->AddItem(iter.Value);
		}
	}

	ListView_Item->SetScrollbarVisibility(ESlateVisibility::Visible);
	ListView_Item->ScrollToTop();
}

void UItemListWidget::CreateInventoryItemList()
{
	ListView_Item->ClearListItems();
	ListViewItems.Empty();
	ListViewAbilities.Empty();

	if (auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent())
	{
		for (auto iter : invenComp->GetInventory())
		{
			OnAddItemEvent(GetOwningPlayerPawn<ABaseCharacter>(), iter.Value, nullptr);
		}
	}

	if (auto abComp = GetOwningPlayerPawn<ABaseCharacter>()->GetAbilityComponent())
	{
		auto ab = abComp->GetAvailableAbilities();
		for (auto a : ab)
		{
			if (a->GetAbilityInformation().bRegisterInventory)
			{
				UE_LOGFMT(LogTemp,Log,"CreateInventoryItemList : {0}",a->GetAbilityInformation().AbilityTag.ToString());
				OnAddAbilityEvent(a->GetAbilityInformation());
			}
		}
	}
}

void UItemListWidget::HideWidgetIfNotSpirit(UUserWidget* Widget)
{
	if (auto button = Cast<UItemButtonWidget>(Widget))
	{
		if (auto info = Cast<UItemData>(button->GetInventoryData())->InventoryItem.GetItemInformation())
		{
			if (info->Item_Type == EItemType::EQUIPMENT)
			{
				if (static_cast<const FEquipmentInformation*>(info)->EquipType == EEquipType::SPIRIT)
				{
					return;
				}
			}
		}
	}

	Widget->SetVisibility(ESlateVisibility::Collapsed);
}

void UItemListWidget::HideWidgetIfNotArmor(UUserWidget* Widget)
{
	if (auto button = Cast<UItemButtonWidget>(Widget))
	{
		if (auto info = Cast<UItemData>(button->GetInventoryData())->InventoryItem.GetItemInformation())
		{
			if (info->Item_Type == EItemType::EQUIPMENT)
			{
				if (static_cast<const FEquipmentInformation*>(info)->EquipType == EEquipType::ARMOR)
				{
					return;
				}
			}
		}
	}

	Widget->SetVisibility(ESlateVisibility::Collapsed);
}

void UItemListWidget::RefreshFromInventoryItem(const FInventoryItem& Item)
{
	OnAddItemEvent(GetOwningPlayerPawn<ABaseCharacter>(), Item, nullptr);
}

void UItemListWidget::OnItemButtonHovered(UInventoryData* Data)
{
	if (ParentsUMG.IsValid())
	{
		switch (ItemListType)
		{
		case EItemListType::INVENTORY:
			Cast<UInventoryWidget>(ParentsUMG)->ShowItemInformation(Data);
			break;
		case EItemListType::EQUIPMENT:
			Cast<UEquipWidget>(ParentsUMG)->ShowItemInformation(Data);
			break;
		case EItemListType::MERCHANT:

			break;
		case EItemListType::ENHANCEMENT:
			Cast<UEnhancementWidget>(ParentsUMG)->ShowItemInformation(Data);
			break;
		}
	}
}

void UItemListWidget::RefreshItemButton(const FGuid& ItemUniqueID)
{
	if(ListViewItems.Contains(ItemUniqueID))
	{
		const auto& data = ListViewItems[ItemUniqueID];
		if(auto button = ListView_Item->GetEntryWidgetFromItem(data))
		{
			Cast<UItemButtonWidget>(button)->RefreshItemData(this);
		}
		
	}
}

void UItemListWidget::HiddenAbility() const
{
	for (auto iter : ListViewAbilities)
	{
		if (auto widget = ListView_Item->GetEntryWidgetFromItem(iter.Value))
		{
			widget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UItemListWidget::HiddenItem() const
{
	for (auto iter : ListViewItems)
	{
		if (auto widget = ListView_Item->GetEntryWidgetFromItem(iter.Value))
		{
			widget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UItemListWidget::OnAddItemEvent(class ABaseCharacter* UsedBy, const FInventoryItem& ItemInfo,
                                     AItemActor* ItemActor)
{
	UE_LOGFMT(LogTemp, Log, "인벤토리 위젯 업데이트");
	/*UKismetSystemLibrary::PrintString(this,StaticEnum<EItemListType>()->GetValueAsString(ItemListType));*/

	if (IsOrbItem(ItemInfo))
	{
		return;
	}

	//이미 있는경우
	if (IsAlreadyContain(ItemInfo))
	{
		OnInventoryUpdateEvent(ItemInfo.UniqueID, ItemInfo.ItemCount);
	}
	else
	{
		//처음 추가하는 경우
		if (const auto data = NewObject<UItemData>())
		{
			data->OnPlayerBuyItemFromNPC = OnPlayerBuyItemFromNPC;
			data->InventoryItem = ItemInfo;

			ListView_Item->AddItem(data);
			ListViewItems.Add(ItemInfo.UniqueID, data);

			ListView_Item->SetScrollbarVisibility(ESlateVisibility::Visible);
		}
	}
}

void UItemListWidget::OnAddAbilityItemEvent(ABaseCharacter* GetBy, const FAbilityInformation& AbilityInfo,
                                            AItemActor* ItemActor)
{
	UE_LOGFMT(LogTemp,Log,"어빌리티 아이템 추가 이벤트 호출 : {0}",AbilityInfo.AbilityName.ToString());
	if (!IsAlreadyContain(AbilityInfo))
	{
		OnAddAbilityEvent(AbilityInfo);
	}
}

void UItemListWidget::OnRemoveItemEvent(ABaseCharacter* Player, const FGuid& ItemUniqueID)
{
	if (ListViewItems.Contains(ItemUniqueID))
	{
		ListView_Item->RemoveItem(ListViewItems[ItemUniqueID]);
		ListView_Item->RequestRefresh();
	}
}

void UItemListWidget::OnInventoryUpdateEvent(const FGuid& ItemUniqueID, const int32& NewCount)
{
	if (ListViewItems.Contains(ItemUniqueID))
	{
		UE_LOGFMT(LogTemp, Log, "{0} {1}. 다음 아이템을 가진 버튼을 찾아 업데이트를 시도합니다 : {2}", __FUNCTION__, __LINE__,
		          ListViewItems[ItemUniqueID]->InventoryItem.GetItemInformation()->Item_Name.ToString());
		ListViewItems[ItemUniqueID]->InventoryItem.ItemCount = NewCount;
		if (auto button = ListView_Item->GetEntryWidgetFromItem<UItemButtonWidget>(ListViewItems[ItemUniqueID]))
		{
			button->RefreshItemData(this);
			ListView_Item->RequestRefresh();
		}
	}
}

void UItemListWidget::OnAddAbilityEvent(const FAbilityInformation& AbilityInformation)
{
	if (AbilityInformation.bRegisterInventory)
	{
		if (const auto data = NewObject<UAbilityData>())
		{
			UE_LOGFMT(LogTemp,Log,"인벤토리 위젯에 어빌리티 추가됨 : {0}",AbilityInformation.AbilityTag.ToString());
			data->AbilityInformation = AbilityInformation;

			ListView_Item->AddItem(data);
			ListViewAbilities.Add(AbilityInformation.AbilityTag, data);
		}
	}
}

void UItemListWidget::OnRemoveAbilityEvent(const FGameplayTag& AbilityTag)
{
	if (ListViewAbilities.Contains(AbilityTag))
	{
		ListView_Item->RemoveItem(ListViewAbilities[AbilityTag]);
		ListView_Item->RequestRefresh();
	}
}

bool UItemListWidget::IsAlreadyContain(const FInventoryItem& ItemInfo) const
{
	return ListViewItems.Contains(ItemInfo.UniqueID);
}

bool UItemListWidget::IsAlreadyContain(const FAbilityInformation& AbilityInfo) const
{
	return ListViewAbilities.Contains(AbilityInfo.AbilityTag);
}

void UItemListWidget::HiddenEquipment() const
{
	for (auto iter : ListViewItems)
	{
		if (auto info = iter.Value->InventoryItem.GetItemInformation())
		{
			if (info->Item_Type == EItemType::EQUIPMENT)
			{
				if (auto widget = ListView_Item->GetEntryWidgetFromItem(iter.Value))
				{
					widget->SetVisibility(ESlateVisibility::Collapsed);
				}
			}
		}
	}
}

void UItemListWidget::HiddenConsumeItem() const
{
	for (auto iter : ListViewItems)
	{
		if (iter.Value->InventoryItem.GetItemInformation()->Item_Type == EItemType::CONSUME)
		{
			if (auto widget = ListView_Item->GetEntryWidgetFromItem(iter.Value))
			{
				widget->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

bool UItemListWidget::IsOrbItem(const FInventoryItem& ItemInfo) const
{
	if (ItemInfo.GetItemInformation()->Item_Type == EItemType::EQUIPMENT)
	{
		if (static_cast<const FEquipmentInformation*>(ItemInfo.GetItemInformation())->EquipType == EEquipType::ORB)
		{
			return true;
		}
	}
	return false;
}

void UItemListWidget::QuickSlotSetting() const
{
	Button_Close->SetVisibility(ESlateVisibility::Collapsed);

	Button_All->SetIsEnabled(false);
	Button_Armor->SetIsEnabled(false);
	Button_Spirit->SetIsEnabled(false);
	Button_Ring->SetIsEnabled(false);

	HiddenEquipment();
}

void UItemListWidget::EquipSetting(UEquipWidget* ParentsWidget)
{
	ParentsUMG = ParentsWidget;

	ItemListType = EItemListType::EQUIPMENT;
	Button_Close->SetVisibility(ESlateVisibility::Collapsed);

	Button_All->SetIsEnabled(false);
	Button_Enhancement->SetIsEnabled(false);

	HiddenAbility();
	HiddenConsumeItem();
}

void UItemListWidget::MerchantSetting()
{
	ItemListType = EItemListType::MERCHANT;
	Button_Close->SetVisibility(ESlateVisibility::Collapsed);
	CreateItemList_All();
}

void UItemListWidget::InventorySetting(UInventoryWidget* ParentsWidget)
{
	ParentsUMG = ParentsWidget;
	ItemListType = EItemListType::INVENTORY;
	Button_Close->SetVisibility(ESlateVisibility::Collapsed);
	CreateItemList_All();
}

void UItemListWidget::EnhancementSetting(UEnhancementWidget* ParentsWidget)
{
	ParentsUMG = ParentsWidget;
	ItemListType = EItemListType::ENHANCEMENT;
	Button_Close->SetVisibility(ESlateVisibility::Collapsed);
}

bool UItemListWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                   UDragDropOperation* InOperation)
{
	if (auto oper = Cast<UDragAndDropOperation>(InOperation))
	{
		oper->DraggedWidget->SetRenderOpacity(oper->OriginalOpacity);
		oper->DraggedWidget->SetVisibility(ESlateVisibility::Visible);
		if (oper->DraggedWidget->IsA<UMerchantButtonWidget>())
		{
			auto button = Cast<UMerchantButtonWidget>(oper->DraggedWidget);
			if (auto data = button->GetMerchandiseData())
			{
				UE_LOGFMT(LogTemp, Log, "상점 아이템 버튼 드롭됨!!");
				if (data->IsA<UMerchandiseItemListData>())
				{
					OnPlayerBuyItemFromNPC.Broadcast(Cast<UMerchandiseItemListData>(data)->MerchandiseItem);
					return true;
				}

				if (data->IsA<UMerchandiseAbilityListData>())
				{
					OnPlayerBuyAbilityFromNPC.Broadcast(Cast<UMerchandiseAbilityListData>(data)->MerchandiseAbility);
					return true;
				}
			}
		}
	}

	return false;
}

void UItemListWidget::HideFilter()
{
	if (ScaleBox_Filter)
	{
		ScaleBox_Filter->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UItemListWidget::HideCloseButton()
{
	if (Button_Close)
	{
		Button_Close->SetVisibility(ESlateVisibility::Collapsed);
	}
}
