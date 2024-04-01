// Fill out your copyright notice in the Description page of Project Settings.


#include "96_Library/AIConHelperLibrary.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"


void UAIConHelperLibrary::ChangePlayerState(AActor* AIConOrMonster, AActor* Player, EPlayerCharacterState NewState)
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
