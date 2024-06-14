// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/00_Task/BTTask_ChangeValue.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ChangeBoolValue::UBTTask_ChangeBoolValue()
{
	NodeName = TEXT("부울 값 변경");
	BlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ChangeBoolValue, BlackboardKey));
}

EBTNodeResult::Type UBTTask_ChangeBoolValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(),bNewValue);
	return EBTNodeResult::Succeeded;
}
