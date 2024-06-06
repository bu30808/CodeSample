// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/03_EQS/EnvQueryContext_Target.h"

#include "96_Library/AbilityHelperLibrary.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UEnvQueryContext_Target::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	if(auto querier = Cast<AActor>(QueryInstance.Owner))
	{
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, UAbilityHelperLibrary::GetTargetFromBlackboard(querier,"Target"));
	}
}
