// Fill out your copyright notice in the Description page of Project Settings.


#include "97_Interface/01_Monster/AIInterface.h"

#include "00_Character/BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Logging/StructuredLog.h"

// Add default functionality here for any IAIInterface functions that are not pure virtual.
float IAIInterface::GetScaledAttackRange(float Original)
{
	if (auto aiPawn = Cast<ABaseCharacter>(this))
	{
		return (Original * Cast<AActor>(this)->GetActorScale3D().X) + aiPawn->GetCapsuleComponent()->
		                                                                      GetScaledCapsuleRadius();
	}

	return Original;
}
