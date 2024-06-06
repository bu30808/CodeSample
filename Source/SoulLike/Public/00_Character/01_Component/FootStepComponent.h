// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/BaseCharacter.h"
#include "Engine/DataAsset.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "FootStepComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogFootStep, Log, All);

UCLASS(BlueprintType)
class SOULLIKE_API UFootStepDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	class USoundBase* FootStepSound;
	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<EPhysicalSurface>, class UNiagaraSystem*> FootStepNiagara;
};

/*
 * 발소리 및 발자국 이팩트를 처리하는 컴포넌트 입니다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULLIKE_API UFootStepComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFootStepComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UFootStepDataAsset* FootStepDataAsset;
	UPROPERTY(EditAnywhere)
	float TraceLength = 50.f;
	UPROPERTY(EditAnywhere)
	FVector StartOffset = FVector(0, 0, 30.f);
	UPROPERTY()
	TWeakObjectPtr<class ABaseCharacter> Owner;

	bool CreateFootStepTrace(FName SocketName, FHitResult& OutHit,bool& bFindWater);
	void SpawnSoundAndEffect(const FHitResult& OutHit);
	//Deep은 물의 깊이이며, 숫자가 클 수록 깊습니다. 0~2의 값을 가질수 있습니다.
	void SpawnWaterSoundAndEffect(const FHitResult& OutHit, const int32& Deep);
	bool CheckWaterDeep(FHitResult& WaterHit);

public:
	//사운드와 파티클을 생성합니다.
	UFUNCTION(BlueprintCallable)
	void MakeFootStep(FName SocketName);
};
