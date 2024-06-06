// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "97_Interface/BossMonsterInterface.h"
#include "Goblin.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API AGoblin : public ABaseMonster
{
	GENERATED_BODY()

public:
	AGoblin();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class USkeletalMeshComponent* HelmetComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class USkeletalMeshComponent* SkirtComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class USkeletalMeshComponent* LegComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class USkeletalMeshComponent* BracerComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class USkeletalMeshComponent* BodyComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class UStaticMeshComponent* WeaponComponent;

	
};

UCLASS()
class SOULLIKE_API AGoblinRogue : public AGoblin
{
	GENERATED_BODY()

public:
	AGoblinRogue();
};

UCLASS()
class SOULLIKE_API AGoblinWarrior : public AGoblin
{
	GENERATED_BODY()
public:
	AGoblinWarrior();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class UStaticMeshComponent* ShieldComponent;
};

UCLASS()
class SOULLIKE_API AGoblinMage : public AGoblin
{
	GENERATED_BODY()

public:
	AGoblinMage();
};

UCLASS()
class SOULLIKE_API AGoblinCrystallized : public AGoblin
{
	GENERATED_BODY()
public:
	AGoblinCrystallized();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class USkeletalMeshComponent* CrystallizedArmComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class USkeletalMeshComponent* CrystallizedBodyComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class USkeletalMeshComponent* CrystallizedLegBottomComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class USkeletalMeshComponent* CrystallizedLegTopComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class USkeletalMeshComponent* CrystallizedHeadComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class USkeletalMeshComponent* CrystallizedShoulderComponent;
};

UCLASS()
class SOULLIKE_API AGoblinBattleMage : public AGoblinCrystallized, public IBossMonsterInterface
{
	GENERATED_BODY()
public:
	AGoblinBattleMage();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class UStaticMeshComponent* StaffComponent;
public:
	UFUNCTION(BlueprintCallable,BlueprintPure)
	class UPrimitiveComponent* GetStaffComponent() const {return StaffComponent;}
};