// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/01_Decorator/BTDecorator_CheckCharacterState.h"

#include "AIController.h"
#include "00_Character/BaseCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Logging/StructuredLog.h"

UBTDecorator_CheckCharacterState::UBTDecorator_CheckCharacterState()
{
	BlackboardKey.SelectedKeyName = "CharacterState";
	NodeName = TEXT("캐릭터 상태 확인");
}

bool UBTDecorator_CheckCharacterState::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
                                                                  uint8* NodeMemory) const
{
		return CharacterState == static_cast<ECharacterState>(OwnerComp.GetBlackboardComponent()->GetValueAsEnum(BlackboardKey.SelectedKeyName));
}

FString UBTDecorator_CheckCharacterState::GetStaticDescription() const
{
	return TEXT("캐릭터 상태 확인");
}
