// Fill out your copyright notice in the Description page of Project Settings.


#include "97_Interface/HostileInterface.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/00_Player/00_Controller/UserController.h"
#include "00_Character/04_NPC/NPCAIController.h"
#include "00_Character/04_NPC/NPCBase.h"

// Add default functionality here for any IHostileInterface functions that are not pure virtual.
void IHostileInterface::AddHitCount(int32& Count)
{
	Count++;
}

bool IHostileInterface::IsBeHostile(const int32& CurHitCount, const int32& MaxHitCount) const
{
	return CurHitCount > MaxHitCount;
}


void IHostileInterface::ShowGotHitDialog(class ANPCBase* NPC, const AActor* HitBy)
{
	if(HitBy->IsA<APlayerCharacter>())
	{
		//다이얼로그가 켜져있지 않다면, 추가합니다.
		Cast<AUserController>(HitBy->GetInstigatorController())->AddDialogWidget(NPC,Execute_GetGotHitDialog(NPC));
	}
}
