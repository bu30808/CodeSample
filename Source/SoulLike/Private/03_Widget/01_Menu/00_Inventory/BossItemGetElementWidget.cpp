// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/00_Inventory/BossItemGetElementWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UBossItemGetElementWidget::SetItemInformation(const FItemInformation* ItemInformation, const int32& ItemCount)
{
	if(ItemInformation)
	{
		Image_Item->SetBrushFromSoftTexture(ItemInformation->Item_Image);
		TextBlock_ItemName->SetText(ItemInformation->Item_Name);
		TextBlock_ItemCount->SetText(FText::AsNumber(ItemCount));
	}
}
