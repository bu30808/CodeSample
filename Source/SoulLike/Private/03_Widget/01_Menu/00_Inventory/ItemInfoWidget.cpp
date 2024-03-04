// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/00_Inventory/ItemInfoWidget.h"

#include "96_Library/ItemHelperLibrary.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"

void UItemInfoWidget::SetInfo(const FInventoryItem& Item)
{
	if(auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent())
	{
		Image_Item->SetBrushFromSoftTexture(Item.GetItemInformation()->Item_Image);
		TextBlock_ItemName->SetText(FText::FromString(Item.GetItemInformation()->Item_Name));
		TextBlock_ItemCount->SetText(FText::AsNumber(Item.ItemCount));
		TextBlock_ItemType->SetText(FText::FromString(UItemHelperLibrary::ItemTypeToString(Item.GetItemInformation())));
		RichTextBlock_ItemInfo->SetText(FText::FromString(Item.GetFormattedDescription()));
		RichTextBlock_ItemDetail->SetText(FText::FromString(UItemHelperLibrary::GetItemDetailString(Item,invenComp)));
	}
}
