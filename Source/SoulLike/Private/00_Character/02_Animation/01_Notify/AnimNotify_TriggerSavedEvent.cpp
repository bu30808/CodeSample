// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/02_Animation/01_Notify/AnimNotify_TriggerSavedEvent.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "99_Subsystem/AbilitySubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

UAnimNotify_TriggerSavedEvent::UAnimNotify_TriggerSavedEvent()
{
	bIsNativeBranchingPoint = true;
}

void UAnimNotify_TriggerSavedEvent::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotify(BranchingPointPayload);

	if (BranchingPointPayload.SkelMeshComponent)
	{
		if (const auto character = Cast<APlayerCharacter>(BranchingPointPayload.SkelMeshComponent->GetOwner()))
		{
			if (const auto abilitySubsystem = UGameplayStatics::GetGameInstance(character)->GetSubsystem<
				UAbilitySubsystem>())
			{
				abilitySubsystem->TriggerSavedEvent(AbilityTag);
			}
			else
			{
				UE_LOGFMT(LogTemp, Error, "{0} {1}", __FUNCTION__, __LINE__);
			}
		}
		else
		{
			UE_LOGFMT(LogTemp, Error, "{0} {1}", __FUNCTION__, __LINE__);
		}
	}
}
