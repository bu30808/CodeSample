// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/04_Character/SoulWidget.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "96_Library/ComponentHelperLibrary.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"

void USoulWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (auto attComp = GetOwningPlayerPawn<ABaseCharacter>()->GetAttributeComponent())
	{
		if (TextBlock_Soul)
		{
			TextBlock_Soul->SetText(FText::AsNumber(attComp->GetEXP()));
		}
		attComp->OnCharacterInformationUpdate.AddUniqueDynamic(this, &USoulWidget::OnCharacterInformationUpdateEvent);
		attComp->OnUpdateExp.AddUniqueDynamic(this, &USoulWidget::OnUpdateEXPEvent);

		UpdateDissolve.BindDynamic(this, &USoulWidget::OnLerpEXPEvent);

		if (HorizontalBox_GetSoul)
		{
			HorizontalBox_GetSoul->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void USoulWidget::OnCharacterInformationUpdateEvent()
{
	if (auto attComp = GetOwningPlayerPawn<ABaseCharacter>()->GetAttributeComponent())
	{
		TextBlock_Soul->SetText(FText::AsNumber(attComp->GetEXP()));
	}
}

/*
void USoulWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	auto lerp = FMath::FInterpTo(CurExp,NextExp,InDeltaTime,10.f);
	TextBlock_Soul->SetText(FText::AsNumber((int32)lerp));
}
*/

void USoulWidget::OnUpdateEXPEvent(float AddEXP)
{
	if (auto player = GetOwningPlayerPawn<ABaseCharacter>())
	{
		if (EXPLerpTimeLine == nullptr)
		{
			EXPLerpTimeLine = UComponentHelperLibrary::CreateActorComponent<UTimelineComponent>(
				player,TEXT("EXPLerpComponent"));
			EXPLerpTimeLine->AddInterpFloat(EXPLerpCurve, UpdateDissolve);
			EXPLerpTimeLine->SetTimelineLength(LerpLength);
		}

		if (auto attComp = player->GetAttributeComponent())
		{
			NextExp = attComp->GetEXP();
			CurExp = NextExp - AddEXP;

			EXPLerpTimeLine->PlayFromStart();

			HorizontalBox_GetSoul->SetVisibility(ESlateVisibility::Visible);
			TextBlock_SoulAdd->SetText(FText::AsNumber(static_cast<int32>(AddEXP)));
		}
	}
}

void USoulWidget::OnLerpEXPEvent(float Alpha)
{
	if (Alpha == 1.f)
	{
		HorizontalBox_GetSoul->SetVisibility(ESlateVisibility::Hidden);
	}

	auto lerpValue = FMath::Lerp(CurExp, NextExp, Alpha);
	TextBlock_Soul->SetText(FText::AsNumber(static_cast<int32>(lerpValue)));
}
