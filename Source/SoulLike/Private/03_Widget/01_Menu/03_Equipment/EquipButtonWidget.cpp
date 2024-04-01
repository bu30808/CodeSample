// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/03_Equipment/EquipButtonWidget.h"

#include "03_Widget/01_Menu/00_Inventory/ItemButtonWidget.h"
#include "03_Widget/05_Alert/AlertWidget.h"
#include "03_Widget/99_Drag/DragAndDropOperation.h"
#include "03_Widget/99_Drag/DraggableWidget.h"
#include "04_Item/ItemActor.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "Blueprint/DragDropOperation.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Logging/StructuredLog.h"
#include "00_Character/00_Player/00_Controller/UserController.h"
#include "03_Widget/01_Menu/03_Equipment/EquipWidget.h"
#include "03_Widget/MainWidget.h"
#include "96_Library/ItemHelperLibrary.h"
#include "Kismet/GameplayStatics.h"
#define LOCTEXT_NAMESPACE "EquipButtonWidget"

namespace GlobalEquipButtonWidgetText
{
	static const FText emptyText = NSLOCTEXT("EquipButtonWidget", "SlotUnlockText", "비어있음");
	static const FText cannotUnEquipText = NSLOCTEXT("EquipButtonWidget", "CannotUnEquipText", "무기는 장착 해제할 수 없습니다.");
}
#undef LOCTEXT_NAMESPACE

void UEquipButtonWidget::Clean()
{
	ItemData = nullptr;
	TextBlock_Name->SetText(GlobalEquipButtonWidgetText::emptyText);
	Image->SetBrushFromSoftTexture(DefaultImage);
	OnUnEquipped.Clear();
}

void UEquipButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (Image)
	{
		if (IconMap.Contains(EquipSlotType))
		{
			Image->SetBrushFromSoftTexture(IconMap[EquipSlotType]);
		}
	}
}

void UEquipButtonWidget::SetButtonInfo(TWeakObjectPtr<UItemData> NewData)
{
	if (const auto info = NewData->InventoryItem.GetItemInformation())
	{
		if (IsVisible() == false)
		{
			SetVisibility(ESlateVisibility::Visible);
		}

		ItemData = NewData;

		auto itemName = info->Item_Name.ToString();
		if (itemName.Len() > 10)
		{
			itemName = itemName.Left(10);
			itemName += "...";
		}
		TextBlock_Name->SetText(FText::FromString(itemName));
		Image->SetBrushFromSoftTexture(info->Item_Image);

		/*
		auto tooltip = ItemData->InventoryItem.GetFormattedDescription() + "\n\n" +
			UItemHelperLibrary::GetItemDetailString(ItemData->InventoryItem,
			                                        GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent());
		UWidgetHelperLibrary::SetToolTipWidget(this, tooltip);*/
	}
}

bool UEquipButtonWidget::IsCorrectType(UInventoryData* InventoryData) const
{
	if (const auto data = Cast<UItemData>(InventoryData))
	{
		if (const auto info = data->InventoryItem.GetItemInformation())
		{
			if (info->Item_Type == EItemType::EQUIPMENT)
			{
				auto equipType = static_cast<const FEquipmentInformation*>(info)->EquipType;
				switch (equipType)
				{
				case EEquipType::NONE:
					break;
				case EEquipType::SPIRIT:
					return EquipSlotType == EEquipSlotType::SPIRIT;
				case EEquipType::ARMOR:
					return EquipSlotType == EEquipSlotType::ARMOR;
				case EEquipType::RING:
					return EquipSlotType == EEquipSlotType::RING;
				default: ;
				}
			}
		}
	}

	return false;
}

