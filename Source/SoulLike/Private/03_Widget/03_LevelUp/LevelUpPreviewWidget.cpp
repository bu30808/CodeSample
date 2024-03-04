// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/03_LevelUp/LevelUpPreviewWidget.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "03_Widget/03_LevelUp/AttributePreviewElementWidget.h"
#include "Blueprint/WidgetTree.h"
#include "03_Widget/03_LevelUp/AttributePreviewWidget.h"
#include "96_Library/AttributeHelperLibrary.h"

void ULevelUpPreviewWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	TArray<UWidget*> widgets;
	WidgetTree->GetAllWidgets(widgets);

	for (auto w : widgets)
	{
		if (w->IsA<UAttributePreviewElementWidget>())
		{
			auto widget = Cast<UAttributePreviewElementWidget>(w);
			Elements.Add(widget->GetAttributeType(), widget);
		}
	}
}

void ULevelUpPreviewWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void ULevelUpPreviewWidget::OnAddAttributePreviewPointEvent(EAttributePointType AttributeType)
{
	if (auto player = GetOwningPlayerPawn<APlayerCharacter>())
	{
		const auto& map = player->GetAttributeComponent()->GetAttributePerPointMap();
		auto convertType = UAttributeHelperLibrary::ConvertAttributePointTypeToAttributeType(AttributeType);

		if (map.Contains(convertType))
		{
			auto arr = map[convertType];

			for (auto pair : arr)
			{
				if (Elements.Contains(pair.Key))
				{
					Elements[pair.Key]->UpdatePreview(pair.Value);
				}
			}
		}
	}
}


void ULevelUpPreviewWidget::Reset()
{
	for (auto iter : Elements)
	{
		iter.Value->Reset();
	}
}

void ULevelUpPreviewWidget::Init()
{
	for (auto iter : Elements)
	{
		iter.Value->Init(iter.Key);
	}
}
