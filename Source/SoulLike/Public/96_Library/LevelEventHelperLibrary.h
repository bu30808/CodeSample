// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "UObject/Object.h"
#include "LevelEventHelperLibrary.generated.h"


class ABaseMonster;

UCLASS()
class AMainLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	TArray<AActor*> BlockingActors;
	UFUNCTION()
	void OnDeadBossEvent(AActor* Who, AActor* DeadBy);
};
/**
 * 
 */
UCLASS()
class SOULLIKE_API ULevelEventHelperLibrary : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static ABaseMonster* SpawnBoss(AMainLevelScriptActor* ScriptActor,AActor* Trigger, AActor* OverlapActor, TArray<AActor*> BlockingActors, AActor* TargetPoint, TSubclassOf<class ABaseMonster> BossToSpawn, class USoundBase* MusicToPlay);
	UFUNCTION(BlueprintCallable)
	static void ActiveBlockingActors(TArray<AActor*> BlockingActors, bool bActive);
};
