// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "02_Ability/AbilityEffect.h"
#include "03_Widget/01_Menu/05_Orb/OrbMatrixElementWidget.h"


//AI테스크 사용
DECLARE_DELEGATE(FAITaskFinishDelegate);

//Float as String With Precision!
static FORCEINLINE FString GetFloatAsStringWithPrecision(float TheFloat, int32 Precision,
                                                         bool IncludeLeadingZero = true)
{
	//Round to integral if have something like 1.9999 within precision
	float Rounded = roundf(TheFloat);
	if (FMath::Abs(TheFloat - Rounded) < FMath::Pow(10.f, -1 * Precision))
	{
		TheFloat = Rounded;
	}
	FNumberFormattingOptions NumberFormat; //Text.h
	NumberFormat.MinimumIntegralDigits = (IncludeLeadingZero) ? 1 : 0;
	NumberFormat.MaximumIntegralDigits = 10000;
	NumberFormat.MinimumFractionalDigits = Precision;
	NumberFormat.MaximumFractionalDigits = Precision;
	return FText::AsNumber(TheFloat, &NumberFormat).ToString();
}

//Float as FText With Precision!
static FORCEINLINE FText GetFloatAsTextWithPrecision(float TheFloat, int32 Precision, bool IncludeLeadingZero = true)
{
	//Round to integral if have something like 1.9999 within precision
	float Rounded = roundf(TheFloat);
	if (FMath::Abs(TheFloat - Rounded) < FMath::Pow(10.f, -1 * Precision))
	{
		TheFloat = Rounded;
	}
	FNumberFormattingOptions NumberFormat; //Text.h
	NumberFormat.MinimumIntegralDigits = (IncludeLeadingZero) ? 1 : 0;
	NumberFormat.MaximumIntegralDigits = 10000;
	NumberFormat.MinimumFractionalDigits = Precision;
	NumberFormat.MaximumFractionalDigits = Precision;
	return FText::AsNumber(TheFloat, &NumberFormat);
}

#define LOCTEXT_NAMESPACE "EnumTextConvertHelper"

static FORCEINLINE FText AttributeTypeToText(EAttributeType Type)
{
	switch (Type)
	{
	case EAttributeType::NONE:
		break;
	case EAttributeType::HP:
		return NSLOCTEXT("EnumTextConvertHelper", "HP", "체력");
	case EAttributeType::SP:
		return NSLOCTEXT("EnumTextConvertHelper", "SP", "스테미너");
	case EAttributeType::MP:
		return NSLOCTEXT("EnumTextConvertHelper", "MP", "신력");
	case EAttributeType::MaxHP:
		return NSLOCTEXT("EnumTextConvertHelper", "MaxHP", "최대 체력");
	case EAttributeType::MaxSP:
		return NSLOCTEXT("EnumTextConvertHelper", "MaxSP", "최대 스테미너");
	case EAttributeType::MaxMP:
		return NSLOCTEXT("EnumTextConvertHelper", "MaxMP", "최대 신력");
	case EAttributeType::RecoverHP:
		return NSLOCTEXT("EnumTextConvertHelper", "RecoverHP", "체력 회복량");
	case EAttributeType::RecoverSP:
		return NSLOCTEXT("EnumTextConvertHelper", "RecoverSP", "스테미너 회복량");
	case EAttributeType::RecoverMP:
		return NSLOCTEXT("EnumTextConvertHelper", "RecoverMP", "신력 회복량");
	case EAttributeType::PhysicalAttack:
		return NSLOCTEXT("EnumTextConvertHelper", "PhysicalAttack", "물리공격력");
	case EAttributeType::MagicalAttack:
		return NSLOCTEXT("EnumTextConvertHelper", "MagicalAttack", "마법공격력");
	case EAttributeType::PhysicalDefense:
		return NSLOCTEXT("EnumTextConvertHelper", "PhysicalDefense", "물리방어력");
	case EAttributeType::MagicalDefense:
		return NSLOCTEXT("EnumTextConvertHelper", "MagicalDefense", "마법방어력");
	case EAttributeType::ActionSpeed:
		return NSLOCTEXT("EnumTextConvertHelper", "ActionSpeed", "행동속도");
	case EAttributeType::Endurance:
		return NSLOCTEXT("EnumTextConvertHelper", "Endurance", "강인도");
	case EAttributeType::Strength:
		return NSLOCTEXT("EnumTextConvertHelper", "Strength", "힘");
	case EAttributeType::Dexterity:
		return NSLOCTEXT("EnumTextConvertHelper", "Dexterity", "민첩");
	case EAttributeType::Intelligence:
		return NSLOCTEXT("EnumTextConvertHelper", "Intelligence", "지능");
	case EAttributeType::Willpower:
		return NSLOCTEXT("EnumTextConvertHelper", "Willpower", "의지");
	case EAttributeType::Level:
		return NSLOCTEXT("EnumTextConvertHelper", "Level", "레벨");
	case EAttributeType::EXP:
		return NSLOCTEXT("EnumTextConvertHelper", "EXP", "영혼");
	default: ;
	}

	return FText::FromString("ERROR");
}

