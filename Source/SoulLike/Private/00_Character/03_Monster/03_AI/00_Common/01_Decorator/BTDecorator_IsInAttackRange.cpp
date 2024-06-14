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
	if (auto bbComp = OwnerComp.GetBlackboardComponent())
	{
		const auto attackRange = bbComp->GetValueAsFloat(AttackRangeKey.SelectedKeyName);
		const auto distanceToTarget = bbComp->GetValueAsFloat(GetSelectedBlackboardKey());
		


		if(OffsetDistance >= 0)
		{
			UE_LOGFMT(LogTemp, Log, "읽어온 공격 범위 : {0}, 확인할 범위 : {1} >= {2} ", attackRange,  attackRange - OffsetDistance, distanceToTarget);
			return attackRange - OffsetDistance >= distanceToTarget;
		}
		UE_LOGFMT(LogTemp, Log, "읽어온 공격 범위 : {0}, 확인할 범위 : {1} >= {2} ", attackRange,  attackRange + OffsetDistance, distanceToTarget);
		return attackRange + OffsetDistance >= distanceToTarget;
	}

	return false;
}