bool UEquipButtonWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                      UDragDropOperation* InOperation)
{
	if (EquipButtonType != EEquipButtonType::EQUIPMENT)
	{
		return false;
	}


	if (auto oper = Cast<UDragAndDropOperation>(InOperation))
	{
		//인벤토리 버튼이 드롭되었을때만 처리합니다.
		if (oper->DraggedWidget.IsValid() && oper->DraggedWidget->IsA<UItemButtonWidget>())
		{
			oper->DraggedWidget->SetRenderOpacity(oper->OriginalOpacity);
			oper->DraggedWidget->SetVisibility(ESlateVisibility::Visible);
			if (const auto inventoryButton = Cast<UItemButtonWidget>(oper->DraggedWidget))
			{
				const auto data = inventoryButton->GetInventoryData();
				//이 버튼에 등록되어야 할 타입과 맞는지 확인합니다.
				if (IsCorrectType(data))
				{
					//이전 장비를 해제합니다.
					if (ItemData.IsValid())
					{
						//같은 장비가 드롭다운됐다면 아무것도 안 합니다.
						auto preID = ItemData->InventoryItem.UniqueID;
						if (preID == Cast<UItemData>(data)->InventoryItem.UniqueID)
						{
							return false;
						}

						//이 부분에서 다시 장착 이미지가 설정되는 문제가 있습니다.
						if (auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent())
						{
							invenComp->UnEquip(preID);
						}

						UE_LOGFMT(LogTemp, Error, "장비버튼 {0}이 드롭되었습니다.", inventoryButton->GetName());
						//다른 장비 슬롯에 장착된 같은 아이디의 아이템이 있다면, 슬롯을 비웁니다.
						if (ParentWidget.IsValid())
						{
							int32 outIndex;
							if (auto button = ParentWidget->GetRingSlotByUniqueID(
								Cast<UItemData>(data)->InventoryItem.UniqueID, outIndex))
							{
								UE_LOGFMT(LogTemp, Error, "장비버튼 {0}를 비웁니다.", button->GetName());
								button->OnUnEquipped.Broadcast();
								button->Clean();
							}
						}

						//이전 아이템 버튼의 장착 표시를 끕니다.
						if (ItemData->OwnItemButtonWidget.IsValid())
						{
							ItemData->OwnItemButtonWidget->SetEquipped(false);
						}
					}

					//이 버튼의 정보를 설정합니다.
					SetButtonInfo(Cast<UItemData>(data));
					UGameplayStatics::PlaySound2D(this, EquipSound);
					//장비 아이템을 사용 합니다.
					GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent()->UseItem(
						Cast<UItemData>(data)->InventoryItem.UniqueID);

					UE_LOGFMT(LogTemp, Error, "장비버튼 {0}에 장착 아이콘을 켭니다.", inventoryButton->GetName());
					inventoryButton->SetEquipped(true);

					return true;
				}
			}
		}
	}

	return false;
}

void UEquipButtonWidget::UnEquip()
{
	if (ItemData.IsValid() == false)
	{
		return;
	}

	if (const auto data = Cast<UItemData>(ItemData))
	{
		if (const auto info = data->InventoryItem.GetItemInformation())
		{
			//무기는 우클릭으로 장비해제 할 수 없음.
			if (info->Item_Type == EItemType::EQUIPMENT)
			{
				if (static_cast<const FEquipmentInformation*>(info)->
					EquipType != EEquipType::SPIRIT)
				{
					GetOwningPlayerPawn<APlayerCharacter>()->GetInventoryComponent()->UnEquip(
						data->InventoryItem.UniqueID);


					Clean();
				}
				else
				{
					UWidgetHelperLibrary::ShowAlertMsg(GetOwningPlayer<AUserController>(), EAlertMsgType::Normal,
					                                   GlobalEquipButtonWidgetText::cannotUnEquipText,
					                                   FOnButtonClicked());
				}
			}
		}
	}
}

FReply UEquipButtonWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (EquipButtonType != EEquipButtonType::EQUIPMENT)
	{
		return FReply::Unhandled();
	}

	//우클릭시 이 퀵슬롯 초기화.
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		OnUnEquipped.Broadcast();
		//부여된 장비 해제
		UnEquip();
	}


	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UEquipButtonWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (EquipButtonType != EEquipButtonType::EQUIPMENT)
	{
		return FReply::Unhandled();
	}

	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UEquipButtonWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (EquipButtonType == EEquipButtonType::EQUIPMENT)
	{
		if (ParentWidget.IsValid())
		{
			ParentWidget->ShowItemInformation(ItemData.Get());
		}
	}
	else
	{
		if (ItemData.IsValid())
		{
			FString tooltip = ItemData->InventoryItem.GetFormattedDescription().ToString() + "\n\n" +
				UItemHelperLibrary::GetItemDetailText(ItemData->InventoryItem,
				                                      GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent()).
				ToString();
			UWidgetHelperLibrary::SetToolTipWidget(this, FText::FromString(tooltip));
		}
	}

	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UEquipButtonWidget::SetEquipSlotType(EEquipSlotType NewType)
{
	EquipSlotType = NewType;
	if (Image)
	{
		if (IconMap.Contains(EquipSlotType))
		{
			Image->SetBrushFromSoftTexture(IconMap[EquipSlotType]);
		}
	}
}

void UEquipButtonWidget::SetEquipButtonType(EEquipButtonType NewType)
{
	EquipButtonType = NewType;
}

FGuid UEquipButtonWidget::GetEquippedID() const
{
	if (ItemData.IsValid())
	{
		return ItemData->InventoryItem.UniqueID;
	}

	return FGuid();
}

void UEquipButtonWidget::SetParentsWidget(UEquipWidget* PWidget)
{
	ParentWidget = PWidget;
}
