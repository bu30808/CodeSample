// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "97_Interface/BossMonsterInterface.h"
#include "97_Interface/LockOnInterface.h"
#include "WolfBoss.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API AWolfBoss : public ABaseMonster, public IBossMonsterInterface
{
	GENERATED_BODY()

public:
	AWolfBoss();

	UFUNCTION(BlueprintCallable)
	void CreateDynamicMaterials();

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UMaterialInterface*> MaterialInterfaces;
	UPROPERTY(BlueprintReadWrite)
	TArray<class UMaterialInstanceDynamic*> MaterialInstances;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Phase2ColorSet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Phase2EmissivePower;


	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class ADynamicSkyActor> Sky;
	UPROPERTY(Transient)
	float OriginalTime;
	UPROPERTY(EditAnywhere, Transient)
	float NightTime;

public:
	//타임라인 쓰기엔 블루프린트가 편하니까 블루프린트 내부에서 호출되어 변경됩니다.
	UFUNCTION(BlueprintCallable)
	void ChangePhase2ColorSet(float Alpha);

	UFUNCTION(BlueprintCallable)
	void ChangeSkyTime(float Time, float Alpha);
};
