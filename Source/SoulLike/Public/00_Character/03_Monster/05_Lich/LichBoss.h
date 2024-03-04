// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/03_Monster/05_Lich/Lich.h"
#include "94_Task/GameplayTask_LaunchTimerEvent.h"
#include "97_Interface/BossMonsterInterface.h"
#include "LichBoss.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API ALichBoss : public ALich, public IBossMonsterInterface
{
	GENERATED_BODY()

public:
	ALichBoss();

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> SpawnPoints;


	UPROPERTY(EditAnywhere, Category="Beam")
	TSubclassOf<AActor> BeamObject;
	UPROPERTY(BlueprintReadWrite, Category="Beam")
	TWeakObjectPtr<AActor> Beam;
	//기본값은 5입니다. 다른 함수 내부에서 덮어씌워질 수 있습니다.
	UPROPERTY(BlueprintReadOnly)
	int32 AllySpawnCount = 5;
	UPROPERTY(EditAnywhere)
	class UAnimMontage* SummonAllyMontage;

	FTimerHandle SpawnAllyTimerHandle;

	UFUNCTION(BlueprintCallable)
	void SpawnBeam();
	UFUNCTION(BlueprintCallable)
	void DestroyBeam();

	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> SpawnMonsters;
	UFUNCTION(BlueprintCallable)
	virtual void K2_SpawnAlly_Implementation(int32 SpawnCount) override;
	bool CanSpawnMoreAlly() const;


	UFUNCTION()
	void OnAllyDeadEvent(AActor* Who, AActor* DeadBy);

	UFUNCTION()
	void SpawnAllyEvent();

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ABaseMonster>> SpawnableMonsters;
	UPROPERTY(EditAnywhere)
	FAbilityCueInformation SpawnCueInformation;
	UFUNCTION()
	class ABaseMonster* SpawnRandomMonster();
	UFUNCTION()
	void ShareTarget(class ABaseMonster* Monster);


	UPROPERTY(EditAnywhere, Category="SpawnSpell")
	TArray<TSubclassOf<class UAbilityEffect>> SpawnSpellEffects;
	UPROPERTY()
	FTimerHandle LaunchProjectileTimerHandle;
	UPROPERTY()
	TWeakObjectPtr<class UGameplayTask_LaunchTimerEvent> SpawnSpellTask;
	UPROPERTY()
	int32 SpawnSpellCount;
	UPROPERTY(EditAnywhere, Category="SpawnSpell")
	TArray<FName> SpawnSpellPoint;
	UPROPERTY(EditAnywhere, Category="SpawnSpell")
	TSubclassOf<class AProjectileActor> HomingSpellObject;
	UPROPERTY(BlueprintReadOnly, Category="SpawnSpell")
	TArray<AProjectileActor*> SpawnSpells;
	UFUNCTION(BlueprintCallable)
	void SpawnSpell();
	UFUNCTION()
	void OnSpawnSpellTimerEvent();
	UFUNCTION()
	void LaunchProjectileEvent();

	UPROPERTY(EditAnywhere, Category="ProjectileFast")
	TSubclassOf<class AProjectileActor> ProjectileFastObject;
	UPROPERTY(EditAnywhere, Category="ProjectileFast")
	TArray<TSubclassOf<class UAbilityEffect>> ProjectileFastEffects;
	UPROPERTY(BlueprintReadOnly, Category="ProjectileFast")
	TArray<AProjectileActor*> ProjectileFasts;
	UFUNCTION(BlueprintCallable)
	void SpawnProjectileFast();
	UFUNCTION(BlueprintCallable)
	void LaunchProjectileFast();


	UPROPERTY(EditAnywhere, Category="SpawnCrack")
	TSubclassOf<class AActor> CrackObject;
	UPROPERTY(EditAnywhere, Category="SpawnCrack")
	FVector CrackOffset;
	UPROPERTY(EditAnywhere, Category="SpawnCrack")
	float CrackForward = 150.f;
	UFUNCTION(BlueprintCallable)
	void SpawnCrack();
};
