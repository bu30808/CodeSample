// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/02_Animation/01_Notify/AnimNotify_ApplyEffect.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "Kismet/GameplayStatics.h"

class UAbilitySubsystem;

UAnimNotify_ApplyEffect::UAnimNotify_ApplyEffect()
{
	bIsNativeBranchingPoint = true;
}

void UAnimNotify_ApplyEffect::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotify(BranchingPointPayload);

	if (BranchingPointPayload.SkelMeshComponent)
	{
		if (auto character = Cast<ABaseCharacter>(BranchingPointPayload.SkelMeshComponent->GetOwner()))
		{
			if (auto subsystem = UGameplayStatics::GetGameInstance(character)->GetSubsystem<UAbilitySubsystem>())
			{
				subsystem->AppliedEffectDuringAnimation.Add(
					character, character->GetAbilityComponent()->K2_ApplyEffectsWithReturn(EffectObjects, character));
			}
		}
	}
}
