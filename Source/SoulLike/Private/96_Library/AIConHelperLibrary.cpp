// Fill out your copyright notice in the Description page of Project Settings.


#include "96_Library/AIConHelperLibrary.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"


void UAIConHelperLibrary::ChangePlayerState(AActor* AIConOrMonster, AActor* Player, ECombatState NewState)
{
	if (AIConOrMonster->IsA<ABaseMonster>())
	{
		ABaseMonster* mon = Cast<ABaseMonster>(AIConOrMonster);
		if (mon->ShouldForceCombatState())
		{
			Cast<APlayerCharacter>(Player)->SetPlayerStateBy(NewState, mon);
		}
	}
	else
	{
		AMonsterAIController* aiCon = Cast<AMonsterAIController>(AIConOrMonster);
		if (aiCon->ShouldForceCombatState())
		{
			Cast<APlayerCharacter>(Player)->SetPlayerStateBy(NewState, aiCon->GetPawn());
		}
	}
}

AActor* UAIConHelperLibrary::GetBlackboardValueNamedTarget(AActor* BlackboardOwner)
{
	if(auto bbComp = UAIBlueprintHelperLibrary::GetBlackboard(BlackboardOwner))
	{
		if(auto target = bbComp->GetValueAsObject("Target"))
		{
			return Cast<AActor>(target);
		}
		
	}

	return nullptr;
}

ETeamAttitude::Type UAIConHelperLibrary::CheckTeam(AActor* Checker, const AActor& Other)
{
	if(const APawn* CheckerPawn= Cast<APawn>(Checker))
	{
		if (const APawn* OtherPawn = Cast<APawn>(&Other))
		{
			// DEFAULT BEHAVIOR---------------------------------------------------
			/*if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController()))
			{
				return Super::GetTeamAttitudeTowards(*OtherPawn->GetController());
			}*/

			if(const IGenericTeamAgentInterface* checkerTeamAgent = Cast<IGenericTeamAgentInterface>(CheckerPawn->GetController()))
			//OR CUSTOM BEHAVIOUR--------------------------------------------------
			if (const IGenericTeamAgentInterface* otherTeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController()))
			{
				//Create an alliance with Team with ID 10 and set all the other teams as Hostiles:
				FGenericTeamId OtherTeamID = otherTeamAgent->GetGenericTeamId();
				if (OtherTeamID == ETeamAttitude::Neutral)
				{
					return ETeamAttitude::Neutral;
				}

				if (OtherTeamID == checkerTeamAgent->GetGenericTeamId())
				{
					//UE_LOGFMT(LogAICon, Log, "Other Team ID: {0}, My Team ID: {1} , return Friendly", OtherTeamID.GetId(), GetGenericTeamId().GetId());
					return ETeamAttitude::Friendly;
				}
			}
		}
	}

	return ETeamAttitude::Hostile;
}
