// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/00_Task/BTTask_SetFocus.h"

#include "AIController.h"
#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AnimationHelperComponent.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "00_Character/03_Monster/02_Animation/MonsterAnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


UBTTask_SetFocus::UBTTask_SetFocus()
{
	bNotifyTick = false;
	NodeName = TEXT("포커스 설정");
	BlackboardKey.SelectedKeyName = "Target";
}

EBTNodeResult::Type UBTTask_SetFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto aiCon = OwnerComp.GetAIOwner())
	{
		if (auto pawn = aiCon->GetPawn<ABaseCharacter>())
		{
			Cast<AMonsterAIController>(aiCon)->SetSmoothFocusInterpSpeed(SmoothFocusInterpSpeed);
			pawn->bUseControllerRotationYaw = bUseControllerRotationYaw;
			pawn->GetCharacterMovement()->bUseControllerDesiredRotation = bUseControllerRotationYaw;
			pawn->GetCharacterMovement()->bOrientRotationToMovement = !bUseControllerRotationYaw;

			aiCon->SetActorTickEnabled(true);
			
			switch (AIFocusType)
			{
			case EAIFocusType::Target:
				if (auto target = OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()))
				{
					aiCon->SetFocus(Cast<AActor>(target));
				}
				break;
			case EAIFocusType::Location:
				aiCon->SetFocalPoint(OwnerComp.GetBlackboardComponent()->GetValueAsVector(GetSelectedBlackboardKey()));
				break;
			default: ;
			}

			if (bModifySkeletonTransform)
			{
				pawn->GetAnimationHelperComponent()->bModifySkeletonTransform = bModifySkeletonTransform;
			}

			return EBTNodeResult::Succeeded;
		}
	}


	return EBTNodeResult::Failed;
}

FString UBTTask_SetFocus::GetStaticDescription() const
{
	return TEXT("타겟을 계속 바라보고 회전하도록 합니다.");
}
