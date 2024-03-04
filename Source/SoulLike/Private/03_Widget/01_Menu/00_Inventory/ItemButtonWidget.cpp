// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/00_Inventory/ItemButtonWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "03_Widget/SimpleToolTipWidget.h"
#include "03_Widget/01_Menu/00_Inventory/ItemListWidget.h"
#include "03_Widget/01_Menu/00_Inventory/ItemMenuWidget.h"
#include "03_Widget/04_Merchant/MerchantButtonWidget.h"
#include "04_Item/ItemActor.h"
#include "96_Library/ItemHelperLibrary.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

TWeakObjectPtr<UItemMenuWidget> UItemButtonWidget::ItemMenuWidget = nullptr;

void UItemButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OnVisibilityChanged.AddUniqueDynamic(this,&UItemButtonWidget::OnVisibilityChangedEvent);
}

void UItemButtonWidget::OnVisibilityChangedEvent(ESlateVisibility InVisibility)
{
	if(!IsVisible())
	{
		GetItemMenuWidget()->RemoveFromParent();
	}
}

void UItemButtonWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if (ListItemObject->IsA<UInventoryData>())
	{
		if (ListItemObject->IsA<UItemData>())
		{
			ProcessInventoryData(ListItemObject);
		}

		if (ListItemObject->IsA<UAbilityData>())
		{
			ProcessAbilityData(ListItemObject);
		}
	}
}

void UItemButtonWidget::OnUseItem()
{
	UE_LOGFMT(LogTemp, Log, "{0} {1}", __FUNCTION__, __LINE__);
	if (InventoryData.IsValid() && InventoryData->IsA<UItemData>())
	{
		if (Cast<UItemData>(InventoryData)->InventoryItem.GetItemInformation()->Item_Type == EItemType::CONSUME)
		{
			if (auto player = GetOwningPlayerPawn<ABaseCharacter>())
			{
				player->GetInventoryComponent()->UseItem(Cast<UItemData>(InventoryData)->InventoryItem.UniqueID);
			}
		}
	}
	OnCancel();
}

void UItemButtonWidget::OnRemoveItem()
{
	UE_LOGFMT(LogTemp, Log, "{0} {1}", __FUNCTION__, __LINE__);
	if (InventoryData.IsValid() && InventoryData->IsA<UItemData>())
	{
		if (auto player = GetOwningPlayerPawn<ABaseCharacter>())
		{
			player->GetInventoryComponent()->RemoveItem(Cast<UItemData>(InventoryData)->InventoryItem.UniqueID);
		}
	}
	OnCancel();
}

