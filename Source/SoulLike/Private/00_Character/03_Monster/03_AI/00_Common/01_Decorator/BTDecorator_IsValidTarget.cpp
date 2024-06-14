// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/01_Decorator/BTDecorator_IsValidTarget.h"

#include "AIController.h"
#include "00_Character/BaseCharacter.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_IsValidTarget::UBTDecorator_IsValidTarget()
{
	BlackboardKey.SelectedKeyName = "Target";
	NodeName=TEXT("타겟이 유효한가요?");
}

bool UBTDecorator_IsValidTarget::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if(auto bbComp = OwnerComp.GetBlackboardComponent())
	{
		if(auto target = bbComp->GetValueAsObject(GetSelectedBlackboardKey()))
		{
			const auto& state = Cast<ABaseCharacter>(target)->GetCharacterState();
			if(state==ECharacterState::DEAD)
			{
				if(auto pawn = OwnerComp.GetAIOwner()->GetPawn<ABaseMonster>())
				{
					pawn->OnBlackboardTargetIsNotValid.Broadcast(OwnerComp.GetAIOwner());
				}
			}
			return state != ECharacterState::DEAD;
		}
	}
	
	return false;
}
