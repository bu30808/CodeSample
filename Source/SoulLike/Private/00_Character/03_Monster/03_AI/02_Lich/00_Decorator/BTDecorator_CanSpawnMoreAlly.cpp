// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/02_Lich/00_Decorator/BTDecorator_CanSpawnMoreAlly.h"

#include "AIController.h"
#include "00_Character/03_Monster/05_Lich/LichBoss.h"

UBTDecorator_CanSpawnMoreAlly::UBTDecorator_CanSpawnMoreAlly()
{
	NodeName = TEXT("아군 더 스폰 가능?");
}

bool UBTDecorator_CanSpawnMoreAlly::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
                                                               uint8* NodeMemory) const
{
	if (const auto pawn = OwnerComp.GetAIOwner()->GetPawn<ALichBoss>())
	{
		return pawn->CanSpawnMoreAlly();
	}
	return false;
}
