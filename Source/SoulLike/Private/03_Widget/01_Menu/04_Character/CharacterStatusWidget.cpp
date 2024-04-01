// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/04_Character/CharacterStatusWidget.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "02_Ability/AbilityEffect.h"
#include "03_Widget/01_Menu/03_Equipment/EquipButtonWidget.h"
#include "03_Widget/01_Menu/04_Character/CharacterInfoElementWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"

void UCharacterStatusWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (InfoElementWidgetObject != nullptr)
	{
		VerticalBox_CharacterElement->ClearChildren();

		//레벨, 경험치정보를 최상단으로
		for (auto i = static_cast<int>(EAttributeType::Level); i <= static_cast<int>(EAttributeType::EXP); i++)
		{
			if (auto widget = CreateWidget<UCharacterInfoElementWidget>(GetOwningPlayer(), InfoElementWidgetObject))
			{
				widget->Init(static_cast<EAttributeType>(i));
				VerticalBox_CharacterElement->AddChildToVerticalBox(widget);
				Elements.Add(widget);
			}
		}

		//나머지 정보
		for (auto i = 0; i < static_cast<int>(EAttributeType::Level); i++)
		{
			if (auto widget = CreateWidget<UCharacterInfoElementWidget>(GetOwningPlayer(), InfoElementWidgetObject))
			{
				if (static_cast<EAttributeType>(i) == EAttributeType::HP || static_cast<EAttributeType>(i) ==
					EAttributeType::SP || static_cast<EAttributeType>(i) == EAttributeType::MP)
				{
					continue;
				}

				widget->Init(static_cast<EAttributeType>(i));
				VerticalBox_CharacterElement->AddChildToVerticalBox(widget);
				Elements.Add(widget);
			}
		}
	}
}

void UCharacterStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (auto player = GetOwningPlayerPawn<ABaseCharacter>())
	{
		player->GetInventoryComponent()->OnUseItem.AddUniqueDynamic(
			this, &UCharacterStatusWidget::OnUseItemEvent);

		player->GetAttributeComponent()->OnCharacterInformationUpdate.AddUniqueDynamic(
			this, &UCharacterStatusWidget::OnCharacterInformationUpdateEvent);
	}
}

void UCharacterStatusWidget::OnUseItemEvent(ABaseCharacter* UsedBy, const FInventoryItem& ItemInfo)
{
	for (auto e : Elements)
	{
		e->UpdateAttributeValue();
	}
}

void UCharacterStatusWidget::OnCharacterInformationUpdateEvent()
{
	for (auto e : Elements)
	{
		e->UpdateAttributeValue();
	}
}
