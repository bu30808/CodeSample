// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/00_Inventory/BossItemGetWidget.h"

#include "03_Widget/01_Menu/00_Inventory/BossItemGetElementWidget.h"
#include "04_Item/ItemActor.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"

void UBossItemGetWidget::OnClicked()
{
	RemoveFromParent();
}

void UBossItemGetWidget::NativeConstruct()
{
	Super::NativeConstruct();

	VerticalBox_Element->ClearChildren();
	Button_OK->OnClicked.AddUniqueDynamic(this,&UBossItemGetWidget::OnClicked);
}

void UBossItemGetWidget::AddItemElement(AItemActor* Item)
{
	
	if(BossItemGetElementObject)
	{
	
		if(auto widget = CreateWidget<UBossItemGetElementWidget>(GetOwningPlayer(),BossItemGetElementObject))
		{
			VerticalBox_Element->AddChildToVerticalBox(widget);
			widget->SetItemInformation(Item->GetItemInformation(),Item->GetItemCount());
		}
		
	}
}