//id = "어쩌구저쩌구" 와 같이 = 사이에 띄어쓰기가 있으면 안 됩니다.
static FORCEINLINE FString AttributeTypeToImageString(EAttributeType Type)
{
	FString imageString;
	switch (Type)
	{
	case EAttributeType::NONE:
		break;
	case EAttributeType::HP:
		imageString = "<img id=\"attribute.hp\"/>";
		break;
	case EAttributeType::SP:
		imageString = "<img id=\"attribute.sp\"/>";
		break;
	case EAttributeType::MP:
		imageString = "<img id=\"attribute.mp\"/>";
		break;
	case EAttributeType::MaxHP:
		imageString = "<img id=\"attribute.maxhp\"/>";
		break;
	case EAttributeType::MaxSP:
		imageString = "<img id=\"attribute.maxsp\"/>";
		break;
	case EAttributeType::MaxMP:
		imageString = "<img id=\"attribute.maxmp\"/>";
		break;
	case EAttributeType::RecoverHP:
		imageString = "<img id=\"attribute.recoverhp\"/>";
		break;
	case EAttributeType::RecoverSP:
		imageString = "<img id=\"attribute.recoversp\"/>";
		break;
	case EAttributeType::RecoverMP:
		imageString = "<img id=\"attribute.recoverhitmp\"/>";
		break;
	case EAttributeType::PhysicalAttack:
		imageString = "<img id=\"attribute.physicalattack\"/>";
		break;
	case EAttributeType::MagicalAttack:
		imageString = "<img id=\"attribute.magicalattack\"/>";
		break;
	case EAttributeType::PhysicalDefense:
		imageString = "<img id=\"attribute.physicaldefense\"/>";
		break;
	case EAttributeType::MagicalDefense:
		imageString = "<img id=\"attribute.magicaldefense\"/>";
		break;
	case EAttributeType::ActionSpeed:
		imageString = "<img id=\"attribute.actionspeed\"/>";
		break;
	case EAttributeType::Endurance:
		imageString = "<img id=\"attribute.endurance\"/>";
		break;
	case EAttributeType::Strength:
		imageString = "<img id=\"attribute.str\"/>";
		break;
	case EAttributeType::Dexterity:
		imageString = "<img id=\"attribute.dex\"/>";
		break;
	case EAttributeType::Intelligence:
		imageString = "<img id=\"attribute.int\"/>";
		break;
	case EAttributeType::Willpower:
		imageString = "<img id=\"attribute.will\"/>";
		break;
	case EAttributeType::Level:
		imageString = "<img id=\"attribute.level\"/>";
		break;
	case EAttributeType::EXP:
		imageString = "<img id=\"attribute.exp\"/>";
		break;
	case EAttributeType::MAX:
		break;
	default: ;
	}

	return imageString;
}

static FORCEINLINE FText OrbMatrixSlotTypeToText(EOrbMatrixSlotType Type)
{
	switch (Type)
	{
	case EOrbMatrixSlotType::CORE:
		return NSLOCTEXT("EnumTextConvertHelper", "CORE", "코어");
	case EOrbMatrixSlotType::PHYSICAL:
		return NSLOCTEXT("EnumTextConvertHelper", "PHYSICAL", "물리");
	case EOrbMatrixSlotType::DEFENCE:
		return NSLOCTEXT("EnumTextConvertHelper", "DEFENCE", "방어");
	case EOrbMatrixSlotType::MAGICAL:
		return NSLOCTEXT("EnumTextConvertHelper", "MAGICAL", "마법");
	case EOrbMatrixSlotType::FREE:
		return NSLOCTEXT("EnumTextConvertHelper", "FREE", "자유");
	default: ;
	}

	return FText::FromString("ERROR");
}

static FORCEINLINE FText OrbMatrixSlotContentToString(EOrbMatrixSlotContent Type)
{
	switch (Type)
	{
	case EOrbMatrixSlotContent::Empty:
		return NSLOCTEXT("EnumTextConvertHelper", "Empty", "비어있음");
	case EOrbMatrixSlotContent::Set:
		return FText::GetEmpty();
	default: ;
	}

	return FText::FromString("ERROR");
}

static FORCEINLINE FText RarityToText(EItemRarity Type)
{
	switch (Type)
	{
	case EItemRarity::Common:
		return NSLOCTEXT("EnumTextConvertHelper", "Common", "일반");
	case EItemRarity::Rare:
		return NSLOCTEXT("EnumTextConvertHelper", "Rare", "희귀");
	case EItemRarity::Epic:
		return NSLOCTEXT("EnumTextConvertHelper", "Epic", "영웅");
	case EItemRarity::Legendary:
		return NSLOCTEXT("EnumTextConvertHelper", "Legendary", "전설");
	default: ;
	}

	return FText::FromString("ERROR");
}

static FORCEINLINE FText RarityToDecoText(EItemRarity Type)
{
	switch (Type)
	{
	case EItemRarity::Common:
		return FText::FromString("<orb.common>");
	case EItemRarity::Rare:
		return FText::FromString("<orb.rare>");
	case EItemRarity::Epic:
		return FText::FromString("<orb.epic>");
	case EItemRarity::Legendary:
		return FText::FromString("<orb.legendary>");
	default: ;
	}

	return FText::FromString("ERROR");
}


static FORCEINLINE FText SlotTypeToDecoText(EOrbMatrixSlotType SlotType)
{
	switch (SlotType)
	{
	case EOrbMatrixSlotType::CORE:
		return FText::FromString("<orb.core>");
	case EOrbMatrixSlotType::PHYSICAL:
		return FText::FromString("<orb.physical>");
	case EOrbMatrixSlotType::DEFENCE:
		return FText::FromString("<orb.defence>");
	case EOrbMatrixSlotType::MAGICAL:
		return FText::FromString("<orb.magical>");
	case EOrbMatrixSlotType::FREE:
		return FText::FromString("<orb.free>");
	default: ;
	}

	return FText::FromString("ERROR");
}
#undef LOCTEXT_NAMESPACE
