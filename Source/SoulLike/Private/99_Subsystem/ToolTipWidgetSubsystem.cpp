// Fill out your copyright notice in the Description page of Project Settings.


#include "99_Subsystem/ToolTipWidgetSubsystem.h"
#include "03_Widget/SimpleToolTipWidget.h"

UToolTipWidgetSubsystem::UToolTipWidgetSubsystem()
{
	static ConstructorHelpers::FClassFinder<USimpleToolTipWidget> widget(TEXT(
		"/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/02_Widget/Menu/UMG_ToolTipSimple.UMG_ToolTipSimple_C'"));
	if (widget.Succeeded())
	{
		SimpleToolTipWidgetObject = widget.Class;
	}
}

USimpleToolTipWidget* UToolTipWidgetSubsystem::CreateSimpleToolTipWidget(class APlayerController* PC, FString Msg) const
{
	if (SimpleToolTipWidgetObject)
	{
		if (auto widget = CreateWidget<USimpleToolTipWidget>(PC, SimpleToolTipWidgetObject))
		{
			widget->SetDescriptionText(Msg);

			return widget;
		}
	}
	return nullptr;
}
