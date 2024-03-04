// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/00_Task/BTTask_AttackByDirection.h"

#include "AIController.h"
#include "00_Character/BaseCharacter.h"
#include "96_Library/MathHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_AttackByDirection::UBTTask_AttackByDirection()
{
	NodeName = TEXT("방향별 공격");
	BlackboardKey.SelectedKeyName = "Target";
}

FString UBTTask_AttackByDirection::GetStaticDescription() const
{
	return TEXT("타겟과의 방향을 계산하여 해당 방향에 설정된 몽타주를 재생합니다.");
}

void UBTTask_AttackByDirection::OnMontageBlendingOutEvent(UAnimMontage* Montage, bool bInterrupted)
{
	FinishDelegate.ExecuteIfBound();
}

EBTNodeResult::Type UBTTask_AttackByDirection::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto character = OwnerComp.GetOwner<AAIController>()->GetPawn<ABaseCharacter>())
	{
		auto target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()));
		auto dir = UMathHelperLibrary::PointToDirection(character, target->GetActorLocation(), FrontDegree);
		if (MontageByDirection.Contains(dir))
		{
			FinishDelegate.BindLambda([&]()
			{
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			});
			character->GetMesh()->GetAnimInstance()->OnMontageBlendingOut.AddUniqueDynamic(
				this, &UBTTask_AttackByDirection::OnMontageBlendingOutEvent);
			character->GetMesh()->GetAnimInstance()->Montage_Play(MontageByDirection[dir]);

			return EBTNodeResult::InProgress;
		}
	}

	return EBTNodeResult::Failed;
}
