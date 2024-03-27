// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/03_LevelUp/AttributePreviewElementWidget.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "Components/HorizontalBox.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "SoulLike/SoulLike.h"
#include "03_Widget/SimpleToolTipWidget.h"
#include "Logging/StructuredLog.h"

void UAttributePreviewElementWidget::NativeConstruct()
{
	Super::NativeConstruct();
	HorizontalBox_Next->SetVisibility(ESlateVisibility::Hidden);
	Init(AttributeType);

	if (ToolTipWidgetObject)
	{
		if (auto widget = CreateWidget<USimpleToolTipWidget>(GetOwningPlayer(), ToolTipWidgetObject))
		{
			widget->SetDescriptionText(AttributeTypeToText(AttributeType));
			SetToolTip(widget);
		}
	}
}

void UAttributePreviewElementWidget::Init(EAttributeType Attribute)
{
	AttributeType = Attribute;

	RichTextBlock_Icon->SetText(FText::FromString(AttributeTypeToImageString(Attribute)));
	HorizontalBox_Next->SetVisibility(ESlateVisibility::Hidden);

	if (auto player = GetOwningPlayerPawn<APlayerCharacter>())
	{
		if (auto att = player->GetAttributeComponent()->GetAttributeByType(AttributeType))
		{
			BaseValue = att->GetBase();
			TextBlock_Cur->SetText(FText::AsNumber(att->GetBase()));
		}
		else
		{
			RemoveFromParent();
		}
	}
}

/*
void UAttributePreviewElementWidget::SetPreview(float PreviewVal) const
{
	if (auto player = GetOwningPlayerPawn<APlayerCharacter>())
	{
		if (auto att = player->GetAttributeComponent()->GetAttributeByType(AttributeType))
		{
			if (att->GetBase() != PreviewVal)
			{
				TextBlock_Next->SetText(FText::AsNumber(PreviewVal));
				HorizontalBox_Next->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}
	}
}*/

void UAttributePreviewElementWidget::Reset() const
{

	HorizontalBox_Next->SetVisibility(ESlateVisibility::Hidden);
}

void UAttributePreviewElementWidget::UpdatePreview(float AddValue)
{	
	AccValue += AddValue;

	TextBlock_Next->SetText(FText::AsNumber(BaseValue + AccValue));
	HorizontalBox_Next->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
