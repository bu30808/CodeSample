// Fill out your copyright notice in the Description page of Project Settings.


#include "02_Ability/03_StatusEffect/02_Bleed/StatusEffect_Bleed.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "97_Interface/BossMonsterInterface.h"

UStatusEffect_Bleed::UStatusEffect_Bleed()
{
	EffectApplyType = EEffectApplyType::DurationWithTick;
	Time = 5.f;

	AttributeEffects.Add(FAttributeEffect(EAttributeType::BleedAcc, EAttributeApplyMethod::REMOVE,INT_MAX));
	UniqueEffectTag = FGameplayTag::RequestGameplayTag("Common.StatusEffect.Bleed.Effect");
}

void UStatusEffect_Bleed::ProcessEffect_Implementation(ABaseCharacter* Target, AActor* EffectBy, UAbilityBase* From, UObject* AdditionalData)
{
	if (auto abComp = Target->GetAbilityComponent())
	{
		abComp->K2_ApplyEffect(BleedInstantObject, EffectBy, FOnEffectExpired(), AdditionalData);
		abComp->K2_ApplyEffect(BleedDurationObject, EffectBy, FOnEffectExpired(), AdditionalData);

		ApplyInstantEffect(Target);
	}
}

void UStatusEffect_Bleed::EndEffect_Implementation(ABaseCharacter* Target)
{
	if (auto attComp = Target->GetAttributeComponent())
	{
		attComp->OnUpdateStatusEffect.Broadcast(EStatusEffect::BLEED, attComp->GetBleedAcc(),
		                                        attComp->GetBleedResist());
	}

	Super::EndEffect_Implementation(Target);
}

UStatusEffect_BleedAcc::UStatusEffect_BleedAcc()
{
	UniqueEffectTag = FGameplayTag::RequestGameplayTag("Common.StatusEffect.Bleed.Acc.Effect");
	CheckStatusEffectTag = FGameplayTag::RequestGameplayTag("Common.StatusEffect.Bleed.Effect");

	AttributeEffects.Add(FAttributeEffect(EAttributeType::BleedAcc, EAttributeApplyMethod::ADD, 0));
}

UStatusEffect_BleedInstant::UStatusEffect_BleedInstant()
{
	EffectApplyType = EEffectApplyType::Instant;
	UniqueEffectTag = FGameplayTag::RequestGameplayTag("Common.StatusEffect.Bleed.Instant.Effect");


	static ConstructorHelpers::FClassFinder<AAbilityCue> cueActor(TEXT(
		"/Script/Engine.Blueprint'/Game/Blueprints/00_Character/02_CommonAbility/StatusEffect/Bleed/AC_BleedInstant.AC_BleedInstant_C'"));
	if (cueActor.Succeeded())
	{
		FAbilityCueInformation cue;
		cue.CueTag = FGameplayTag::RequestGameplayTag("Cue.Common.StatusEffect.Bleed.Instant.Effect");
		cue.bReusable = true;
		cue.AbilityCueObject = cueActor.Class;
		cue.AbilityCueType = EAbilityCueType::AttachToTarget;
		InfiniteAbilityCues.Add(cue);
	}
}

void UStatusEffect_BleedInstant::OverrideAttributeEffects_Implementation(ABaseCharacter* Target, AActor* EffectBy)
{
	if (auto attComp = Target->GetAttributeComponent())
	{
		float resultAmount = Amount;
		if (UKismetSystemLibrary::DoesImplementInterface(Target, UBossMonsterInterface::StaticClass()))
		{
			resultAmount = Amount / 3.f;
		}
		AttributeEffects.Empty();
		AttributeEffects.Add(FAttributeEffect(EAttributeType::HP, EAttributeApplyMethod::REMOVE,
		                                      attComp->GetMaxHP() * resultAmount));
	}
}

UStatusEffect_BleedDuration::UStatusEffect_BleedDuration()
{
	EffectApplyType = EEffectApplyType::DurationWithInterval;
	Time = 5.f;
	IntervalTime = 1.f;

	UniqueEffectTag = FGameplayTag::RequestGameplayTag("Common.StatusEffect.Bleed.Duration.Effect");

	static ConstructorHelpers::FClassFinder<AAbilityCue> cueActor(TEXT(
		"/Script/Engine.Blueprint'/Game/Blueprints/00_Character/02_CommonAbility/StatusEffect/Bleed/AC_BleedDuration.AC_BleedDuration_C'"));
	if (cueActor.Succeeded())
	{
		FAbilityCueInformation cue;
		cue.CueTag = FGameplayTag::RequestGameplayTag("Cue.Common.StatusEffect.Bleed.Duration.Effect");
		cue.bReusable = true;
		cue.AbilityCueObject = cueActor.Class;
		cue.AbilityCueType = EAbilityCueType::AttachToTarget;
		IntervalAbilityCues.Add(cue);
	}
}

void UStatusEffect_BleedDuration::OverrideAttributeEffects_Implementation(ABaseCharacter* Target, AActor* EffectBy)
{
	auto ResultAmount = Amount;
	if (UKismetSystemLibrary::DoesImplementInterface(Target, UBossMonsterInterface::StaticClass()))
	{
		ResultAmount /= 3.f;
	}


	if (auto attComp = Target->GetAttributeComponent())
	{
		TArray<FAttributeEffect> newEffects;
		newEffects.Add(FAttributeEffect(EAttributeType::HP, EAttributeApplyMethod::REMOVE,
		                                attComp->GetMaxHP() * ResultAmount));

		AttributeEffects = newEffects;
	}
}
