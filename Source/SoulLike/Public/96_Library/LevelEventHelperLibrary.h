// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "92_Tools/NexusPortal.h"
#include "Engine/LevelScriptActor.h"
#include "UObject/Object.h"
#include "WorldPartition/DataLayer/DataLayerInstance.h"
#include "LevelEventHelperLibrary.generated.h"


class UDataTable;
class ABaseMonster;

DECLARE_DYNAMIC_DELEGATE(FK2OnLayerActivated);
UCLASS()
class AMainLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	AMainLevelScriptActor();
	
	UPROPERTY(EditAnywhere)
	UDataTable* LayerTable;
	
	UPROPERTY(Transient)
	TArray<AActor*> BlockingActors;

	
	UFUNCTION(BlueprintNativeEvent)
	void OnDeadBossEvent(AActor* Who, AActor* DeadBy, EDeadReason DeadReason);
	virtual void OnDeadBossEvent_Implementation(AActor* Who, AActor* DeadBy, EDeadReason DeadReason);
	
	bool IsAlreadySet(AActor* Player,const TArray<UDataLayerAsset*>& LayerToActive, const TArray<UDataLayerAsset*>& LayerToLoaded);
	
	UFUNCTION(BlueprintCallable)
	void ActiveLayerSetting(AActor* Player, TArray<UDataLayerAsset*> LayerToActive, TArray<UDataLayerAsset*> LayerToLoaded, const
	                        FK2OnLayerActivated& K2_OnLayerActivated);
	UFUNCTION(BlueprintCallable)
	void EndActiveLayerSetting(AActor* Player);


	FOnLayerActivated OnLayerActivated;
	
	UFUNCTION()
	void OnActivateLayerEvent(const UDataLayerInstance* DataLayer, EDataLayerRuntimeState State);
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
	static ABaseMonster* SpawnBoss(AMainLevelScriptActor* ScriptActor, AActor* Trigger, AActor* OverlapActor,
	                               TArray<AActor*> BlockingActors, AActor* TargetPoint,
	                               TSubclassOf<class ABaseMonster> BossToSpawn);
	UFUNCTION(BlueprintCallable)
	static void ActiveBlockingActors(TArray<AActor*> BlockingActors, bool bActive);
};
