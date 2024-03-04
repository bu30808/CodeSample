// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "SoulLike/SoulLike.h"
#include "BTTask_AttackByDirection.generated.h"

enum class EDirection : uint8;

/**
 * 
 */
UCLASS()
class SOULLIKE_API UBTTask_AttackByDirection : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	FAITaskFinishDelegate FinishDelegate;

	UBTTask_AttackByDirection();
	virtual FString GetStaticDescription() const override;

	//정면으로 판정할 각도.
	UPROPERTY(EditAnywhere)
	float FrontDegree = 60.f;
	UPROPERTY(EditAnywhere)
	TMap<EDirection, class UAnimMontage*> MontageByDirection;

	UFUNCTION()
	void OnMontageBlendingOutEvent(UAnimMontage* Montage, bool bInterrupted);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
