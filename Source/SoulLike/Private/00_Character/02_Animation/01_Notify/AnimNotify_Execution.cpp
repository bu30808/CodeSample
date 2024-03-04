// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/02_Animation/01_Notify/AnimNotify_Execution.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "99_Subsystem/AbilitySubsystem.h"
#include "Kismet/GameplayStatics.h"

UAnimNotify_Execution::UAnimNotify_Execution()
{
	bIsNativeBranchingPoint = true;
}

void UAnimNotify_Execution::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (BranchingPointPayload.SkelMeshComponent)
	{
		if (auto character = Cast<APlayerCharacter>(BranchingPointPayload.SkelMeshComponent->GetOwner()))
		{
			if (auto subsystem = UGameplayStatics::GetGameInstance(character)->GetSubsystem<UAbilitySubsystem>())
			{
				if (auto monster = character->GetExecuteMonster())
				{
					subsystem->AppliedEffectDuringAnimation.Add(
						character, monster->GetAbilityComponent()->K2_ApplyEffectsWithReturn(EffectObjects, character));
				}
			}
		}
	}
}
