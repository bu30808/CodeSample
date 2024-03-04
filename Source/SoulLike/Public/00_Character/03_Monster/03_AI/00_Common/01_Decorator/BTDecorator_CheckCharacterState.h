// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTDecorator_CheckCharacterState.generated.h"

enum class ECharacterState : uint8;
/**
 * 
 */
UCLASS()
class SOULLIKE_API UBTDecorator_CheckCharacterState : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	ECharacterState CharacterState;


	UBTDecorator_CheckCharacterState();
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;
};
