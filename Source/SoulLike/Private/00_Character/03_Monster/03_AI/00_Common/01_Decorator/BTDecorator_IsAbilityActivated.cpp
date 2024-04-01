// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/01_Decorator/BTDecorator_IsAbilityActivated.h"

#include "AIController.h"
#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "Logging/StructuredLog.h"

UBTDecorator_IsAbilityActivated::UBTDecorator_IsAbilityActivated()
{
	bCreateNodeInstance = true;
	NodeName = TEXT("어빌리티가 사용중인가요?");
}

bool UBTDecorator_IsAbilityActivated::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
                                                                 uint8* NodeMemory) const
{
	if (auto pawn = OwnerComp.GetAIOwner()->GetPawn<ABaseCharacter>())
	{
		bool result = pawn->GetAbilityComponent()->IsAlreadyActivated(TagToCheck);
		return result;
	}

	UE_LOGFMT(LogTemp, Error, "폰을 가져올 수 없습니다.");

	return false;
}
