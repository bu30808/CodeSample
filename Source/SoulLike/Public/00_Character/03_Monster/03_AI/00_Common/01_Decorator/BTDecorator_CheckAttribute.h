// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BehaviorTree/Blackboard/BlackboardKeyEnums.h"
#include "BTDecorator_CheckAttribute.generated.h"

enum class EAttributeType : uint8;

UENUM(BlueprintType)
enum class EAttributeCheckType : uint8
{
	Absolute,
	Percent
};

/**
 * 
 */
UCLASS()
class SOULLIKE_API UBTDecorator_CheckAttribute : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

protected:
	UBTDecorator_CheckAttribute();
	

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EArithmeticKeyOperation::Type> ArithmeticOperation;
	UPROPERTY(EditAnywhere)
	EAttributeType AttributeType;

	UPROPERTY(EditAnywhere)
	EAttributeCheckType AttributeCheckType;
	UPROPERTY(EditAnywhere,meta=(EditCondition ="AttributeCheckType == EAttributeCheckType::Absolute"))
	float AbsoluteValue;
	UPROPERTY(EditAnywhere,meta=(EditCondition ="AttributeCheckType == EAttributeCheckType::Percent"))
	float PercentValue;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	bool CheckAttributeOperationAbsolute(const FAttribute* attribute) const;
	bool CheckAttributeOperationPercent(UAttributeComponent* attComp,const FAttribute* const attribute) const ;
};
