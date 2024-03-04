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

static FORCEINLINE FString AttributeTypeToString(EAttributeType Type)
{
	FString toolTip;
	switch (Type)
	{
	case EAttributeType::NONE:
		break;
	case EAttributeType::HP:
		toolTip = TEXT("체력");
		break;
	case EAttributeType::SP:
		toolTip = TEXT("스테미너");
		break;
	case EAttributeType::MP:
		toolTip = TEXT("신력");
		break;
	case EAttributeType::MaxHP:
		toolTip = TEXT("최대체력");
		break;
	case EAttributeType::MaxSP:
		toolTip = TEXT("최대 스테미너");
		break;
	case EAttributeType::MaxMP:
		toolTip = TEXT("최대 신력");
		break;
	case EAttributeType::RecoverHP:
		toolTip = TEXT("초당 체력 회복량");
		break;
	case EAttributeType::RecoverSP:
		toolTip = TEXT("초당 스테미너 회복량");
		break;
	case EAttributeType::RecoverMP:
		toolTip = TEXT("타격시 신력 회복량");
		break;
	case EAttributeType::PhysicalAttack:
		toolTip = TEXT("물리공격력");
		break;
	case EAttributeType::MagicalAttack:
		toolTip = TEXT("마법공격력");
		break;
	case EAttributeType::PhysicalDefense:
		toolTip = TEXT("물리방어력");
		break;
	case EAttributeType::MagicalDefense:
		toolTip = TEXT("마법방어력");
		break;
	case EAttributeType::ActionSpeed:
		toolTip = TEXT("행동속도");
		break;
	case EAttributeType::Endurance:
		toolTip = TEXT("강인도");
		break;
	case EAttributeType::Strength:
		toolTip = TEXT("힘");
		break;
	case EAttributeType::Dexterity:
		toolTip = TEXT("민첩");
		break;
	case EAttributeType::Intelligence:
		toolTip = TEXT("지능");
		break;
	case EAttributeType::Willpower:
		toolTip = TEXT("의지");
		break;
	case EAttributeType::Level:
		toolTip = TEXT("레벨");
		break;
	case EAttributeType::EXP:
		toolTip = TEXT("영혼");
		break;
	default: ;
	}

	return toolTip;
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

static FORCEINLINE FString OrbMatrixSlotTypeToString(EOrbMatrixSlotType Type)
{
	switch (Type)
	{
	case EOrbMatrixSlotType::CORE:
		return TEXT("코어 슬롯");
	case EOrbMatrixSlotType::PHYSICAL:
		return TEXT("물리 슬롯");
	case EOrbMatrixSlotType::DEFENCE:
		return TEXT("방어 슬롯");
	case EOrbMatrixSlotType::MAGICAL:
		return TEXT("마법 슬롯");
	case EOrbMatrixSlotType::FREE:
		return TEXT("자유 슬롯");
	default: ;
	}

	return "ERROR";
}

static FORCEINLINE FString OrbMatrixSlotContentToString(EOrbMatrixSlotContent Type)
{
	switch (Type)
	{
	case EOrbMatrixSlotContent::Empty:
		return TEXT("비어있음");
	case EOrbMatrixSlotContent::Set:
		return "";
	default: ;
	}

	return "ERROR";
}

static FORCEINLINE FString RarityToString(EItemRarity Type)
{
	switch (Type)
	{
	case EItemRarity::Common:
		return TEXT("일반");
	case EItemRarity::Rare:
		return TEXT("희귀");
	case EItemRarity::Epic:
		return TEXT("영웅");
	case EItemRarity::Legendary:
		return TEXT("전설");
	default: ;
	}

	return "ERROR";
}

static FORCEINLINE FString RarityToDecoText(EItemRarity Type)
{
	switch (Type)
	{
	case EItemRarity::Common:
		return "<orb.common>";
	case EItemRarity::Rare:
		return "<orb.rare>";
	case EItemRarity::Epic:
		return "<orb.epic>";
	case EItemRarity::Legendary:
		return "<orb.legendary>";
	default: ;
	}

	return "ERROR";
}

static FORCEINLINE FString SlotTypeToString(EOrbMatrixSlotType SlotType)
{
	switch (SlotType)
	{
	case EOrbMatrixSlotType::CORE:
		return TEXT("코어");
	case EOrbMatrixSlotType::PHYSICAL:
		return TEXT("물리");
	case EOrbMatrixSlotType::DEFENCE:
		return TEXT("방어");
	case EOrbMatrixSlotType::MAGICAL:
		return TEXT("마법");
	case EOrbMatrixSlotType::FREE:
		return TEXT("자유");
	default: ;
	}

	return "ERROR";
}

static FORCEINLINE FString SlotTypeToDecoText(EOrbMatrixSlotType SlotType)
{
	switch (SlotType)
	{
	case EOrbMatrixSlotType::CORE:
		return "<orb.core>";
	case EOrbMatrixSlotType::PHYSICAL:
		return "<orb.physical>";
	case EOrbMatrixSlotType::DEFENCE:
		return "<orb.defence>";
	case EOrbMatrixSlotType::MAGICAL:
		return "<orb.magical>";
	case EOrbMatrixSlotType::FREE:
		return "<orb.free>";
	default: ;
	}

	return "ERROR";
}
