// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/01_Decorator/BTDecorator_Chance.h"

UBTDecorator_Chance::UBTDecorator_Chance()
{
	NodeName = TEXT("실행 확률");
}

bool UBTDecorator_Chance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	return FMath::RandRange(0.f, 1.f) <= ActiveChance;
}

FString UBTDecorator_Chance::GetStaticDescription() const
{
	return FString::FormatAsNumber(ActiveChance * 100.f) + TEXT("% 확률로 이 노드를 실행합니다.");
}
