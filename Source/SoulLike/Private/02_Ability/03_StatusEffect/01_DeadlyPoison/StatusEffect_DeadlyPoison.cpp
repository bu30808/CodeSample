// Fill out your copyright notice in the Description page of Project Settings.


#include "02_Ability/03_StatusEffect/01_DeadlyPoison/StatusEffect_DeadlyPoison.h"

#include "00_Character/BaseCharacter.h"
#include "97_Interface/BossMonsterInterface.h"
#include "Kismet/KismetSystemLibrary.h"

UStatusEffect_DeadlyPoison::UStatusEffect_DeadlyPoison()
{
	Amount = 0.03f;
	UniqueEffectTag = FGameplayTag::RequestGameplayTag("Common.StatusEffect.DeadlyPoison.Effect");

	static ConstructorHelpers::FClassFinder<AAbilityCue> cueActor(TEXT(
		"/Script/Engine.Blueprint'/Game/Blueprints/00_Character/02_CommonAbility/StatusEffect/DeadlyPoison/AC_DeadlyPoison.AC_DeadlyPoison_C'"));
	if (cueActor.Succeeded())
	{
		FAbilityCueInformation cue;
		cue.CueTag = FGameplayTag::RequestGameplayTag("Cue.Common.StatusEffect.DeadlyPoison.Effect");
		cue.AbilityCueObject = cueActor.Class;
		cue.AbilityCueType = EAbilityCueType::AttachToTarget;
		InfiniteAbilityCues.Add(cue);
	}
}

UStatusEffect_DeadlyPoisonAcc::UStatusEffect_DeadlyPoisonAcc()
{
	UniqueEffectTag = FGameplayTag::RequestGameplayTag("Common.StatusEffect.DeadlyPoison.Acc.Effect");
	CheckStatusEffectTag = FGameplayTag::RequestGameplayTag("Common.StatusEffect.DeadlyPoison.Effect");

	AttributeEffects.Add(FAttributeEffect(EAttributeType::DeadlyPoisonAcc, EAttributeApplyMethod::ADD, 0));
}
