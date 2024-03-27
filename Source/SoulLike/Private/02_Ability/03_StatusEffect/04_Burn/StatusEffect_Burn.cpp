// Fill out your copyright notice in the Description page of Project Settings.


#include "02_Ability/03_StatusEffect/04_Burn/StatusEffect_Burn.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AttributeComponent.h"

UStatusEffect_Burn::UStatusEffect_Burn()
{
	Amount = 0.004f;
	UniqueEffectTag = FGameplayTag::RequestGameplayTag("Common.StatusEffect.Burn.Effect");

	{
		static ConstructorHelpers::FClassFinder<AAbilityCue> cueActor(TEXT(
	"/Script/Engine.Blueprint'/Game/Blueprints/00_Character/02_CommonAbility/StatusEffect/Burn/AC_Burn.AC_Burn_C'"));
		if (cueActor.Succeeded())
		{
			FAbilityCueInformation cue;
			cue.CueTag = FGameplayTag::RequestGameplayTag("Cue.Common.StatusEffect.Burn.Effect");
			cue.bReusable = true;
			cue.AbilityCueObject = cueActor.Class;
			cue.AbilityCueType = EAbilityCueType::AttachToTarget;
			InfiniteAbilityCues.Add(cue);
		}
	
	}
}

UStatusEffect_BurnAcc::UStatusEffect_BurnAcc()
{
	UniqueEffectTag = FGameplayTag::RequestGameplayTag("Common.StatusEffect.Burn.Acc.Effect");
	CheckStatusEffectTag = FGameplayTag::RequestGameplayTag("Common.StatusEffect.Burn.Effect");

	AttributeEffects.Add(FAttributeEffect(EAttributeType::BurnAcc,EAttributeApplyMethod::ADD,0));
}

UStatusEffect_DodgeBurnDecrease::UStatusEffect_DodgeBurnDecrease()
{
	EffectApplyType = EEffectApplyType::Instant;
	AttributeEffects.Add(FAttributeEffect(EAttributeType::BurnAcc,EAttributeApplyMethod::REMOVE,20));
	UniqueEffectTag = FGameplayTag::RequestGameplayTag("Common.StatusEffect.Burn.Reduce.Effect");
}

void UStatusEffect_DodgeBurnDecrease::ProcessEffect_Implementation(ABaseCharacter* Target, AActor* EffectBy,
	UAbilityBase* From, UObject* AdditionalData)
{
	Super::ProcessEffect_Implementation(Target, EffectBy, From, AdditionalData);

	if(auto attComp = Target->GetAttributeComponent())
	{
		attComp->OnUpdateStatusEffect.Broadcast(EStatusEffect::BURN,attComp->GetBurnAcc(),attComp->GetBurnResist());
	}
}
