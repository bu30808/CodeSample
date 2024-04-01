// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/04_Character/CharacterInfoElementWidget.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "Components/RichTextBlock.h"
#include "SoulLike/SoulLike.h"
#include "03_Widget/SimpleToolTipWidget.h"
#include "Logging/StructuredLog.h"
#define LOCTEXT_NAMESPACE "CharacterInfoElementWidget"

void UCharacterInfoElementWidget::Init(EAttributeType NewType)
{
	AttributeType = NewType;


	FText toolTip = AttributeTypeToText(AttributeType);


	if (RichTextBlock_Text->GetToolTip() == nullptr)
	{
		if (auto widget = CreateWidget<USimpleToolTipWidget>(GetOwningPlayer(), SimpleToolTipWidgetObject))
		{
			widget->SetDescriptionText(toolTip);
			RichTextBlock_Text->SetToolTip(widget);
		}
	}
	else
	{
		if (auto widget = Cast<USimpleToolTipWidget>(RichTextBlock_Text->GetToolTip()))
		{
			widget->SetDescriptionText(toolTip);
			RichTextBlock_Text->SetToolTip(widget);
		}
	}

	UpdateAttributeValue();
}

void UCharacterInfoElementWidget::UpdateAttributeValue()
{
	const FString msg = CreateImageString() + " " + GetAttributeValueString();
	RichTextBlock_Text->SetText(FText::FromString(msg));
}

void UCharacterInfoElementWidget::NativeConstruct()
{
	Super::NativeConstruct();


	/*FString msg = CreateImageString()+GetAttributeValueString();
	RichTextBlock_Text->SetText(FText::FromString(msg));*/
}

FString UCharacterInfoElementWidget::CreateImageString() const
{
	return AttributeTypeToImageString(AttributeType);
}

FString UCharacterInfoElementWidget::MakeStringValue(const FAttribute& Attribute) const
{
	const auto base = Attribute.GetBase();
	const auto cur = Attribute.GetCurrent();

	if (AttributeType == EAttributeType::Level || AttributeType == EAttributeType::EXP)
	{
		return GetFloatAsStringWithPrecision(cur, 0);
	}
	const FText baseStatusText = NSLOCTEXT("CharacterInfoElementWidget", "BaseStatusText", "기본값");
	const FText curStatusText = NSLOCTEXT("CharacterInfoElementWidget", "CurStatusText", "현재값");

	if (AttributeType == EAttributeType::RecoverHP || AttributeType == EAttributeType::RecoverSP || AttributeType ==
		EAttributeType::RecoverMP || AttributeType == EAttributeType::ActionSpeed)
	{
		if (base != cur)
		{
			UE_LOGFMT(LogTemp, Log, "어트리뷰트 타입 :{0}, 기본값 : {1} / 현재값 : {2}",
			          StaticEnum<EAttributeType>()->GetValueAsString(AttributeType), base, cur);


			return baseStatusText.ToString() + " : " + FString::SanitizeFloat(base) + ", " + curStatusText.ToString() +
				" : " +
				FString::SanitizeFloat(cur);
		}
		return FString::SanitizeFloat(cur);
	}

	if (base != cur)
	{
		return baseStatusText.ToString() + " : " + FString::FormatAsNumber(base) + ", " + curStatusText.ToString() +
			" : " + FString::FormatAsNumber(cur);
	}

	return FString::FormatAsNumber(cur);
}

FString UCharacterInfoElementWidget::GetAttributeValueString() const
{
	FString valueString;

	if (auto character = GetOwningPlayerPawn<ABaseCharacter>())
	{
		if (auto comp = character->GetAttributeComponent())
		{
			switch (AttributeType)
			{
			case EAttributeType::NONE:
				break;
			case EAttributeType::HP:
				valueString = MakeStringValue(comp->GetHPAttribute());
				break;
			case EAttributeType::SP:
				valueString = MakeStringValue(comp->GetSPAttribute());
				break;
			case EAttributeType::MP:
				valueString = MakeStringValue(comp->GetMPAttribute());
				break;
			case EAttributeType::MaxHP:
				valueString = MakeStringValue(comp->GetMaxHPAttribute());
				break;
			case EAttributeType::MaxSP:
				valueString = MakeStringValue(comp->GetMaxSPAttribute());
				break;
			case EAttributeType::MaxMP:
				valueString = MakeStringValue(comp->GetMaxMPAttribute());
				break;
			case EAttributeType::RecoverHP:
				valueString = MakeStringValue(comp->GetRecoverHPAttribute());
				break;
			case EAttributeType::RecoverSP:
				valueString = MakeStringValue(comp->GetRecoverSPAttribute());
				break;
			case EAttributeType::RecoverMP:
				valueString = MakeStringValue(comp->GetRecoverHitMPAttribute());
				break;
			case EAttributeType::PhysicalAttack:
				valueString = MakeStringValue(comp->GetPhysicalAttackAttribute());
				break;
			case EAttributeType::MagicalAttack:
				valueString = MakeStringValue(comp->GetMagicalAttackAttribute());
				break;
			case EAttributeType::PhysicalDefense:
				valueString = MakeStringValue(comp->GetPhysicalDefenseAttribute());
				break;
			case EAttributeType::MagicalDefense:
				valueString = MakeStringValue(comp->GetMagicalDefenseAttribute());
				break;
			case EAttributeType::ActionSpeed:
				valueString = MakeStringValue(comp->GetActionSpeedAttribute());
				break;
			case EAttributeType::Endurance:
				valueString = MakeStringValue(comp->GetEnduranceAttribute());
				break;
			case EAttributeType::Strength:
				valueString = MakeStringValue(comp->GetStrengthAttribute());
				break;
			case EAttributeType::Dexterity:
				valueString = MakeStringValue(comp->GetDexterityAttribute());
				break;
			case EAttributeType::Intelligence:
				valueString = MakeStringValue(comp->GetIntelligenceAttribute());
				break;
			case EAttributeType::Willpower:
				valueString = MakeStringValue(comp->GetWillpowerAttribute());
				break;
			case EAttributeType::Level:
				valueString = MakeStringValue(comp->GetLevelAttribute());
				break;
			case EAttributeType::EXP:
				valueString = MakeStringValue(comp->GetEXPAttribute());
				break;
			case EAttributeType::MAX:
				break;
			default: ;
			}
		}
	}

	return valueString;
}
#undef LOCTEXT_NAMESPACE
