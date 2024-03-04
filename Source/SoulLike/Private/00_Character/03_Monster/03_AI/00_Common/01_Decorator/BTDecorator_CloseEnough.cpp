// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/01_Decorator/BTDecorator_CloseEnough.h"

#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_CloseEnough::UBTDecorator_CloseEnough()
{
	NodeName = TEXT("충분히 가깝니?");
	BlackboardKey.SelectedKeyName = "Target";
	bShowInverseConditionDesc = true;
}

FString UBTDecorator_CloseEnough::GetStaticDescription() const
{
	return TEXT("타겟이 ") + FString::FormatAsNumber(Distance) + TEXT("보다 가까이 있는지 확인합니다.");
}

bool UBTDecorator_CloseEnough::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* bbComp = OwnerComp.GetBlackboardComponent();
	if (!bbComp)
	{
		return false;
	}

	if (UObject* target = bbComp->GetValueAsObject(GetSelectedBlackboardKey()))
	{
		return OwnerComp.GetOwner<AMonsterAIController>()->GetPawn()->GetDistanceTo(Cast<AActor>(target)) < Distance;
	}


	return false;
}
