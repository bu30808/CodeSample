// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/NavigationWidget.h"

#include "00_Character/00_Player/00_Controller/UserController.h"
#include "03_Widget/MainWidget.h"
#include "03_Widget/01_Menu/NavigationButtonWidget.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "Components/Button.h"

/*
void UNavigationWidget::Init(UMainMenuWidget* MenuWidget)
{
	MainMenuWidget = MenuWidget;

	if (MainMenuWidget.IsValid())
	{
		Button_Item->Button->OnClicked.AddUniqueDynamic(MenuWidget, &UMainMenuWidget::OnClickedItemButton);
		//Button_Quick->Button->OnClicked.AddUniqueDynamic(MenuWidget, &UMainMenuWidget::OnClickedQuickSettingButton);
		Button_Equip->Button->OnClicked.AddUniqueDynamic(MenuWidget, &UMainMenuWidget::OnClickedEquipButton);
		Button_Orb->Button->OnClicked.AddUniqueDynamic(MenuWidget, &UMainMenuWidget::OnClickedOrbButton);
	}
}
*/
void UNavigationWidget::NativeConstruct()
{
	Super::NativeConstruct();
}


void UNavigationWidget::Init(UMainWidget* Widget)
{
	MainWidget = Widget;

	if (MainWidget.IsValid())
	{
		Button_CharacterInformation->Button->OnClicked.AddUniqueDynamic(Widget, &UMainWidget::OpenCharacterInformation);
		Button_Item->Button->OnClicked.AddUniqueDynamic(Widget, &UMainWidget::OpenInventory);
		Button_Equip->Button->OnClicked.AddUniqueDynamic(Widget, &UMainWidget::OpenEquipment);
		Button_Orb->Button->OnClicked.AddUniqueDynamic(Widget, &UMainWidget::OpenOrb);
		Button_Option->Button->OnClicked.AddUniqueDynamic(Widget, &UMainWidget::OpenOption);
	}
}
