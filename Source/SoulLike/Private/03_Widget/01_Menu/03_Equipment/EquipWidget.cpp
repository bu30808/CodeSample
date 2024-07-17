// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/03_Equipment/EquipWidget.h"


#include "03_Widget/01_Menu/00_Inventory/ItemInfoWidget.h"
#include "03_Widget/01_Menu/00_Inventory/ItemListWidget.h"
#include "03_Widget/01_Menu/02_QuickSlot/QuickSlotButtonWidget.h"
#include "03_Widget/01_Menu/03_Equipment/EquipButtonWidget.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "Components/HorizontalBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/VerticalBox.h"
#include "Logging/StructuredLog.h"
#include "SoulLike/SoulLike.h"

void UEquipWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (VerticalBox)
	{
		for (auto i = 0; i < VerticalBox->GetChildrenCount(); i++)
		{
			if (auto child = VerticalBox->GetChildAt(i))
			{
				if (auto equipButtonWidget = Cast<UEquipButtonWidget>(child))
				{
					equipButtonWidget->SetSlotIndex(i);
				}
			}
		}
	}

	for (auto i = 0; i < UniformGridPanel_Consume->GetChildrenCount(); i++)
	{
		if (auto widget = Cast<UItemQuickSlotButtonWidget>(UniformGridPanel_Consume->GetChildAt(i)))
		{
			widget->OnRemoveAlreadyRegisteredSlotItem.BindUObject(this, &UEquipWidget::OnRemoveAlreadyRegisteredSlotItemEvent);
			widget->SetIndex(i);
		}
	}
	

	if (VerticalBox)
	{
		for (auto i = 0; i < VerticalBox->GetChildrenCount(); i++)
		{
			if (auto child = VerticalBox->GetChildAt(i))
			{
				if (auto equipButtonWidget = Cast<UEquipButtonWidget>(child))
				{
					equipButtonWidget->SetSlotIndex(i);
					AllEquipSlots.Add(equipButtonWidget);
				}
			}
		}
	}

	if (HorizontalBox_Ring1)
	{
		auto startIndex = AllEquipSlots.Num();
		for (auto i = 0; i < HorizontalBox_Ring1->GetChildrenCount(); i++)
		{
			if (auto child = HorizontalBox_Ring1->GetChildAt(i))
			{
				if (auto equipButtonWidget = Cast<UEquipButtonWidget>(child))
				{
					equipButtonWidget->SetSlotIndex(i + startIndex);
					AllEquipSlots.Add(equipButtonWidget);
					RingSlots.Add(equipButtonWidget);
				}
			}
		}
	}

	if (HorizontalBox_Ring2)
	{
		auto startIndex = AllEquipSlots.Num() - 1;
		for (auto i = 0; i < HorizontalBox_Ring2->GetChildrenCount(); i++)
		{
			if (auto child = HorizontalBox_Ring2->GetChildAt(i))
			{
				if (auto equipButtonWidget = Cast<UEquipButtonWidget>(child))
				{
					equipButtonWidget->SetSlotIndex(i + startIndex);
					AllEquipSlots.Add(equipButtonWidget);
					RingSlots.Add(equipButtonWidget);
				}
			}
		}
	}

	for (auto iter : AllEquipSlots)
	{
		iter->SetParentsWidget(this);
	}
}

void UEquipWidget::NativeConstruct()
{
	bUseAsPopUp = true;
	bRemovable = false;
	Super::NativeConstruct();
	if (UMG_ItemList)
	{
		UMG_ItemList->EquipSetting(this);
	}
}


void UEquipWidget::OnVisibilityChangedEvent(ESlateVisibility InVisibility)
{
	Super::OnVisibilityChangedEvent(InVisibility);

	if (IsVisible())
	{
		UMG_ItemList->CreateInventoryItemList();
		UMG_ItemList->CreateItemList_Sprite();
	}
}

