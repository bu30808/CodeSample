// Fill out your copyright notice in the Description page of Project Settings.


#include "02_Ability/03_StatusEffect/StatusEffect_TickBase.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "97_Interface/BossMonsterInterface.h"

UStatusEffect_TickBase::UStatusEffect_TickBase()
{
	EffectApplyType = EEffectApplyType::InfiniteWithTick;
}

void UStatusEffect_TickBase::OverrideAttributeEffects_Implementation(ABaseCharacter* Target, AActor* EffectBy)
{
	ResultAmount = Amount;
	if(UKismetSystemLibrary::DoesImplementInterface(Target,UBossMonsterInterface::StaticClass()))
	{
		ResultAmount/= 3.f;
	}

	
	if(auto attComp = Target->GetAttributeComponent())
	{
		TArray<FAttributeEffect> newEffects;
		newEffects.Add(FAttributeEffect(EAttributeType::HP,EAttributeApplyMethod::REMOVE,attComp->GetMaxHP() * ResultAmount));

		AttributeEffects = newEffects;
	}
}
