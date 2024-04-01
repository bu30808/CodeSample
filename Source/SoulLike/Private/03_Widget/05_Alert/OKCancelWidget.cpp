// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/05_Alert/OKCancelWidget.h"

#include "Components/Button.h"
#include "Components/RichTextBlock.h"

void UOKCancelWidget::NativeConstruct()
{
	bUseAsPopUp = true;
	bRemovable = true;

	Super::NativeConstruct();
	Button_Ok->OnClicked.AddUniqueDynamic(this, &UOKCancelWidget::OnClickedOKButton);
	Button_Cancel->OnClicked.AddUniqueDynamic(this, &UOKCancelWidget::OnClickedCancelButton);
}

void UOKCancelWidget::OnClickedOKButton()
{
	OnClickedOK.ExecuteIfBound();
	SetVisibility(ESlateVisibility::Collapsed);
}

void UOKCancelWidget::OnClickedCancelButton()
{
	OnClickedCancel.ExecuteIfBound();
	SetVisibility(ESlateVisibility::Collapsed);
}

void UOKCancelWidget::SetAlertMsg(EAlertMsgType AlertMsgType, FText Msg, const FOnButtonClicked& OnOkClickedEvent,
	const FOnButtonClicked& OnCancelClickedEvent)
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
	OnClickedOK = OnOkClickedEvent;
	OnClickedCancel = OnCancelClickedEvent;
}
