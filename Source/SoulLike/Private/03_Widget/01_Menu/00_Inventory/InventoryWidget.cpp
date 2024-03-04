// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/00_Inventory/InventoryWidget.h"

#include "03_Widget/01_Menu/00_Inventory/ItemInfoWidget.h"
#include "03_Widget/01_Menu/00_Inventory/ItemListWidget.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "Components/Button.h"


void UInventoryWidget::NativeConstruct()
{
	bUseAsPopUp = true;
	bRemovable = false;
	
	Super::NativeConstruct();

	if(Button_Close)
	{
		Button_Close->OnClicked.AddUniqueDynamic(this,&UInventoryWidget::OnClickedCloseButton);
	}

	if(UMG_ItemList)
	{
		UMG_ItemList->InventorySetting(this);
	}
	
}

void UInventoryWidget::OnClickedCloseButton()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UInventoryWidget::ShowItemInformation(UInventoryData* Data)
{
	if(UMG_ItemInfo)
	{
		if(auto itemData = Cast<UItemData>(Data))
		{
			UMG_ItemInfo->SetInfo(itemData->InventoryItem);
		}
	}
}
