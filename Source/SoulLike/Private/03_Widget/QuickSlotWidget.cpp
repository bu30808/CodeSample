// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/QuickSlotWidget.h"

#include "03_Widget/01_Menu/02_QuickSlot/QuickSlotButtonWidget.h"
#include "Components/HorizontalBox.h"

void UQuickSlotWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
}

void UQuickSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent())
	{
		/*invenComp->OnUpdateMainAbilityQuickSlot.AddUniqueDynamic(
			this, &UQuickSlotWidget::OnUpdateCurAbilityQuickSlotEvent);
		invenComp->OnUpdateMainConsumeQuickSlot.AddUniqueDynamic(
			this, &UQuickSlotWidget::OnUpdateCurConsumeQuickSlotEvent);*/
	}
}
