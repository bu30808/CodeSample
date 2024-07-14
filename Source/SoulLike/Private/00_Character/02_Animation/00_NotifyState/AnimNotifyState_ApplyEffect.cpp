// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/02_Animation/00_NotifyState/AnimNotifyState_ApplyEffect.h"

#include "99_Subsystem/AbilitySubsystem.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

UAnimNotifyState_ApplyEffect::UAnimNotifyState_ApplyEffect()
{
#if WITH_EDITOR
	bShouldFireInEditor = false;
#endif
}


void UAnimNotifyState_ApplyEffect::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                               float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp)
	{
		if (auto character = Cast<ABaseCharacter>(MeshComp->GetOwner()))
		{
			if (auto subsystem = UGameplayStatics::GetGameInstance(character)->GetSubsystem<UAbilitySubsystem>())
			{
				subsystem->AppliedEffectDuringAnimation.Add(character, FAnimationNotifyEffects(character->GetAbilityComponent()->K2_ApplyEffectsWithReturn(EffectObjects, character)));
			}
		}
	}
}

void UAnimNotifyState_ApplyEffect::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		if (auto character = Cast<ABaseCharacter>(MeshComp->GetOwner()))
		{
			if (auto subsystem = UGameplayStatics::GetGameInstance(character)->GetSubsystem<UAbilitySubsystem>())
			{
				if (subsystem->AppliedEffectDuringAnimation.Contains(character))
				{
					character->GetAbilityComponent()->EndEffects(subsystem->AppliedEffectDuringAnimation[character].Effects);
					subsystem->AppliedEffectDuringAnimation.Remove(character);
				}
			}
		}
	}
}
