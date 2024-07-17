// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/00_Task/BTTask_ChangeMovementState.h"

#include "AIController.h"
#include "00_Character/03_Monster/BaseMonster.h"

UBTTask_ChangeMovementState::UBTTask_ChangeMovementState()
{
	NodeName = TEXT("걷거나 뛰거나");
}

EBTNodeResult::Type UBTTask_ChangeMovementState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto character = OwnerComp.GetAIOwner()->GetPawn<ABaseCharacter>())
	{
		if (!bChangeSpeedMultiplier)
		{
			SpeedMultiplier = 1.f;
		}
		character->ChangeMovementState(NewMovementState, SpeedMultiplier);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

void UBTTask_ChangeMovementState::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                                 EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	
}

FString UBTTask_ChangeMovementState::GetStaticDescription() const
{
	switch (NewMovementState)
	{
	case EMovementState::Walk:
		return TEXT("걷기로 변경합니다.");
	case EMovementState::Run:
		return TEXT("뛰기로 변경합니다.");
	default: ;
	}
	return Super::GetStaticDescription();
}
