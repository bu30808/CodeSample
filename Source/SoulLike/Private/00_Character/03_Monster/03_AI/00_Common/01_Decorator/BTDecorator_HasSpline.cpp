// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/01_Decorator/BTDecorator_HasSpline.h"

#include "AIController.h"
#include "Components/SplineComponent.h"

UBTDecorator_HasSpline::UBTDecorator_HasSpline()
{
	NodeName = TEXT("움직여야 할 스플라인이 있나요?");
}

bool UBTDecorator_HasSpline::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if(auto pawn = OwnerComp.GetAIOwner()->GetPawn())
	{
		if(pawn->GetComponentByClass(USplineComponent::StaticClass()))
		{
			return true;
		}
		
	}
	return false;
}
