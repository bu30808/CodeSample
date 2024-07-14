// Fill out your copyright notice in the Description page of Project Settings.


#include "99_Subsystem/AttributeProcessSubsystem.h"


#include "00_Character/01_Component/AttributeComponent.h"
#include "02_Ability/AbilityEffect.h"
#include "02_Ability/00_AttributeProcessor/AttributeProcessor.h"
#include "Logging/StructuredLog.h"

UAttributeProcessSubsystem::UAttributeProcessSubsystem()
{
}

void UAttributeProcessSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);


	Processor.Emplace(EAttributeApplyMethod::NONE, nullptr);
	Processor.Emplace(EAttributeApplyMethod::ADD, NewObject<UAttributeProcessor_Add>(this));
	Processor.Emplace(EAttributeApplyMethod::REMOVE, NewObject<UAttributeProcessor_Remove>(this));
	Processor.Emplace(EAttributeApplyMethod::MULT, NewObject<UAttributeProcessor_Multiplication>(this));
	Processor.Emplace(EAttributeApplyMethod::DIV, NewObject<UAttributeProcessor_Division>(this));
	Processor.Emplace(EAttributeApplyMethod::SET, NewObject<UAttributeProcessor_Set>(this));
}

UAttributeProcessor* UAttributeProcessSubsystem::GetProcessor(EAttributeApplyMethod Method)
{
	if (Processor.Contains(Method))
	{
		return Processor[Method];
	}
	UE_LOGFMT(LogEffect, Error, "프로세서가 존재하지 않습니다.");
	return nullptr;
}

UAttributeProcessor* UAttributeProcessSubsystem::GetReverseProcessor(EAttributeApplyMethod Method)
{
	switch (Method)
	{
	case EAttributeApplyMethod::NONE:
		break;
	case EAttributeApplyMethod::ADD:
		return Processor[EAttributeApplyMethod::REMOVE];
	case EAttributeApplyMethod::REMOVE:
		return Processor[EAttributeApplyMethod::ADD];
	case EAttributeApplyMethod::MULT:
		return Processor[EAttributeApplyMethod::DIV];
	case EAttributeApplyMethod::DIV:
		return Processor[EAttributeApplyMethod::MULT];
	default: ;
	}

	UE_LOGFMT(LogEffect, Error, "프로세서가 존재하지 않습니다.");
	return nullptr;
}


TArray<FAttributeEffect> UAttributeProcessSubsystem::UpdateAttributeEffectsAffectedByOwnersAttribute(
	const UAttributeComponent* AttributeComponent, TArray<FAttributeEffect> AttributeEffects, EAttackType AttackType)
{
	if (AttackType == EAttackType::None)
	{
		for (auto& effect : AttributeEffects)
		{
			switch (effect.Attribute)
			{
			case EAttributeType::NONE:

				break;
			case EAttributeType::MaxHP:
				effect.ApplyValue += AttributeComponent->GetMaxHP();
				break;
			case EAttributeType::MaxSP:
				effect.ApplyValue += AttributeComponent->GetMaxSP();
				break;
			case EAttributeType::MaxMP:
				effect.ApplyValue += AttributeComponent->GetMaxHP();
				break;
			case EAttributeType::HP:
			case EAttributeType::RecoverHP:
				effect.ApplyValue += AttributeComponent->GetRecoverHP();
				break;
			case EAttributeType::SP:
			case EAttributeType::RecoverSP:
				effect.ApplyValue += AttributeComponent->GetRecoverSP();
				break;
			case EAttributeType::MP:
			case EAttributeType::RecoverMP:
				effect.ApplyValue += AttributeComponent->GetRecoverHitMP();
				break;
			case EAttributeType::PhysicalAttack:
				effect.ApplyValue += AttributeComponent->GetPhysicalAttack();
				break;
			case EAttributeType::MagicalAttack:
				effect.ApplyValue += AttributeComponent->GetMagicalAttack();
				break;
			case EAttributeType::PhysicalDefense:
				effect.ApplyValue += AttributeComponent->GetPhysicalDefense();
				break;
			case EAttributeType::MagicalDefense:
				effect.ApplyValue += AttributeComponent->GetMagicalDefense();
				break;
			case EAttributeType::ActionSpeed:
				effect.ApplyValue += AttributeComponent->GetActionSpeed();
				break;
			default: ;
			}
		}
	}
	else
	{
		for (auto& effect : AttributeEffects)
		{
			if (effect.Attribute == EAttributeType::HP)
			{
				switch (AttackType)
				{
				case EAttackType::Physical:
					UE_LOGFMT(LogTemp, Log, "{0}의 물리 피해량 갱신 : {1} ", GetNameSafe(AttributeComponent->GetOwner()),
					          AttributeComponent->GetPhysicalAttack());
					effect.ApplyValue += AttributeComponent->GetPhysicalAttack();
					break;
				case EAttackType::Magical:
					UE_LOGFMT(LogTemp, Log, "{0}의 마법 피해량 갱신 : {1} ", GetNameSafe(AttributeComponent->GetOwner()),
					          AttributeComponent->GetMagicalAttack());
					effect.ApplyValue += AttributeComponent->GetMagicalAttack();
					break;
				default: ;
				}
			}
		}
	}

	return AttributeEffects;
}
