// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/04_Character/CharacterTotalInfoWidget.h"

#include "03_Widget/01_Menu/03_Equipment/EquipButtonWidget.h"
#include "03_Widget/01_Menu/03_Equipment/EquipWidget.h"
#include "96_Library/ItemHelperLibrary.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Logging/StructuredLog.h"

void UCharacterTotalInfoWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (VerticalBox_EquipInfo)
	{
		EquipmentButtons = VerticalBox_EquipInfo->GetAllChildren();
	}
}

void UCharacterTotalInfoWidget::NativeConstruct()
{
	UE_LOGFMT(LogTemp,Log,"팝업위젯/// 컨스트럭트");
	
	bUseAsPopUp = true;
	bRemovable = false;
	
	Super::NativeConstruct();

	if (auto pawn = GetOwningPlayerPawn<APlayerCharacter>())
	{
		if (auto invenComp = pawn->GetInventoryComponent())
		{
			invenComp->OnUseItem.AddUniqueDynamic(this, &UCharacterTotalInfoWidget::OnEquippedItemEvent);
		}
	}
	
}

void UCharacterTotalInfoWidget::OnEquippedItemEvent(ABaseCharacter* UsedBy, const FInventoryItem& ItemInfo)
{
	if (UItemHelperLibrary::IsEquipment(ItemInfo))
	{
		if (auto data = NewObject<UItemData>())
		{
			data->InventoryItem = ItemInfo;

			if (UItemHelperLibrary::IsOrbCore(ItemInfo))
			{
				Cast<UEquipButtonWidget>(EquipmentButtons[0])->SetButtonInfo(data);
				return;
			}

			//장비창 정보를 가져옵니다.
			//장비창에는 코어슬롯이 없기 때문에 가져온 인덱스에서 +1 합니다.
			if (EquipmentWidget.IsValid())
			{
				if (UItemHelperLibrary::IsSprite(ItemInfo))
				{
					Cast<UEquipButtonWidget>(EquipmentButtons[1])->SetButtonInfo(data);
					return;
				}

				if (UItemHelperLibrary::IsArmor(ItemInfo))
				{
					Cast<UEquipButtonWidget>(EquipmentButtons[2])->SetButtonInfo(data);
					return;
				}

				if (UItemHelperLibrary::IsRing(ItemInfo))
				{
					int32 index;
					//장신구의 경우
					if (auto slot = EquipmentWidget->GetRingSlotByUniqueID(ItemInfo.UniqueID, index))
					{
						Cast<UEquipButtonWidget>(EquipmentButtons[index + 2])->SetButtonInfo(data);
					}
					return;
				}
			}
		}
	}
}

void UCharacterTotalInfoWidget::AddRingSlot()
{
	if (!EquipmentButtons[5]->IsVisible())
	{
		EquipmentButtons[5]->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	if (!EquipmentButtons[6]->IsVisible())
	{
		EquipmentButtons[6]->SetVisibility(ESlateVisibility::Visible);
		return;
	}
}

void UCharacterTotalInfoWidget::RemoveRingSlot()
{
	if (EquipmentButtons[6]->IsVisible())
	{
		EquipmentButtons[6]->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	if (EquipmentButtons[5]->IsVisible())
	{
		EquipmentButtons[5]->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
}

void UCharacterTotalInfoWidget::OnVisibilityChangedEvent(ESlateVisibility InVisibility)
{
	UE_LOGFMT(LogTemp,Log,"팝업위젯/// OnVisibilityChangedEvent");
	Super::OnVisibilityChangedEvent(InVisibility);
	
	if (auto pawn = GetOwningPlayerPawn<APlayerCharacter>())
	{
		pawn->ShowRender(IsVisible());
		
		if (IsVisible())
		{
			if (auto invenComp = pawn->GetInventoryComponent())
			{
				for (auto iter : EquipmentButtons)
				{
					const auto& equipID = Cast<UEquipButtonWidget>(iter)->GetEquippedID();
					if (equipID != FGuid())
					{
						auto newData = NewObject<UItemData>();
						newData->InventoryItem = invenComp->GetInventoryItem(equipID);
						Cast<UEquipButtonWidget>(iter)->SetButtonInfo(newData);
					}
				}
			}
		}
	}
}
