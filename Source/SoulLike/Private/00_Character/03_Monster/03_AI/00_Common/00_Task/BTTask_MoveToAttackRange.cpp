// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/00_Task/BTTask_MoveToAttackRange.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Logging/StructuredLog.h"

/*
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

EBTNodeResult::Type UBTTask_MoveToAttackRange::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(OwnerComp.GetAIOwner())
	{
		UKismetSystemLibrary::PrintString(this,FString::Printf(TEXT("%s, 무브 테스크 종료됨"),*GetNameSafe(OwnerComp.GetAIOwner()->GetPawn())));
		OwnerComp.GetAIOwner()->StopMovement();
	}
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTask_MoveToAttackRange::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if(auto character = OwnerComp.GetAIOwner()->GetPawn<ABaseCharacter>())
	{
		if(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()) == nullptr)
		{
			UE_LOGFMT(LogAICon,Warning,"블랙보드 타겟이 비어있습니다. MoveTo를 종료합니다.");
			OwnerComp.GetAIOwner()->StopMovement();
			FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
		}

		if(character->GetCharacterState() != ECharacterState::NORMAL)
		{
			UE_LOGFMT(LogAICon,Warning,"ECharacterState::NORMAL이 아닙니다. MoveTo를 종료합니다.");
			OwnerComp.GetAIOwner()->StopMovement();
			FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
		}
	}
}
*/
UBTTask_MoveToAttackRange::UBTTask_MoveToAttackRange()
{
	bCreateNodeInstance = true;
	NodeName = TEXT("공격 범위 정보를 이용한 이동");
	BlackboardKey.SelectedKeyName = "Target";
	AttackRangeKey.SelectedKeyName = "AttackRange";
}

EBTNodeResult::Type UBTTask_MoveToAttackRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AICon = OwnerComp.GetAIOwner();
	if(AICon.IsValid()){
		AcceptableRadius = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(AttackRangeKey.SelectedKeyName) - OffsetDistance;

		AICon->ReceiveMoveCompleted.AddUniqueDynamic(this,&UBTTask_MoveToAttackRange::OnMoveFinished);
		
		auto target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()));
		AICon->MoveToActor(target,AcceptableRadius);
	
		return EBTNodeResult::InProgress;
	}
	
	return EBTNodeResult::Failed;
}

EBTNodeResult::Type UBTTask_MoveToAttackRange::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(AICon.IsValid()){
		AICon->ReceiveMoveCompleted.RemoveDynamic(this,&UBTTask_MoveToAttackRange::OnMoveFinished);
		AICon->StopMovement();
	}
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTask_MoveToAttackRange::OnMoveFinished(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if(AICon.IsValid()){
		AICon->ReceiveMoveCompleted.RemoveDynamic(this,&UBTTask_MoveToAttackRange::OnMoveFinished);
		FinishLatentTask(*Cast<UBehaviorTreeComponent>(AICon->GetBrainComponent()),EBTNodeResult::Succeeded);
	}
}