void UItemButtonWidget::OnCancel()
{
	UE_LOGFMT(LogTemp, Log, "{0} {1}", __FUNCTION__, __LINE__);

	if (auto toolTip = GetToolTip())
	{
		toolTip->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UItemButtonWidget::ShowMenu()
{
	if (const auto menu = GetItemMenuWidget())
	{
		float x, y;
		if (GetOwningPlayer()->GetMousePosition(x, y))
		{
			if (!menu->IsInViewport())
			{
				menu->AddToViewport();
			}

			//뷰포트를 넘어가는지 확인합니다.
			{
				FVector2D ViewportSize = FVector2D::ZeroVector;
				if (UGameViewportClient* GameViewport = GEngine->GameViewport)
				{
					GameViewport->GetViewportSize(ViewportSize);
				}

				auto menuSize = menu->GetDesiredSize();
				if (x + menuSize.X > ViewportSize.X)
				{
					x -= menuSize.X;
				}

				if (y + menuSize.Y > ViewportSize.Y)
				{
					y -= menuSize.Y;
				}

				//x,y 좌표를 좀 더 움직여서 메뉴에 마우스가 호버할 수 있도록 합니다.
				//이렇게 해야 메뉴에서 호버 해제됐을때 메뉴 없애버릴수 있음.
				x-=25.f;
				y-=25.f;
			}

			//툴팁이 떠 있다면 잠깐 없앱니다.
			if (auto toolTip = GetToolTip())
			{
				toolTip->SetVisibility(ESlateVisibility::Collapsed);
			}


			//메뉴버튼을 누르면 할 일들을 설정합니다.
			menu->OnClickedUseButton.AddUniqueDynamic(this, &UItemButtonWidget::OnUseItem);
			menu->OnClickedRemoveButton.AddUniqueDynamic(this, &UItemButtonWidget::OnRemoveItem);
			menu->OnClickedCancelButton.AddUniqueDynamic(this, &UItemButtonWidget::OnCancel);

			//메뉴버튼의 위치를 조정합니다.
			menu->SetPositionInViewport(FVector2d(x, y));
		}
	}
}

void UItemButtonWidget::OnClickedEvent()
{
	UGameplayStatics::PlaySound2D(this,ClickSound);
	Image_Background->SetBrushTintColor(PressedColor);
	
	if (InventoryData.IsValid() == false || InventoryData->IsA<UAbilityData>())
	{
		return;
	}

	if (Cast<UItemData>(InventoryData)->InventoryItem.GetItemInformation()->Item_Type != EItemType::CONSUME)
	{
		return;
	}

	if(OnPlayerBuyItemFromNPC.IsBound())
	{
		return;
	}

	if(ItemListType == EItemListType::INVENTORY)
	{
		ShowMenu();
	}
}

void UItemButtonWidget::ProcessInventoryData(UObject* ListItemObject)
{
	if (const auto data = Cast<UItemData>(ListItemObject))
	{
		InventoryData = data;
		InventoryData->OwnItemButtonWidget = this;
		
		OnPlayerBuyItemFromNPC = data->OnPlayerBuyItemFromNPC;

		if (const auto info = data->InventoryItem.GetItemInformation())
		{
			Image_Item->SetBrushFromSoftTexture(info->Item_Image);
			TextBlock_ItemName->SetText(FText::FromString(info->Item_Name));
			TextBlock_Count->SetText(FText::AsNumber(data->InventoryItem.ItemCount));

			if (GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent()->IsEquipped(
				data->InventoryItem.UniqueID))
			{
				SetEquipped(true);
			}
			else
			{
				SetEquipped(false);
			}
			
		}
	}
}

void UItemButtonWidget::ProcessAbilityData(UObject* ListItemObject)
{
	if (const auto data = Cast<UAbilityData>(ListItemObject))
	{
		InventoryData = data;
		InventoryData->OwnItemButtonWidget = this;

		const auto info = data->AbilityInformation;

		Image_Item->SetBrushFromSoftTexture(info.AbilityImage);
		TextBlock_ItemName->SetText(FText::FromString(info.AbilityName));
		TextBlock_Count->SetText(FText::AsNumber(1));

		UWidgetHelperLibrary::SetToolTipWidget(this, info.AbilityDescription);
	}
}

UItemMenuWidget* UItemButtonWidget::GetItemMenuWidget()
{
	if (ItemMenuWidget.IsValid() == false)
	{
		ItemMenuWidget = CreateWidget<UItemMenuWidget>(GetOwningPlayer(), ItemMenuWidgetObject);
	}
	
	ItemMenuWidget->ParentsWidget = this;
	
	return ItemMenuWidget.Get();
}

void UItemButtonWidget::RefreshItemData(class UItemListWidget* ParentsWidget)
{
	if(OnHovered.IsBound() == false)
	{
		OnHovered.AddUniqueDynamic(ParentsWidget,&UItemListWidget::OnItemButtonHovered);
	}

	ItemListType = ParentsWidget->GetItemListType();
	
	
	NativeOnListItemObjectSet(InventoryData.Get());
}

void UItemButtonWidget::SetEquipped(bool bEquip) const
{
	
	if (Image_Equipped)
	{
		if (bEquip)
		{
			Image_Equipped->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			Image_Equipped->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	else
	{
		UE_LOGFMT(LogTemp, Error, "버튼이 아직 유효하지 않은거 같네요.");
	}
}

bool UItemButtonWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
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

FReply UItemButtonWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	/*//좌클릭시 클릭 처리
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		OnClickedEvent();
		return FReply::Handled();
	}*/
	
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UItemButtonWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//좌클릭시 클릭 처리
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		OnClickedEvent();
	}
	
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UItemButtonWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnHoveredEvent();
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UItemButtonWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	OnUnhoveredEvent();
	Super::NativeOnMouseLeave(InMouseEvent);
}

void UItemButtonWidget::OnHoveredEvent()
{
	UGameplayStatics::PlaySound2D(this,HoverSound);
	Image_Background->SetBrushTintColor(HoverColor);
	
	if(InventoryData.IsValid()){

		OnHovered.Broadcast(InventoryData.Get());

		if(InventoryData->IsA<UItemData>())
		{
				UWidgetHelperLibrary::SetToolTipWidget(this, UItemHelperLibrary::GetItemToolTipString(
					Cast<UItemData>(InventoryData)->InventoryItem, GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent()));

		}
	}else
	{
		UKismetSystemLibrary::PrintString(this,TEXT("아이템 정보가 유효하지 않아 정보 출력이 불가능합니다."));
	}
}

void UItemButtonWidget::OnUnhoveredEvent()
{
	Image_Background->SetBrushTintColor(NormalColor);
	/*if(auto menu = GetItemMenuWidget())
	{
		if(menu->ParentsWidget == this)
		{
			if(menu->IsInViewport())
			{
				menu->RemoveFromParent();
			}
		}
	}*/
}
