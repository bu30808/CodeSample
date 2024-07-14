// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/99_Composite/BTCompositeNode_RandomSelector.h"


UBTCompositeNode_RandomSelector::UBTCompositeNode_RandomSelector(const FObjectInitializer& ObjectInitializer)
{
	NodeName = "RandomSelector";
}

void UBTCompositeNode_RandomSelector::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                                       EBTMemoryInit::Type InitType) const
{
	InitializeNodeMemory<FBTCompositeMemory>(NodeMemory, InitType);
}

void UBTCompositeNode_RandomSelector::CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTMemoryClear::Type CleanupType) const
{
	CleanupNodeMemory<FBTCompositeMemory>(NodeMemory, CleanupType);
}

int32 UBTCompositeNode_RandomSelector::GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild,
	EBTNodeResult::Type LastResult) const
{

	int32 nextChildIndex = BTSpecialChild::ReturnToParent;
	if(PrevChild == BTSpecialChild::NotInitialized)
	{
		nextChildIndex = FMath::RandRange(0,GetChildrenNum()-1);
		
	}else if(LastResult == EBTNodeResult::Failed)
	{
		nextChildIndex = FMath::RandRange(0,GetChildrenNum()-1);
	}
	
	return nextChildIndex;
}
