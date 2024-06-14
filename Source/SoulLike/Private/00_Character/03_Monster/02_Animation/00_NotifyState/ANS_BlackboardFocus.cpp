// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/02_Animation/00_NotifyState/ANS_BlackboardFocus.h"

#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "96_Library/AbilityHelperLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

UANS_BlackboardFocus::UANS_BlackboardFocus()
{
#if WITH_EDITOR
	bShouldFireInEditor = false;
#endif
}

void UANS_BlackboardFocus::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if(MeshComp)
	{
		if(auto npc = Cast<ABaseCharacter>(MeshComp->GetOwner()))
		{
			if(auto target = UAbilityHelperLibrary::GetTargetFromBlackboard(npc))
			{
				if(auto aiCon = Cast<AMonsterAIController>(npc->GetController()))
				{
					aiCon->SetSmoothFocusInterpSpeed(SmoothFocusInterpSpeed);
					npc->bUseControllerRotationYaw = bUseControllerRotationYaw;
					npc->GetCharacterMovement()->bUseControllerDesiredRotation = bUseControllerRotationYaw;
					npc->GetCharacterMovement()->bOrientRotationToMovement = !bUseControllerRotationYaw;
					aiCon->SetFocus(target);
				}
			}
		}
	}
	
}

void UANS_BlackboardFocus::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if(MeshComp)
	{
		if(auto npc = Cast<ABaseCharacter>(MeshComp->GetOwner()))
		{
			if(auto aiCon = Cast<AMonsterAIController>(npc->GetController()))
			{
				npc->bUseControllerRotationYaw = false;
				npc->GetCharacterMovement()->bOrientRotationToMovement = true;
				aiCon->K2_ClearFocus();
			}
		}
	}
}

