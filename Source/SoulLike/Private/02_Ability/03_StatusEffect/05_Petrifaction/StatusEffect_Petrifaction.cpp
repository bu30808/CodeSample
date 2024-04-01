// Fill out your copyright notice in the Description page of Project Settings.


#include "02_Ability/03_StatusEffect/05_Petrifaction/StatusEffect_Petrifaction.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "96_Library/AttributeHelperLibrary.h"
#include "97_Interface/BossMonsterInterface.h"
#include "Kismet/GameplayStatics.h"

UStatusEffect_Petrifaction::UStatusEffect_Petrifaction()
{
	EffectApplyType = EEffectApplyType::Infinite;
	UniqueEffectTag = FGameplayTag::RequestGameplayTag("Common.StatusEffect.Petrifaction.Effect");
}

void UStatusEffect_Petrifaction::ProcessEffect_Implementation(ABaseCharacter* Target, AActor* EffectBy,
                                                              UAbilityBase* From, UObject* AdditionalData)
{
	Target->SetIgnoreMoveInput(true, EffectBy, UniqueEffectTag);
	Target->SetIgnoreLookInput(true, EffectBy, UniqueEffectTag);

	if (auto instance = Target->GetMesh()->GetAnimInstance())
	{
		instance->Montage_Pause();
		Target->GetMesh()->bPauseAnims = true;
	}

	Target->ChangeStatusEffectMaterial(EStatusEffect::PETRIFACTION);

	FTimerHandle killTimerHandle;
	FTimerDelegate killTimerDel = FTimerDelegate::CreateUObject(this, &UStatusEffect_Petrifaction::KillTarget, Target,
	                                                            EffectBy);
	Target->GetWorldTimerManager().SetTimer(killTimerHandle, killTimerDel, 3.f, false);
}

bool UStatusEffect_Petrifaction::CanApplyEffect_Implementation(ABaseCharacter* Target, bool bShowLog) const
{
	return Super::CanApplyEffect_Implementation(Target, bShowLog) && !Target->GetAbilityComponent()->HasEffectTag(
		UniqueEffectTag);
}

void UStatusEffect_Petrifaction::KillTarget(ABaseCharacter* Target, AActor* EffectBy)
{
	/*Target->OnDead.Broadcast(Target,EffectBy);*/
	if (Target->GetCharacterState() != ECharacterState::DEAD)
	{
		Target->SetCharacterState(ECharacterState::DEAD);
		UGameplayStatics::OpenLevel(this, FName(UGameplayStatics::GetCurrentLevelName(Target)));
	}
}

UStatusEffect_PetrifactionAcc::UStatusEffect_PetrifactionAcc()
{
	EffectApplyType = EEffectApplyType::Instant;
	UniqueEffectTag = FGameplayTag::RequestGameplayTag("Common.StatusEffect.Petrifaction.Acc.Effect");
	CheckStatusEffectTag = FGameplayTag::RequestGameplayTag("Common.StatusEffect.Petrifaction.Effect");
}

bool UStatusEffect_PetrifactionAcc::ApplyInstantEffect_Implementation(ABaseCharacter* Target, UObject* AdditionalInfo,
                                                                      float DeltaTime)
{
	if (UKismetSystemLibrary::DoesImplementInterface(Target, UBossMonsterInterface::StaticClass()))
	{
		EndEffect(Target);
		return false;
	}

	bool result = Super::ApplyInstantEffect_Implementation(Target, AdditionalInfo, DeltaTime);

	if (auto attComp = Target->GetAttributeComponent())
	{
		for (auto iter : AttributeEffects)
		{
			EStatusEffect statusEffect;
			float accValue;
			float resistValue;
			UAttributeHelperLibrary::AttributeEffectElementToStatusEffectValues(
				attComp, iter.Attribute, statusEffect, accValue, resistValue);

			attComp->OnUpdateStatusEffect.Broadcast(statusEffect, accValue, resistValue);
		}
	}

	return result;
}
