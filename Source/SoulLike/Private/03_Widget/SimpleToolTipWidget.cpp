// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/SimpleToolTipWidget.h"

#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Logging/StructuredLog.h"

void USimpleToolTipWidget::SetDescriptionText(FString Msg)
{
	RichTextBlock_Description->SetText(FText::FromString(Msg));
}

void USimpleToolTipWidget::AddDescriptionText(const FString& AddDescription)
{
	auto cur = RichTextBlock_Description->GetText().ToString();
	cur += AddDescription;

	RichTextBlock_Description->SetText(FText::FromString(cur));
}
