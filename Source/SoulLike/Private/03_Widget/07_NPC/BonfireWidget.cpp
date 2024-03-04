// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/07_NPC/BonfireWidget.h"

#include "96_Library/WidgetHelperLibrary.h"


void UBonfireWidget::NativeConstruct()
{
	bUseAsPopUp = true;
	bRemovable = true;
	Super::NativeConstruct();
}
