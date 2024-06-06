// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/03_AI/00_Common/03_EQS/EnvQueryContext_Strafe.h"

#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "Kismet/KismetMathLibrary.h"

void UEnvQueryContext_Strafe::ProvideContext(FEnvQueryInstance& QueryInstance,
                                             FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	if (auto querier = Cast<AActor>(QueryInstance.Owner))
	{
		FVector ResultVector;
		switch (StrafeDirection)
		{
		case EStrafeDirection::LEFT:
			ResultVector = querier->GetActorLocation() + UKismetMathLibrary::GetRightVector(querier->GetActorRotation())
				* -1 * Distance;
			break;
		case EStrafeDirection::RIGHT:
			ResultVector = querier->GetActorLocation() + UKismetMathLibrary::GetRightVector(querier->GetActorRotation())
				* Distance;
			break;
		case EStrafeDirection::RANDOM:

			if (FMath::RandBool())
			{
				ResultVector = querier->GetActorLocation() + UKismetMathLibrary::GetRightVector(
					querier->GetActorRotation()) * -1 * Distance;
			}
			else
			{
				ResultVector = querier->GetActorLocation() + UKismetMathLibrary::GetRightVector(
					querier->GetActorRotation()) * Distance;
			}
			break;
		default: ;
		}

		UEnvQueryItemType_Point::SetContextHelper(ContextData, ResultVector);
	}
}
