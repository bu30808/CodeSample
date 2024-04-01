// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/00_Task/BTTask_MoveToAttackRange.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTTask_MoveToAttackRange::UBTTask_MoveToAttackRange()
{
	bCreateNodeInstance = true;
	bNotifyTick = true;
	NodeName = TEXT("공격 범위까지 이동");
	BlackboardKey.SelectedKeyName = "Target";
	AttackRangeKey.SelectedKeyName = "AttackRange";
}

EBTNodeResult::Type UBTTask_MoveToAttackRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AcceptableRadius = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(AttackRangeKey.SelectedKeyName) -
		ErrorMargin;

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
