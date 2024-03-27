// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "00_Character/04_NPC/Bonfire.h"
#include "92_Tools/TutorialActor.h"
#include "UObject/Object.h"
#include "SaveGameHelperLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API USaveGameHelperLibrary : public UObject
{
	GENERATED_BODY()
public:


	UFUNCTION(BlueprintCallable,BlueprintPure)
	static bool IsSavableGameMode(class UObject* WorldContext);

	UFUNCTION(BlueprintCallable,BlueprintPure)
	static class USoulLikeInstance* GetSoulLikeInstance(class UObject* WorldContext);
	
	UFUNCTION(BlueprintCallable,BlueprintPure)
	static bool IsActivatedBonfire(class ABonfire* Bonfire);
	
	UFUNCTION(BlueprintCallable,BlueprintPure)
	static bool IsAlreadyPickUppedItem(class AItemActor* Item);

	UFUNCTION(BlueprintCallable,BlueprintPure)
	static bool IsAlreadyReadTutorial(class ATutorialActor* TutorialActor);

	UFUNCTION(BlueprintCallable)
	static void SaveKillBoss(class ABaseMonster* BossMonster);
	
	UFUNCTION(BlueprintCallable,BlueprintPure)
	static bool IsBossKilled(class UObject* WorldContext,FGameplayTag BossMonsterTag);

};
