﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "96_Library/SaveGameHelperLibrary.h"

#include "01_GameMode/SoulLikeGameMode.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

bool USaveGameHelperLibrary::IsSavableGameMode(UObject* WorldContext)
{
	if (WorldContext)
	{
		return UGameplayStatics::GetGameMode(WorldContext)->IsA<ASoulLikeGameMode>();
	}

	return false;
}

USoulLikeInstance* USaveGameHelperLibrary::GetSoulLikeInstance(UObject* WorldContext)
{
	return Cast<USoulLikeInstance>(UGameplayStatics::GetGameInstance(WorldContext));
}

bool USaveGameHelperLibrary::IsActivatedBonfire(ABonfire* Bonfire)
{
	if (Bonfire)
	{
		if (IsSavableGameMode(Bonfire))
		{
			if (const auto instance = GetSoulLikeInstance(Bonfire))
			{
				return instance->IsActivatedBonfire(Bonfire);
			}
		}
	}

	return false;
}

bool USaveGameHelperLibrary::IsAlreadyPickUppedItem(AItemActor* Item)
{
	if (Item)
	{
		if (IsSavableGameMode(Item))
		{
			if (const auto instance = GetSoulLikeInstance(Item))
			{
				return instance->IsAlreadyPickUppedItem(Item);
			}
		}
	}

	return false;
}

bool USaveGameHelperLibrary::IsAlreadyReadTutorial(ATutorialActor* TutorialActor)
{
	if (TutorialActor)
	{
		if (IsSavableGameMode(TutorialActor))
		{
			if (const auto instance = GetSoulLikeInstance(TutorialActor))
			{
				return instance->IsAlreadyReadTutorial(TutorialActor);
			}
		}
	}

	return false;
}

void USaveGameHelperLibrary::SaveKillBoss(ABaseMonster* BossMonster)
{
	if (BossMonster)
	{
		if (IsSavableGameMode(BossMonster))
		{
			if (const auto instance = GetSoulLikeInstance(BossMonster))
			{
				instance->SaveKilledBoss(BossMonster);
			}
		}
	}
}

bool USaveGameHelperLibrary::IsBossKilled(UObject* WorldContext, FGameplayTag BossMonsterTag)
{
	if (BossMonsterTag.IsValid())
	{
		if (IsSavableGameMode(WorldContext))
		{
			if (const auto instance = GetSoulLikeInstance(WorldContext))
			{
				return instance->IsBossKilled(BossMonsterTag);
			}
		}
	}

	return false;
}

bool USaveGameHelperLibrary::IsSameLevel(UObject* CurLevelContext, const FString& CompareLevel)
{
	return UGameplayStatics::GetCurrentLevelName(CurLevelContext).Equals(CompareLevel);
}
