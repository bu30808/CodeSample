// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/00_Task/BTTask_ClearFocus.h"

#include "AIController.h"
#include "00_Character/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_ClearFocus::UBTTask_ClearFocus()
{
	bNotifyTick = false;
	NodeName = TEXT("포커스 제거");
	BlackboardKey.SelectedKeyName = "Target";
}

EBTNodeResult::Type UBTTask_ClearFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto aiCon = OwnerComp.GetAIOwner())
	{
		if (auto pawn = aiCon->GetPawn<ABaseCharacter>())
		{
			pawn->bUseControllerRotationYaw = false;
			pawn->GetCharacterMovement()->bOrientRotationToMovement = true;
			pawn->bModifySkeletonTransform = false;
			aiCon->K2_ClearFocus();
			return EBTNodeResult::Succeeded;
		}
	}


	return EBTNodeResult::Failed;
}

FString UBTTask_ClearFocus::GetStaticDescription() const
{
	return TEXT("타겟을 그만 바라보게 합니다.");
}
