// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/08_StatusEffect/StatusEffectListWidget.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "03_Widget/08_StatusEffect/StatusEffectProgressWidget.h"
#include "Components/VerticalBox.h"
#include "Logging/StructuredLog.h"

void UStatusEffectListWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (const auto attComp = GetOwningPlayerPawn<APlayerCharacter>()->GetAttributeComponent())
	{
		attComp->OnUpdateStatusEffect.AddUniqueDynamic(this, &UStatusEffectListWidget::OnUpdateStatusEffect);
	}
}

void UStatusEffectListWidget::UpdateStatusEffect(EStatusEffect StatusEffect, float Percent)
{
	if (Percent <= 0.f)
	{
		if (AddedWidgetList.Contains(StatusEffect))
		{
			VerticalBox_EffectList->RemoveChild(AddedWidgetList[StatusEffect]);
			AddedWidgetList.Remove(StatusEffect);
		}
		return;
	}

	if (StatusEffectProgressWidgetObject)
	{
		if (!AddedWidgetList.Contains(StatusEffect))
		{
			auto widget = CreateWidget<
				UStatusEffectProgressWidget>(GetOwningPlayer(), StatusEffectProgressWidgetObject);
			widget->SetImage(StatusEffect);
			VerticalBox_EffectList->AddChildToVerticalBox(widget);
			AddedWidgetList.Add(StatusEffect, widget);
		}

		AddedWidgetList[StatusEffect]->SetProgress(Percent);
	}
}

void UStatusEffectListWidget::OnUpdateStatusEffect(EStatusEffect StatusEffect, float Value, float ResistValue)
{
	UE_LOGFMT(LogTemp,Log,"{0} / {1}",Value,ResistValue);
	
	UpdateStatusEffect(StatusEffect, Value / ResistValue);
}
