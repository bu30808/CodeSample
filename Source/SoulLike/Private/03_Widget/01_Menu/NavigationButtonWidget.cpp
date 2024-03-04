// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/NavigationButtonWidget.h"

#include "Components/Button.h"

void UNavigationButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
	//Button->OnClicked.AddUniqueDynamic(this,&UNavigationButtonWidget::OnClickedButton);
}
