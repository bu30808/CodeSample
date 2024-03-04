// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/00_Task/BTTask_ChangeMonsterState.h"

#include "AIController.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"

UBTTask_ChangeMonsterState::UBTTask_ChangeMonsterState()
{
	bCreateNodeInstance = true;
	NodeName = TEXT("몬스터 상태 변경");
	BlackboardKey.SelectedKeyName = "Target";
}

EBTNodeResult::Type UBTTask_ChangeMonsterState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto ownerCharacter = OwnerComp.GetAIOwner()->GetPawn<ABaseMonster>())
	{
		/*ownerCharacter->SetMonsterState(
			static_cast<EMonsterState>(FMath::RandRange(0, static_cast<int32>(EMonsterState::MAX) - 1)));*/
		ownerCharacter->SetMonsterState(NewMonsterState);

		//평화로 바꾸는 경우 타겟을 잊습니다.
		if(NewMonsterState==EMonsterState::Peaceful)
		{
			if(auto taget = OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()))
			{
				//OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(),nullptr);
				OwnerComp.GetAIOwner()->PerceptionComponent->ForgetActor(Cast<AActor>(taget));
				OwnerComp.GetAIOwner()->PerceptionComponent->OnTargetPerceptionForgotten.Broadcast(Cast<AActor>(taget));
			}
		}
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

FString UBTTask_ChangeMonsterState::GetStaticDescription() const
{
	return TEXT("몬스터 상태(평화, 전투) 전환");
}
