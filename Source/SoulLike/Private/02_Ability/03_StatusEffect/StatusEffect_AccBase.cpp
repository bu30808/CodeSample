// Fill out your copyright notice in the Description page of Project Settings.


#include "02_Ability/03_StatusEffect/StatusEffect_AccBase.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "96_Library/AttributeHelperLibrary.h"
#include "Logging/StructuredLog.h"

UStatusEffect_AccBase::UStatusEffect_AccBase()
{
	EffectApplyType = EEffectApplyType::Instant;
}

bool UStatusEffect_AccBase::ApplyInstantEffect_Implementation(ABaseCharacter* Target, UObject* AdditionalInfo,
                                                              float DeltaTime)
{
	bool result = Super::ApplyInstantEffect_Implementation(Target, AdditionalInfo, DeltaTime);
	
	for(auto iter : AttributeEffects)
	{
		EStatusEffect statusEffect;
		float accValue = 0;
		float resistValue = 0;
		UAttributeHelperLibrary::AttributeEffectElementToStatusEffectValues(Target->GetAttributeComponent(),iter.Attribute,statusEffect,accValue,resistValue);

		UE_LOGFMT(LogEffect,Log,"{0}에게 적용할 상태이상 {1}의 수치 : {2}, {3}의 상태이상 저항 : {4}",GetNameSafe(Target),StaticEnum<EStatusEffect>()->GetValueAsString(statusEffect),accValue,GetNameSafe(Target),resistValue);
		
		//UKismetSystemLibrary::PrintString(Target,FString::Printf(TEXT("적용할 상태이상 수치 : %f, 저항수치 : %f"),iter.ApplyValue,resistValue));

		Target->GetAttributeComponent()->OnUpdateStatusEffect.Broadcast(statusEffect,accValue,resistValue);

	}
	
	return result;
}

bool UStatusEffect_AccBase::CanApplyEffect_Implementation(ABaseCharacter* Target, bool bShowLog) const
{
	return Super::CanApplyEffect_Implementation(Target, bShowLog) && !Target->GetAbilityComponent()->HasEffectTag(CheckStatusEffectTag);
}
