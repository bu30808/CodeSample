// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/01_Decorator/BTDecorator_DirectionCheck.h"

#include "AIController.h"
#include "96_Library/MathHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "00_Character/BaseCharacter.h"

UBTDecorator_DirectionCheck::UBTDecorator_DirectionCheck()
{
	NodeName = TEXT("방향 확인");
	BlackboardKey.SelectedKeyName = "Target";
}

FString UBTDecorator_DirectionCheck::GetStaticDescription() const
{
	FString msg = TEXT("타겟이 ");

	msg += StaticEnum<EDirection>()->GetValueAsString(DirectionToCheck);

	msg += TEXT("방향에 있는지 확인합니다.");

	return msg;
}

bool UBTDecorator_DirectionCheck::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (auto pawn = OwnerComp.GetAIOwner()->GetPawn())
	{
		if (auto target = Cast<ABaseCharacter>(
			OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey())))
		{
			return UMathHelperLibrary::PointToDirection(pawn, target->GetActorLocation(), FrontDegree, RightDegree, LeftDegree) ==
				DirectionToCheck;
		}
	}


	return false;
}
