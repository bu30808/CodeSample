// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/00_Task/BTTask_StopMovement.h"

#include "AIController.h"

UBTTask_StopMovement::UBTTask_StopMovement()
{
	NodeName = TEXT("Stop Movement");
	
}

EBTNodeResult::Type UBTTask_StopMovement::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(auto aiCon = OwnerComp.GetAIOwner())
	{
		aiCon->StopMovement();
		return EBTNodeResult::Succeeded;
	}
	
	return EBTNodeResult::Failed;
}
