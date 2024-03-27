// Fill out your copyright notice in the Description page of Project Settings.


#include "02_Ability/03_StatusEffect/03_ChillAndFreeze/StatusEffect_Chill.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "97_Interface/BossMonsterInterface.h"

UStatusEffect_Chill::UStatusEffect_Chill()
{
	EffectApplyType = EEffectApplyType::Infinite;
	bRestoreAttributeOnEnd = true;

	UniqueEffectTag = FGameplayTag::RequestGameplayTag("Common.StatusEffect.Chill.Effect");
	static ConstructorHelpers::FClassFinder<AAbilityCue> cueActor(TEXT(
	"/Script/Engine.Blueprint'/Game/Blueprints/00_Character/02_CommonAbility/StatusEffect/ChillAndFreeze/AC_Chill.AC_Chill_C'"));
	if (cueActor.Succeeded())
	{
		FAbilityCueInformation cue;
		cue.CueTag = FGameplayTag::RequestGameplayTag("Cue.Common.StatusEffect.Chill.Effect");
		cue.bReusable = true;
		cue.AbilityCueObject = cueActor.Class;
		cue.AbilityCueType = EAbilityCueType::AttachToTarget;
		InfiniteAbilityCues.Add(cue);
	}
	
}

void UStatusEffect_Chill::OverrideAttributeEffects_Implementation(ABaseCharacter* Target, AActor* EffectBy)
{
	if(auto attComp = Target->GetAttributeComponent())
	{
		float resultAmount = SlowRate;
		if(UKismetSystemLibrary::DoesImplementInterface(Target,UBossMonsterInterface::StaticClass()))
		{
			resultAmount = SlowRate / 3.f;
		}
		TArray<FAttributeEffect> newEffects;

		newEffects.Add(FAttributeEffect(EAttributeType::ActionSpeed,EAttributeApplyMethod::REMOVE,attComp->GetActionSpeed() * resultAmount));
		newEffects.Add(FAttributeEffect(EAttributeType::MoveSpeed,EAttributeApplyMethod::REMOVE,attComp->GetMoveSpeed() * resultAmount));

		AttributeEffects = newEffects;
	}
}

void UStatusEffect_Chill::RestoreAttribute_Implementation(ABaseCharacter* Target)
{
	Super::RestoreAttribute_Implementation(Target);
	Target->GetMesh()->SetPlayRate(OriginalPlayRate);
}

bool UStatusEffect_Chill::ApplyInstantEffect_Implementation(ABaseCharacter* Target, UObject* AdditionalInfo,
	float DeltaTime)
{
	bool result =  Super::ApplyInstantEffect_Implementation(Target, AdditionalInfo, DeltaTime);

	OriginalPlayRate = Target->GetMesh()->GetPlayRate();
	Target->GetMesh()->SetPlayRate(OriginalPlayRate - SlowRate);
	
	
	return result;
}

UStatusEffect_ChillAcc::UStatusEffect_ChillAcc()
{
	UniqueEffectTag = FGameplayTag::RequestGameplayTag("Common.StatusEffect.Chill.Acc.Effect");
	CheckStatusEffectTag = FGameplayTag::RequestGameplayTag("Common.StatusEffect.Freeze.Effect");

	AttributeEffects.Add(FAttributeEffect(EAttributeType::ChillAcc,EAttributeApplyMethod::ADD,0));
}

UStatusEffect_Freeze::UStatusEffect_Freeze()
{
	EffectApplyType = EEffectApplyType::Duration;
	Time = 3.f;
	AttributeEffects.Add(FAttributeEffect(EAttributeType::ChillAcc,EAttributeApplyMethod::REMOVE,INT_MAX));
	UniqueEffectTag = FGameplayTag::RequestGameplayTag("Common.StatusEffect.Freeze.Effect");

	{
		static ConstructorHelpers::FClassFinder<AAbilityCue> cueActor(TEXT(
	"/Script/Engine.Blueprint'/Game/Blueprints/00_Character/02_CommonAbility/StatusEffect/ChillAndFreeze/AC_Freeze.AC_Freeze_C'"));
		if (cueActor.Succeeded())
		{
			FAbilityCueInformation cue;
			cue.CueTag = FGameplayTag::RequestGameplayTag("Cue.Common.StatusEffect.Freeze.Effect");
			cue.bReusable = true;
			cue.AbilityCueObject = cueActor.Class;
			cue.AbilityCueType = EAbilityCueType::AttachToTarget;
			InfiniteAbilityCues.Add(cue);
		}
	
	}

	{
		static ConstructorHelpers::FClassFinder<AAbilityCue> cueActor(TEXT(
	"/Script/Engine.Blueprint'/Game/Blueprints/00_Character/02_CommonAbility/StatusEffect/ChillAndFreeze/AC_Freeze_Out.AC_Freeze_Out_C'"));
		if (cueActor.Succeeded())
		{
			FAbilityCueInformation cue;
			cue.CueTag = FGameplayTag::RequestGameplayTag("Cue.Common.StatusEffect.Freeze.End.Effect");
			cue.bReusable = true;
			cue.AbilityCueObject = cueActor.Class;
			cue.AbilityCueType = EAbilityCueType::AttachToTarget;
			InstanceAbilityCues.Add(cue);
		}
	
	}
}

void UStatusEffect_Freeze::ProcessEffect_Implementation(ABaseCharacter* Target, AActor* EffectBy, UAbilityBase* From, UObject* AdditionalData)
{
	Super::ProcessEffect_Implementation(Target, EffectBy, From, AdditionalData);


	AccruedBy = EffectBy;
	
	Target->SetIgnoreMoveInput(true,EffectBy,FGameplayTag::RequestGameplayTag("Common.StatusEffect.Freeze.Effect"));
	if(auto instance = Target->GetMesh()->GetAnimInstance())
	{
		instance->Montage_Pause();
		Target->GetMesh()->bPauseAnims = true;
	}
	Target->ChangeStatusEffectMaterial(EStatusEffect::FREEZE);
}

void UStatusEffect_Freeze::EndEffect_Implementation(ABaseCharacter* Target)
{
	Target->SetIgnoreMoveInput(false,AccruedBy,FGameplayTag::RequestGameplayTag("Common.StatusEffect.Freeze.Effect"));
	if(auto instance = Target->GetMesh()->GetAnimInstance())
	{
		instance->Montage_Resume(nullptr);
		Target->GetMesh()->bPauseAnims = false;
	}
	if(auto attComp = Target->GetAttributeComponent())
	{
		attComp->OnUpdateStatusEffect.Broadcast(EStatusEffect::CHILL,attComp->GetChillAcc(),attComp->GetChillResist());
	}

	Target->RestoreStatusEffectMaterial();
	
	Super::EndEffect_Implementation(Target);
}

void UStatusEffect_Freeze::OverrideTime_Implementation(ABaseCharacter* Target)
{
	Super::OverrideTime_Implementation(Target);
}
