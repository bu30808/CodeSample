// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/00_Task/BTTask_ForceUpdateTargetDistance.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ForceUpdateTargetDistance::UBTTask_ForceUpdateTargetDistance()
{
	NodeName = TEXT("타겟 거리 업데이트");
	BlackboardKey.SelectedKeyName = TEXT("Target");
}

EBTNodeResult::Type UBTTask_ForceUpdateTargetDistance::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto obj = OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsFloat("DistanceToTarget",
															Cast<AActor>(obj)->GetDistanceTo(
																OwnerComp.GetAIOwner()->GetPawn()));
		return EBTNodeResult::Succeeded;
	}
	
	
	return EBTNodeResult::Failed;
}
