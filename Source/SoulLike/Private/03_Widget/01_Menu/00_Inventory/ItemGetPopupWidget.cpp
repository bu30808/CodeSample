// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/00_Inventory/ItemGetPopupWidget.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "Components/ScrollBox.h"
#include "03_Widget/01_Menu/00_Inventory/ItemGetElementWidget.h"

void UItemGetPopupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ScrollBox->ClearChildren();

	if (auto player = GetOwningPlayerPawn<ABaseCharacter>())
	{
		player->GetInventoryComponent()->OnGetItem.AddUniqueDynamic(this, &UItemGetPopupWidget::OnGetItem);
	}
}

void UItemGetPopupWidget::OnGetItem(TSoftObjectPtr<UTexture2D> Image, FText Name, int32 Count)
{
	if (ItemGetElementObject)
	{
		if (auto widget = CreateWidget<UItemGetElementWidget>(GetOwningPlayer(), ItemGetElementObject))
		{
			widget->SetData(Image, Name, Count);
			ScrollBox->AddChild(widget);
			ScrollBox->ScrollToEnd();
		}
	}
}
