// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/97_PopUp/PopUpBasedWidget.h"

#include "96_Library/WidgetHelperLibrary.h"

void UPopUpBasedWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if(bUseAsPopUp)
	{
		UWidgetHelperLibrary::PopUpWidgetProcess(this, bRemovable);
		OnVisibilityChanged.AddUniqueDynamic(this,&UPopUpBasedWidget::OnVisibilityChangedEvent);
	}
}

void UPopUpBasedWidget::OnVisibilityChangedEvent(ESlateVisibility InVisibility)
{
	if(bUseAsPopUp)
	{
		UWidgetHelperLibrary::PopUpWidgetProcess(this, bRemovable);
	}
}
