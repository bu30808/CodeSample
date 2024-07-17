// Fill out your copyright notice in the Description page of Project Settings.
#include "03_Widget/04_Merchant/MerchantButtonWidget.h"

#include "03_Widget/SimpleToolTipWidget.h"
#include "03_Widget/01_Menu/00_Inventory/ItemButtonWidget.h"
#include "03_Widget/04_Merchant/MerchantListWidget.h"
#include "03_Widget/04_Merchant/MerchantWidget.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Logging/StructuredLog.h"

#define LOCTEXT_NAMESPACE "MerchantButtonWidget"

namespace GlobalMerchantButtonWidgetText
{
	const FText infiniteText = NSLOCTEXT("MerchantButtonWidget", "InfiniteText", "무제한");
	const FText sellText = NSLOCTEXT("MerchantButtonWidget", "SellText", "판매");
}
#undef LOCTEXT_NAMESPACE

void UMerchantButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (ToolTipWidgetObject)
	{
		SetToolTip(CreateWidget(GetOwningPlayer(), ToolTipWidgetObject));
	}
}

void UMerchantButtonWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if (ListItemObject->IsA<UMerchandiseItemListData>())
	{
		if (auto merchandiseItemListData = Cast<UMerchandiseItemListData>(ListItemObject))
		{
			Data = Cast<UMerchandiseItemListData>(ListItemObject);
			if (const auto itemData = merchandiseItemListData->MerchandiseItem.GetItemInformation())
			{
				if (itemData->Item_Image)
				{
					Image_Item->SetBrushFromSoftTexture(
						itemData->Item_Image);
				}else
				{
					UE_LOGFMT(LogTemp,Error,"{0} 아이템 이미지를 가져올 수 없습니다!!",itemData->Item_Name.ToString());
				}

				TextBlock_ItemName->SetText(itemData->Item_Name);

				if (merchandiseItemListData->MerchandiseItem.MerchandiseData.bSellInfinite)
				{
					TextBlock_Count->SetText(GlobalMerchantButtonWidgetText::infiniteText);
				}
				else
				{
					TextBlock_Count->SetText(
						FText::AsNumber(merchandiseItemListData->MerchandiseItem.MerchandiseData.Count));
				}

				TextBlock_Price->SetText(
					FText::AsNumber(merchandiseItemListData->MerchandiseItem.MerchandiseData.Price));

				UWidgetHelperLibrary::SetToolTipWidget(
					this, merchandiseItemListData->MerchandiseItem.GetFormattedDescription());
			}
		}
	}

	if (ListItemObject->IsA<UMerchandiseAbilityListData>())
	{
		Data = Cast<UMerchandiseAbilityListData>(ListItemObject);
		if (auto merchandiseAbilityListData = Cast<UMerchandiseAbilityListData>(Data))
		{
			if(const auto abilityData = merchandiseAbilityListData->MerchandiseAbility.GetAbilityInformation()){
				
				Image_Item->SetBrushFromSoftTexture(abilityData->AbilityImage);
				TextBlock_ItemName->SetText(abilityData->AbilityName);

				TextBlock_Count->SetText(
					FText::AsNumber(merchandiseAbilityListData->MerchandiseAbility.MerchandiseAbilityData.Count));
				TextBlock_Price->SetText(
					FText::AsNumber(merchandiseAbilityListData->MerchandiseAbility.MerchandiseAbilityData.Price));

				
				const auto& description = FText::Format(FText::FromString("{0}\n\n{1}"),merchandiseAbilityListData->MerchandiseAbility.GetAbilityDescription(),merchandiseAbilityListData->MerchandiseAbility.GetAbilityReqDescription());
				Cast<USimpleToolTipWidget>(GetToolTip())->SetDescriptionText(description);
				
			}
		}
	}
}

bool UMerchantButtonWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                         UDragDropOperation* InOperation)
{
	if (auto oper = Cast<UDragAndDropOperation>(InOperation))
	{
		oper->DraggedWidget->SetRenderOpacity(oper->OriginalOpacity);
		oper->DraggedWidget->SetVisibility(ESlateVisibility::Visible);
		if (oper->DraggedWidget->IsA<UItemButtonWidget>())
		{
			if (Data->MerchantListWidget.IsValid())
			{
				if (Data->MerchantListWidget->ShopType == EShopType::Purchase)
				{
					const auto button = Cast<UItemButtonWidget>(oper->DraggedWidget);
					const auto data = button->GetInventoryData();

					return Data->MerchantListWidget->PlayerSellItemOrAbility(data);
				}
			}
		}
	}
	return false;
}

void UMerchantButtonWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	Image_Background->SetBrushTintColor(HoveredColor);
}

void UMerchantButtonWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	Image_Background->SetBrushTintColor(NormalColor);
}
