// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/02_Service/BTService_GetDistance.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Logging/StructuredLog.h"

UBTService_GetDistance::UBTService_GetDistance()
{
	NodeName = TEXT("거리 측정");
	bNotifyTick = true;
	BlackboardKey.SelectedKeyName = "Target";
}

FString UBTService_GetDistance::GetStaticDescription() const
{
	return TEXT("타겟과의 거리를 계산해 저장합니다.");
}

void UBTService_GetDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (auto obj = OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsFloat("DistanceToTarget",
		                                                    Cast<AActor>(obj)->GetDistanceTo(
			                                                    OwnerComp.GetAIOwner()->GetPawn()));
	}
	else
	{
		UE_LOGFMT(LogTemp, Error, "타겟정보를 가져올 수 없습니다.");
	}
}
