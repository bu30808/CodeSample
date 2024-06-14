// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "97_Interface/BossMonsterInterface.h"
#include "ForestGolem_ElderEnt.generated.h"

UCLASS()
class SOULLIKE_API AForestGolem_ElderEnt : public ABaseMonster,public IBossMonsterInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AForestGolem_ElderEnt();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class UStaticMeshComponent* WeaponComponent;

};

