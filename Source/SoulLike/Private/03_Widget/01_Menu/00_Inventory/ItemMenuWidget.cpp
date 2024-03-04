// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/00_Inventory/ItemMenuWidget.h"

void UItemMenuWidget::OnClickedUseButtonEvent()
{
	OnClickedUseButton.Broadcast();
	RemoveFromParent();
}

void UItemMenuWidget::OnClickedRemoveButtonEvent()
{
	OnClickedRemoveButton.Broadcast();
	RemoveFromParent();
}

void UItemMenuWidget::OnClickedCancelButtonEvent()
{
	OnClickedCancelButton.Broadcast();
	RemoveFromParent();
}

void UItemMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Button_Use->OnClicked.AddUniqueDynamic(this, &UItemMenuWidget::OnClickedUseButtonEvent);
	Button_Remove->OnClicked.AddUniqueDynamic(this, &UItemMenuWidget::OnClickedRemoveButtonEvent);
	Button_Cancel->OnClicked.AddUniqueDynamic(this, &UItemMenuWidget::OnClickedCancelButtonEvent);
}

void UItemMenuWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	RemoveFromParent();
}
