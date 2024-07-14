// Fill out your copyright notice in the Description page of Project Settings.


#include "04_Item/00_Consume/ConsumeItemActor.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "02_Ability/AbilityBase.h"
#include "99_Subsystem/ItemManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

class UItemManagerSubsystem;

const FItemInformation* AConsumeItemActor::GetItemInformation()
{
	if (auto instance = UGameplayStatics::GetGameInstance(this))
	{
		if (auto system = instance->GetSubsystem<UItemManagerSubsystem>())
		{
			//여기서 무엇인가 하세요.
			return system->GetConsumeItemInformation(ItemTag);
		}
	}
#ifndef WITH_EDITOR
	UE_LOGFMT(LogTemp,Error,"서브시스템을 가져올 수 없습니다 : {0} {1}",__FUNCTION__,__LINE__);
#endif

	return nullptr;
}


int32 AConsumeItemActor::GetEffectValue()
{
	if (auto info = GetItemInformation())
	{
		return static_cast<const FConsumeInformation*>(info)->EffectValue;
	}

	return -1;
}

bool AConsumeItemActor::UseItem_Implementation(AActor* Target, const FGuid& ThisItemsUniqueID)
{
	if (Super::UseItem_Implementation(Target, ThisItemsUniqueID))
	{
		GetOwner<ABaseCharacter>()->GetInventoryComponent()->DecreaseItemCount(ThisItemsUniqueID);
		return true;
	}

	return false;
}


void AConsumeItemActor::OnEndAbilityEvent_Implementation()
{
	Super::OnEndAbilityEvent_Implementation();
}

void ASoulItemActor::OnOverrideSelfEffectEvent_Implementation(const TArray<UAbilityEffect*>& SelfEffect, AActor* Target)
{

	TArray<FAttributeEffect> newEffect;
	newEffect.Add(FAttributeEffect(EAttributeType::EXP,EAttributeApplyMethod::ADD,GetEffectValue()));
	SelfEffect[0]->AttributeEffects = newEffect;
	
}

FText ASoulItemActor::GetFormattedDescription_Implementation()
{
	return FText::Format(GetNotFormattedDescription(),GetEffectValue());
}
