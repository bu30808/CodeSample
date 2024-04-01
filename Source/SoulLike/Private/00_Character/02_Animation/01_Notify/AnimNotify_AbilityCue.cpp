// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/02_Animation/01_Notify/AnimNotify_AbilityCue.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "Logging/StructuredLog.h"

UAnimNotify_AbilityCue::UAnimNotify_AbilityCue()
{
	bIsNativeBranchingPoint = false;
#if WITH_EDITOR
	bShouldFireInEditor = false;
#endif
}

void UAnimNotify_AbilityCue::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                    const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp->GetOwner())
	{
		if (const auto character = MeshComp->GetOwner<ABaseCharacter>())
		{
			CueInformation.AttachTarget = character;
			CueInformation.SpawnLocation = character->GetActorLocation();
			character->GetAbilityComponent()->ApplyCue(CueInformation);
		}
	}
}

FString UAnimNotify_AbilityCue::GetEditorComment()
{
	return TEXT("어빌리티 큐 생성");
}
