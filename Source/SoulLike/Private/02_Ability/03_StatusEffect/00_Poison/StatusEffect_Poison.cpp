// Fill out your copyright notice in the Description page of Project Settings.


#include "02_Ability/03_StatusEffect/00_Poison/StatusEffect_Poison.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "97_Interface/BossMonsterInterface.h"
#include "Kismet/KismetSystemLibrary.h"

UStatusEffect_Poison::UStatusEffect_Poison()
{
	Amount = 0.01f;
	
	/*EffectApplyType = EEffectApplyType::InfiniteWithTick;*/
	UniqueEffectTag = FGameplayTag::RequestGameplayTag("Common.StatusEffect.Poison.Effect");

	static ConstructorHelpers::FClassFinder<AAbilityCue> cueActor(TEXT(
	"/Script/Engine.Blueprint'/Game/Blueprints/00_Character/02_CommonAbility/StatusEffect/Poison/AC_Poison.AC_Poison_C'"));
	if (cueActor.Succeeded())
	{
		FAbilityCueInformation cue;
		cue.CueTag = FGameplayTag::RequestGameplayTag("Cue.Common.StatusEffect.Poison.Effect");
		cue.AbilityCueObject = cueActor.Class;
		cue.AbilityCueType = EAbilityCueType::AttachToTarget;
		InfiniteAbilityCues.Add(cue);
	}
}


UStatusEffect_PoisonAcc::UStatusEffect_PoisonAcc()
{
	/*EffectApplyType = EEffectApplyType::InfiniteWithTick;*/
	UniqueEffectTag = FGameplayTag::RequestGameplayTag("Common.StatusEffect.Poison.Acc.Effect");
	CheckStatusEffectTag = FGameplayTag::RequestGameplayTag("Common.StatusEffect.Poison.Effect");

	AttributeEffects.Add(FAttributeEffect(EAttributeType::PoisonAcc,EAttributeApplyMethod::ADD,0));
}

