// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BTCompositeNode_RandomSelector.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UBTCompositeNode_RandomSelector : public UBTCompositeNode
{
	GENERATED_BODY()

protected:
	UBTCompositeNode_RandomSelector(const FObjectInitializer& ObjectInitializer);
 
	virtual void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const override;
	virtual void CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const override;

	virtual int32 GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const override;
};
