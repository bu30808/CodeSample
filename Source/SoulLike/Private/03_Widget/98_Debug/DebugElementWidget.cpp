// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/98_Debug/DebugElementWidget.h"

#include "03_Widget/SimpleToolTipWidget.h"
#include "03_Widget/98_Debug/AbilityDebugWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UDebugElementWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (ToolTipWidgetObject)
	{
		SetToolTip(CreateWidget(GetOwningPlayer(), ToolTipWidgetObject));
	}
}

void UDebugElementWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if (ListItemObject->IsA<UAbilityDebuggingData>())
	{
		auto data = Cast<UAbilityDebuggingData>(ListItemObject);
		const auto& info = data->AbilityInformation;

		Image_Ability->SetVisibility(ESlateVisibility::Visible);
		TextBlock_Tag->SetVisibility(ESlateVisibility::Collapsed);

		Image_Ability->SetBrushFromSoftTexture(info.AbilityImage);

		FString msg = info.AbilityName.ToString() + "\n" + info.AbilityTag.ToString() + "\n" + info.AbilityDescription.
			ToString();
		Cast<USimpleToolTipWidget>(GetToolTip())->SetDescriptionText(FText::FromString(msg));

		return;
	}

	if (ListItemObject->IsA<UAbilityEffectDebuggingData>())
	{
		auto data = Cast<UAbilityEffectDebuggingData>(ListItemObject);
		Image_Ability->SetVisibility(ESlateVisibility::Collapsed);
		TextBlock_Tag->SetVisibility(ESlateVisibility::Visible);

		TextBlock_Tag->SetText(FText::FromString(data->AbilityEffectTag.ToString()));

		return;
	}

	if (ListItemObject->IsA<UAbilityCueDebuggingData>())
	{
		auto data = Cast<UAbilityCueDebuggingData>(ListItemObject);
		Image_Ability->SetVisibility(ESlateVisibility::Collapsed);
		TextBlock_Tag->SetVisibility(ESlateVisibility::Visible);

		TextBlock_Tag->SetText(FText::FromString(data->CueTag.ToString()));
	}
}

void UDebugIgnoreMoveInputWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if (auto data = Cast<UIgnoreMoveInputDebuggingData>(ListItemObject))
	{
		TextBlock_Tag->SetText(FText::FromString(data->Tag.ToString()));
		if (data->By.IsValid())
		{
			TextBlock_Who->SetText(FText::FromString(data->By->GetActorNameOrLabel()));
		}
		else
		{
			TextBlock_Who->SetText(FText::FromString(TEXT("NULL")));
		}
		TextBlock_Count->SetText(FText::AsNumber(data->Count));
	}
}
