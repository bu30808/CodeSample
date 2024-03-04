// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/00_Task/BTTask_SpawnAlly.h"

#include "AIController.h"
#include "00_Character/03_Monster/BaseMonster.h"

UBTTask_SpawnAlly::UBTTask_SpawnAlly()
{
	NodeName = TEXT("아군 소환");
	BlackboardKey.SelectedKeyName = "Target";
}

EBTNodeResult::Type UBTTask_SpawnAlly::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto pawn = OwnerComp.GetAIOwner()->GetPawn<ABaseMonster>())
	{
		if (OnFinishSpawnAlly.IsBound() == false)
		{
			OnFinishSpawnAlly.AddUniqueDynamic(this, &UBTTask_SpawnAlly::FinishTaskEvent);
		}

		pawn->SpawnAlly(SpawnCount, OnFinishSpawnAlly);

		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

void UBTTask_SpawnAlly::FinishTaskEvent(UBehaviorTreeComponent* OwnerComp, EBTNodeResult::Type Result)
{
	FinishLatentTask(*OwnerComp, Result);
}
