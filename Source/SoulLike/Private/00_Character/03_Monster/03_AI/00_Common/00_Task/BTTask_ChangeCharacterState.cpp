// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/00_Task/BTTask_ChangeCharacterState.h"

#include "AIController.h"
#include "00_Character/03_Monster/BaseMonster.h"

UBTTask_ChangeCharacterState::UBTTask_ChangeCharacterState()
{
	bCreateNodeInstance = true;
	NodeName = TEXT("캐릭터 상태 변경");
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_ChangeCharacterState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	if(auto pawn = OwnerComp.GetAIOwner()->GetPawn<ABaseMonster>())
	{
		pawn->SetCharacterState(NewState);
		return EBTNodeResult::Succeeded;
	}
	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
