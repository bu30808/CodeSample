// Fill out your copyright notice in the Description page of Project Settings.


#include "96_Library/AttributeHelperLibrary.h"

#include "00_Character/01_Component/AttributeComponent.h"
#include "02_Ability/AbilityEffect.h"
#include "03_Widget/03_LevelUp/AttributePreviewWidget.h"
#include "Kismet/GameplayStatics.h"
#include "99_Subsystem/AttributeProcessSubsystem.h"
#include "Logging/StructuredLog.h"

EAttributeType UAttributeHelperLibrary::ConvertAttributePointTypeToAttributeType(EAttributePointType PointType)
{
	switch (PointType)
	{
	case EAttributePointType::LEVEL:
		return EAttributeType::Level;
	case EAttributePointType::EXP:
		return EAttributeType::EXP;
	case EAttributePointType::MAXHP:
		return EAttributeType::MaxHP;
	case EAttributePointType::MAXSP:
		return EAttributeType::MaxSP;
	case EAttributePointType::MAXMP:
		return EAttributeType::MaxMP;
	case EAttributePointType::STR:
		return EAttributeType::Strength;
	case EAttributePointType::DEX:
		return EAttributeType::Dexterity;
	case EAttributePointType::INT:
		return EAttributeType::Intelligence;
	case EAttributePointType::WILL:
		return EAttributeType::Willpower;
	case EAttributePointType::MAX:
		return EAttributeType::MAX;
	default: ;
	}

	return EAttributeType::NONE;
}

UAttributeProcessSubsystem* UAttributeHelperLibrary::GetAttributeProcessSubsystem(UObject* WorldContext)
{
	return UGameplayStatics::GetGameInstance(WorldContext)->GetSubsystem<UAttributeProcessSubsystem>();
}

EStatusEffect UAttributeHelperLibrary::AttributeTypeToStatusEffect(EAttributeType Type)
{
	switch (Type) {
	case EAttributeType::PoisonAcc:
		return EStatusEffect::POISON;
	case EAttributeType::DeadlyPoisonAcc:
		return EStatusEffect::DEADLY_POISON;
	case EAttributeType::BurnAcc:
		return EStatusEffect::BURN;
	case EAttributeType::ChillAcc:
		return EStatusEffect::CHILL;
	case EAttributeType::BleedAcc:
		return EStatusEffect::BLEED;
	case EAttributeType::PetrifactionAcc:
		return EStatusEffect::PETRIFACTION;
	default: ;
	}
	
	return EStatusEffect::MAX;
}

void UAttributeHelperLibrary::AttributeEffectElementToStatusEffectValues(UAttributeComponent* AttributeComponent,
                                                                         EAttributeType AttributeType, EStatusEffect& StatusEffect, float& TotalAccValue, float& ResistValue)
{
	StatusEffect = AttributeTypeToStatusEffect(AttributeType);

	switch (StatusEffect) {
	case EStatusEffect::POISON:
		TotalAccValue = AttributeComponent->GetPoisonAcc();
		ResistValue = AttributeComponent->GetPoisonResist();
		break;
	case EStatusEffect::DEADLY_POISON:
		TotalAccValue = AttributeComponent->GetDeadlyPoisonAcc();
		ResistValue = AttributeComponent->GetDeadlyPoisonResist();
		break;
	case EStatusEffect::BURN:
		TotalAccValue = AttributeComponent->GetBurnAcc();
		ResistValue = AttributeComponent->GetBurnResist();
		break;
	case EStatusEffect::CHILL:
		TotalAccValue = AttributeComponent->GetChillAcc();
		ResistValue = AttributeComponent->GetChillResist();
		break;
	case EStatusEffect::BLEED:
		TotalAccValue = AttributeComponent->GetBleedAcc();
		ResistValue = AttributeComponent->GetBleedResist();
		break;
	case EStatusEffect::PETRIFACTION:
		TotalAccValue = AttributeComponent->GetPetrifactionAcc();
		ResistValue = AttributeComponent->GetPetrifactionResist();
		break;
	case EStatusEffect::MAX:
		UE_LOGFMT(LogEffect,Error,"상태이상 이넘값으로 변환 실패함");
		break;
	default: ;
	}
}
