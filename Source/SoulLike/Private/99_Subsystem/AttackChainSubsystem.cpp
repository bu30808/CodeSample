// Fill out your copyright notice in the Description page of Project Settings.


#include "99_Subsystem/AttackChainSubsystem.h"

#include "Logging/StructuredLog.h"

void UAttackChainSubsystem::AddChainValue(FGameplayTag ChainTag, float Value, UAbilityBase* From)
{
	UE_LOGFMT(LogTemp, Log, "서브시스템 체인 저장 : {0} , {1}", ChainTag.ToString(), Value);
	AttackChainMap.Add(ChainTag, FAbilityChainData(Value,From));
}


float UAttackChainSubsystem::GetChainValue(FGameplayTag ChainTag)
{
	if (AttackChainMap.Contains(ChainTag))
	{
		float val = AttackChainMap[ChainTag].ChainValue;
		//AttackChainMap.Remove(ChainTag);
		return val;
	}

	return 1;
}
