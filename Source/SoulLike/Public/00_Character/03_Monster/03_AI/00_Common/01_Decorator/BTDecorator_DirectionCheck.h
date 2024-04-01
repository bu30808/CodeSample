// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTDecorator_DirectionCheck.generated.h"

enum class EDirection : uint8;
/**
 * 앞, 좌, 우 적용후 남은것을 후방이라고 생각합니다.
 */
UCLASS()
class SOULLIKE_API UBTDecorator_DirectionCheck : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	EDirection DirectionToCheck;

	//이 값 만큼을 전방이라고 생각합니다.
	UPROPERTY(EditAnywhere)
	float FrontDegree = 90.f;
	//전방을 적용한 뒤, 이 값 만큼을 좌측이라고 생각합니다.
	UPROPERTY(EditAnywhere)
	float LeftDegree = 90.f;
	//전방을 적용한 뒤, 이 값 만큼을 우측이라고 생각합니다.
	UPROPERTY(EditAnywhere)
	float RightDegree = 90.f;

	UBTDecorator_DirectionCheck();
	virtual FString GetStaticDescription() const override;
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
