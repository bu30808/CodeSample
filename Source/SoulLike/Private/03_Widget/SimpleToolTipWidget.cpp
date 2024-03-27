// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/SimpleToolTipWidget.h"

#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Logging/StructuredLog.h"

void USimpleToolTipWidget::SetDescriptionText(FText Msg)
{
	RichTextBlock_Description->SetText(Msg);
}

void USimpleToolTipWidget::AddDescriptionText(const FText& AddDescription)
{
	auto cur = RichTextBlock_Description->GetText().ToString();
	cur += AddDescription.ToString();

	RichTextBlock_Description->SetText(FText::FromString(cur));
}
