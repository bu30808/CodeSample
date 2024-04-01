// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/05_Alert/AlertWidget.h"

#include "96_Library/WidgetHelperLibrary.h"
#include "Components/Button.h"
#include "Components/RichTextBlock.h"


void UAlertWidget::NativeConstruct()
{
	bUseAsPopUp = true;
	bRemovable = true;

	Super::NativeConstruct();
	Button_Ok->OnClicked.AddUniqueDynamic(this, &UAlertWidget::OnClickedOKButton);
}

void UAlertWidget::OnClickedOKButton()
{
	OnClicked.ExecuteIfBound();
	SetVisibility(ESlateVisibility::Collapsed);
}

void UAlertWidget::SetAlertMsg(EAlertMsgType AlertMsgType, FText Msg, const FOnButtonClicked& OnClickedEvent)
{
	FString msg;
	switch (AlertMsgType)
	{
	case EAlertMsgType::Normal:
		msg += "<alert.normal>";
		break;
	case EAlertMsgType::Warning:
		msg += "<alert.warning>";
		break;
	case EAlertMsgType::Error:
		msg += "<alert.error>";
		break;
	default: ;
	}

	msg += Msg.ToString();
	if (AlertMsgType != EAlertMsgType::NoUseDeco)
	{
		msg += "</>";
	}
	RichTextBlock_Msg->SetText(FText::FromString(msg));
	OnClicked = OnClickedEvent;


	//RichTextBlock_Msg->SetColorAndOpacity(FSlateColor(Colors[AlertMsgType]));
}
