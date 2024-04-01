// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/01_Decorator/BTDecorator_CheckAttribute.h"

#include "AIController.h"
#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AttributeComponent.h"

UBTDecorator_CheckAttribute::UBTDecorator_CheckAttribute()
{
	NodeName = TEXT("어트리뷰트 체크");
}

bool UBTDecorator_CheckAttribute::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (auto pawn = OwnerComp.GetAIOwner()->GetPawn<ABaseCharacter>())
	{
		if (auto attComp = pawn->GetAttributeComponent())
		{
			if (const auto attribute = attComp->GetAttributeByType(AttributeType))
			{
				if (AttributeCheckType == EAttributeCheckType::Absolute)
				{
					return CheckAttributeOperationAbsolute(attribute);
				}
				return CheckAttributeOperationPercent(attComp, attribute);
			}
		}
	}

	return Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
}


bool UBTDecorator_CheckAttribute::CheckAttributeOperationAbsolute(const FAttribute* const attribute) const
{
	switch (ArithmeticOperation)
	{
	case EArithmeticKeyOperation::Equal:
		return attribute->GetCurrent() == AbsoluteValue;
	case EArithmeticKeyOperation::NotEqual:
		return attribute->GetCurrent() != AbsoluteValue;
	case EArithmeticKeyOperation::Less:
		return attribute->GetCurrent() < AbsoluteValue;
	case EArithmeticKeyOperation::LessOrEqual:
		return attribute->GetCurrent() <= AbsoluteValue;
	case EArithmeticKeyOperation::Greater:
		return attribute->GetCurrent() > AbsoluteValue;
	case EArithmeticKeyOperation::GreaterOrEqual:
		return attribute->GetCurrent() >= AbsoluteValue;
	}
	return false;
}


bool UBTDecorator_CheckAttribute::CheckAttributeOperationPercent(UAttributeComponent* attComp,
                                                                 const FAttribute* const attribute) const
{
	float currentAttribute = attribute->GetCurrent();
	float maxAttribute = 0.0f;

	switch (AttributeType)
	{
	case EAttributeType::HP:
		maxAttribute = attComp->GetMaxHP();
		break;
	case EAttributeType::SP:
		maxAttribute = attComp->GetMaxSP();
		break;
	case EAttributeType::MP:
		maxAttribute = attComp->GetMaxMP();
		break;
	case EAttributeType::MAX:
		// Handle default case if needed
		break;
	}

	float percent = currentAttribute / maxAttribute;

	switch (ArithmeticOperation)
	{
	case EArithmeticKeyOperation::Equal:
		return percent == PercentValue;

	case EArithmeticKeyOperation::NotEqual:
		return percent != PercentValue;

	case EArithmeticKeyOperation::Less:
		return percent < PercentValue;

	case EArithmeticKeyOperation::LessOrEqual:
		return percent <= PercentValue;

	case EArithmeticKeyOperation::Greater:
		return percent > PercentValue;

	case EArithmeticKeyOperation::GreaterOrEqual:
		return percent >= PercentValue;
	}

	return false;
}