void UEquipWidget::AddRingSlot()
{
	if (auto widget = HorizontalBox_Ring2->GetChildAt(0))
	{
		if (!widget->IsVisible())
		{
			widget->SetVisibility(ESlateVisibility::Visible);
			return;
		}
	}

	if (auto widget = HorizontalBox_Ring2->GetChildAt(1))
	{
		if (!widget->IsVisible())
		{
			widget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UEquipWidget::RemoveRingSlot()
{
	if (auto widget = HorizontalBox_Ring2->GetChildAt(0))
	{
		if (widget->IsVisible())
		{
			widget->SetVisibility(ESlateVisibility::Collapsed);
			Cast<UEquipButtonWidget>(widget)->UnEquip();
			return;
		}
	}

	if (auto widget = HorizontalBox_Ring2->GetChildAt(1))
	{
		if (widget->IsVisible())
		{
			widget->SetVisibility(ESlateVisibility::Collapsed);
			Cast<UEquipButtonWidget>(widget)->UnEquip();
		}
	}

}

int32 UEquipWidget::FindIndexByID(FGuid UniqueID)
{
	for (auto iter : AllEquipSlots)
	{
		if (iter->GetEquippedID() == UniqueID)
		{
			return iter->GetIndex();
		}
	}


	return INDEX_NONE;
}

UEquipButtonWidget* UEquipWidget::GetEquipButtonByIndex(int32 FindIndex)
{
	if (AllEquipSlots.IsValidIndex(FindIndex))
	{
		return AllEquipSlots[FindIndex];
	}

	return nullptr;
}

void UEquipWidget::ShowItemInformation(UInventoryData* Data)
{
	if (UMG_ItemInfo && Data != nullptr)
	{
		if (auto itemData = Cast<UItemData>(Data))
		{
			UMG_ItemInfo->SetInfo(itemData->InventoryItem);
		}
	}
}

void UEquipWidget::ForceUpdateSpirit(const FInventoryItem& ItemData)
{
	if (auto itemData = NewObject<UItemData>())
	{
		itemData->InventoryItem = ItemData;
		UMG_EquipSpirit->SetButtonInfo(itemData);
	}
}

UEquipButtonWidget* UEquipWidget::GetRingSlotByUniqueID(const FGuid& UniqueID, int32& OutIndex)
{
	OutIndex = INDEX_NONE;
	for (auto iter : RingSlots)
	{
		if (iter->GetEquippedID() == UniqueID)
		{
			OutIndex = iter->GetIndex();
			return iter;
		}
	}

	return nullptr;
}

TArray<UWidget*> UEquipWidget::GetAllConsumeQuickSlots()
{
	return UniformGridPanel_Consume->GetAllChildren();
}

void UEquipWidget::LoadItemQuickSlots(const TMap<int32, FGuid>& QuickSlotItemSavedMap)
{
	UE_LOGFMT(LogUMG,Log,"{0} {1} 퀵슬롯 정보를 불러와 복구합니다.",__FUNCTION__,__LINE__);
	for (auto iter : QuickSlotItemSavedMap)
	{
		UE_LOGFMT(LogUMG,Log,"{0} {1} 읽어온 아이템 : {2} 슬롯, {3}",__FUNCTION__,__LINE__,iter.Value,iter.Key);
		const auto& index = iter.Key;
		const auto& itemID = iter.Value;
		if(auto button = Cast<UItemQuickSlotButtonWidget>(UniformGridPanel_Consume->GetChildAt(index)))
		{
			button->RestoreSlotFromUniqueID(itemID);
		}
	}
}

int32 UEquipWidget::OnRemoveAlreadyRegisteredSlotItemEvent(UItemData* Data)
{
	UE_LOGFMT(LogUMG,Log,"이미 다른 퀵슬롯에 등록된 아이템 정보를 찾아 제거합니다.");
	if (Data->IsValidLowLevel())
	{
		auto c = UniformGridPanel_Consume->GetAllChildren();
		for (auto w : c)
		{
			if (auto button = Cast<UItemQuickSlotButtonWidget>(w))
			{
				if(auto otherButtonData = button->GetSlotData<UItemData>())
				{
					if(otherButtonData->InventoryItem.UniqueID == Data->InventoryItem.UniqueID)
					{
						UE_LOGFMT(LogUMG,Log,"발견");
						button->ClearSlot();
						return button->GetIndex();
					}
				}
			}
		}
	}

	return -1;
}

