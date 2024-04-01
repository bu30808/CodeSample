// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/01_Decorator/BTDecorator_IsInAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Logging/StructuredLog.h"

UBTDecorator_IsInAttackRange::UBTDecorator_IsInAttackRange()
{
	BlackboardKey.SelectedKeyName = "DistanceToTarget";
	AttackRangeKey.SelectedKeyName = "AttackRange";
	NodeName = TEXT("공격 가능한 범위에 들었나요?");
}

bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
                                                              uint8* NodeMemory) const
{
	bool result = false;
	if (auto bbComp = OwnerComp.GetBlackboardComponent())
	{
		const auto attackRange = bbComp->GetValueAsFloat(AttackRangeKey.SelectedKeyName);
		const auto distanceToTarget = bbComp->GetValueAsFloat(GetSelectedBlackboardKey());
		UE_LOGFMT(LogTemp, Log, "읽어온 공격 범위 : {0}, 마진을 적용한 최종 범위 : {1} ~ {2} , 타겟과의 거리 : {3}", attackRange,
		          attackRange - InMargin, attackRange + OutMargin, distanceToTarget);

		result = (attackRange - InMargin <= distanceToTarget && distanceToTarget <= attackRange + OutMargin);
	}

	return result;
}

UBTDecorator_IsCloserThanAttackRange::UBTDecorator_IsCloserThanAttackRange()
{
	BlackboardKey.SelectedKeyName = "DistanceToTarget";
	AttackRangeKey.SelectedKeyName = "AttackRange";
	NodeName = TEXT("공격 가능한 범위보다 가깝나요?");
}

bool UBTDecorator_IsCloserThanAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
                                                                      uint8* NodeMemory) const
{
	bool result = false;
	if (auto bbComp = OwnerComp.GetBlackboardComponent())
	{
		const auto attackRange = bbComp->GetValueAsFloat(AttackRangeKey.SelectedKeyName);
		const auto distanceToTarget = bbComp->GetValueAsFloat(GetSelectedBlackboardKey());
		result = attackRange + ErrorMargin >= distanceToTarget;
		if (result)
		{
			UE_LOGFMT(LogTemp, Log, "읽어온 공격 범위 : {0}, 마진을 적용한 최종 범위 : {1}  , 타겟과의 거리 : {2}", attackRange,
			          attackRange + ErrorMargin, distanceToTarget);
			UE_LOGFMT(LogTemp, Log, "공격범위보다 가까이 있습니다.");
		}
	}

	return result;
}

UBTDecorator_IsFartherThanAttackRange::UBTDecorator_IsFartherThanAttackRange()
{
	BlackboardKey.SelectedKeyName = "DistanceToTarget";
	AttackRangeKey.SelectedKeyName = "AttackRange";
	NodeName = TEXT("공격 가능한 범위보다 멀리있나요?");
}

bool UBTDecorator_IsFartherThanAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
                                                                       uint8* NodeMemory) const
{
	bool result = false;
	if (auto bbComp = OwnerComp.GetBlackboardComponent())
	{
		const auto attackRange = bbComp->GetValueAsFloat(AttackRangeKey.SelectedKeyName);
		const auto distanceToTarget = bbComp->GetValueAsFloat(GetSelectedBlackboardKey());

		result = distanceToTarget >= attackRange - ErrorMargin;

		if (result)
		{
			UE_LOGFMT(LogTemp, Log, "읽어온 공격 범위 : {0}, 마진을 적용한 최종 범위 : {1} , 타겟과의 거리 : {2}", attackRange,
			          attackRange - ErrorMargin, distanceToTarget);
			UE_LOGFMT(LogTemp, Log, "공격범위보다 멀리 있습니다.");
		}
	}

	return result;
}
