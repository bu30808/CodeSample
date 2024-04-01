// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/00_Inventory/ItemInfoWidget.h"

#include "96_Library/ItemHelperLibrary.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Logging/StructuredLog.h"

void UItemInfoWidget::SetInfo(const FInventoryItem& Item)
{
	if (auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent())
	{
		UE_LOGFMT(LogTemp, Warning, "아이템 정보 출력");

		Image_Item->SetBrushFromSoftTexture(Item.GetItemInformation()->Item_Image);
		TextBlock_ItemName->SetText(Item.GetItemInformation()->Item_Name);
		TextBlock_ItemCount->SetText(FText::AsNumber(Item.ItemCount));
		TextBlock_ItemType->SetText(UItemHelperLibrary::ItemTypeToText(Item.GetItemInformation()));

		RichTextBlock_ItemInfo->SetText(Item.GetFormattedDescription());

		if (UItemHelperLibrary::IsOrbFragment(Item))
		{
			Image_Item->SetColorAndOpacity(UItemHelperLibrary::GetFragmentColorByType(Item));
			RichTextBlock_ItemDetail->SetText(UItemHelperLibrary::GetFragmentToolTipText(Item));
		}
		else
		{
			Image_Item->SetColorAndOpacity(FLinearColor::White);
			RichTextBlock_ItemDetail->SetText(UItemHelperLibrary::GetItemDetailText(Item, invenComp));
		}
	}
}
