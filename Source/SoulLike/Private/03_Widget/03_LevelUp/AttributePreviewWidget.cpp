// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/03_LevelUp/AttributePreviewWidget.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "99_Subsystem/LevelUpSubsystem.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Logging/StructuredLog.h"
#include "SoulLike/SoulLike.h"

void UAttributePreviewWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HorizontalBox_Next->SetVisibility(ESlateVisibility::Hidden);
	Button_Add->SetVisibility(ESlateVisibility::Hidden);

	Button_Add->OnClicked.AddUniqueDynamic(this, &UAttributePreviewWidget::OnClickedEvent);

	Init();
}


void UAttributePreviewWidget::OnClickedEvent()
{
	if (LevelUpSubsystem.IsValid())
	{
		HorizontalBox_Next->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		//다음 레벨로 가려는 경험치가 충분한지 확인합니다.
		auto attComp = GetOwningPlayerPawn<APlayerCharacter>()->GetAttributeComponent();
		auto curLevel = attComp->GetLevel();
		if (LevelUpSubsystem->CanLevelUp(attComp, curLevel, curLevel + AddPoint))
		{
			AddPoint++;
			int32 val = 0;
			switch (AttributePointType)
			{
			case EAttributePointType::MAXHP:
				val = attComp->GetMaxHPAttribute().GetBase() + static_cast<int32>(attComp->GetAttributePerPoint().
						MaxHPPerPoint) *
					AddPoint;
				break;
			case EAttributePointType::MAXSP:
				val = attComp->GetMaxSPAttribute().GetBase() + static_cast<int32>(attComp->GetAttributePerPoint().
						MaxSPPerPoint) *
					AddPoint;
				break;
			case EAttributePointType::MAXMP:
				val = attComp->GetMaxMPAttribute().GetBase() + static_cast<int32>(attComp->GetAttributePerPoint().
						MaxMPPerPoint) *
					AddPoint;
				break;
			case EAttributePointType::STR:
				val = static_cast<int32>(attComp->GetStrengthAttribute().GetBase()) + AddPoint;
				break;
			case EAttributePointType::DEX:
				val = static_cast<int32>(attComp->GetDexterityAttribute().GetBase()) + AddPoint;
				break;
			case EAttributePointType::INT:
				val = static_cast<int32>(attComp->GetIntelligenceAttribute().GetBase()) + AddPoint;
				break;
			case EAttributePointType::WILL:
				val = static_cast<int32>(attComp->GetWillpowerAttribute().GetBase()) + AddPoint;
				break;
			case EAttributePointType::MAX:
				break;
			default: ;
			}

			TextBlock_Next->SetText(FText::AsNumber(val));
			OnAddAttributePreviewPoint.Broadcast(AttributePointType);
		}
		else
		{
			Button_Add->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}


void UAttributePreviewWidget::ReadyToLevelUp() const
{
	if (AttributePointType == EAttributePointType::LEVEL || AttributePointType == EAttributePointType::NEXT_EXP ||
		AttributePointType == EAttributePointType::EXP)
	{
		return;
	}

	Button_Add->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}


void UAttributePreviewWidget::Reset()
{
	AddPoint = 0;
	HorizontalBox_Next->SetVisibility(ESlateVisibility::Hidden);
	Button_Add->SetVisibility(ESlateVisibility::Hidden);
}

void UAttributePreviewWidget::Init()
{
	if (const auto player = GetOwningPlayerPawn<APlayerCharacter>())
	{
		LevelUpSubsystem = GetOwningPlayer()->GetLocalPlayer()->GetSubsystem<ULevelUpSubsystem>();

		const auto attComp = player->GetAttributeComponent();
		float value = 0;

		switch (AttributePointType)
		{
		case EAttributePointType::LEVEL:
			value = attComp->GetLevelAttribute().GetBase();
			break;
		case EAttributePointType::EXP:
			value = attComp->GetEXP();
			break;
		case EAttributePointType::NEXT_EXP:
			value = LevelUpSubsystem->GetNextLevelUpExp(attComp->GetLevel());
			break;
		case EAttributePointType::MAXHP:
			value = attComp->GetMaxHPAttribute().GetBase();
			break;
		case EAttributePointType::MAXSP:
			value = attComp->GetMaxSPAttribute().GetBase();
			break;
		case EAttributePointType::MAXMP:
			value = attComp->GetMaxMPAttribute().GetBase();
			break;
		case EAttributePointType::STR:
			value = attComp->GetStrengthAttribute().GetBase();
			break;
		case EAttributePointType::DEX:
			value = attComp->GetDexterityAttribute().GetBase();
			break;
		case EAttributePointType::INT:
			value = attComp->GetIntelligenceAttribute().GetBase();
			break;
		case EAttributePointType::WILL:
			value = attComp->GetWillpowerAttribute().GetBase();
			break;
		case EAttributePointType::MAX:
			break;
		default: ;
		}


		TextBlock_Cur->SetText(FText::FromString(GetFloatAsStringWithPrecision(value, 0)));
	}
}
