// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/00_Task/BTTask_UseAbility.h"

#include "AIController.h"
#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"

UBTTask_UseAbility::UBTTask_UseAbility()
{
	bNotifyTick = false;
	bCreateNodeInstance = true;
	NodeName = TEXT("어빌리티 사용");
}

EBTNodeResult::Type UBTTask_UseAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AICon = OwnerComp.GetAIOwner();
	
	if (auto character = OwnerComp.GetAIOwner()->GetPawn<ABaseCharacter>())
	{
		if (auto abComp = character->GetAbilityComponent())
		{
			if (AdditionalInfo.IsValid() == false)
			{
				AdditionalInfo = NewObject<UAbilityAdditionalInformation>(character);
			}

			if (AdditionalInfo->OnEndAbility.IsBound() == false)
			{
				AdditionalInfo->OnEndAbility.BindDynamic(this, &UBTTask_UseAbility::OnEndAbilityEvent);
			}

			abComp->ActivateAbility(AbilityTag, character/*, nullptr*/, AdditionalInfo.Get());
		}
	}

	if (!bWaitAbilityEnd)
	{
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::InProgress;
}

FString UBTTask_UseAbility::GetStaticDescription() const
{
	return TEXT("지정한 태그에 해당하는 어빌리티를 사용합니다.");
}

void UBTTask_UseAbility::OnEndAbilityEvent()
{
	if(AICon.IsValid())
	{
		FinishLatentTask(*Cast<UBehaviorTreeComponent>(AICon->BrainComponent),EBTNodeResult::Succeeded);
	}
}

UBTTask_ForceEndAbility::UBTTask_ForceEndAbility()
{
	bNotifyTick = false;
	NodeName = TEXT("어빌리티 종료");
}

EBTNodeResult::Type UBTTask_ForceEndAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto character = OwnerComp.GetAIOwner()->GetPawn<ABaseCharacter>())
	{
		if (auto abComp = character->GetAbilityComponent())
		{
			abComp->ForceEndAbility(AbilityTag);
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}

FString UBTTask_ForceEndAbility::GetStaticDescription() const
{
	return TEXT("지정한 태그에 해당하는 어빌리티를 강제종료합니다.");
}
