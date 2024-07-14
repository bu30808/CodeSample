// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AbilitySubsystem.generated.h"

/**
 * 
 */

USTRUCT()
struct FAnimationNotifyEffects
{
	GENERATED_BODY()
public:
	UPROPERTY(Transient)
	TArray<class UAbilityEffect*> Effects;

	FAnimationNotifyEffects() {}
	FAnimationNotifyEffects(const TArray<class UAbilityEffect*>& AppliedEffects): Effects(AppliedEffects)
	{
		
	}
};


USTRUCT()
struct FAnimationNotifyProjectiles
{
	GENERATED_BODY()
public:
	UPROPERTY(Transient)
	TArray<class AProjectileActor*> Projectiles;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSavedAbilityEvent);

UCLASS()
class SOULLIKE_API UAbilitySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	//애니메이션 노티파이의 ApplyEffect시 캐릭터별로 저장할 Effect배열입니다.
	UPROPERTY()
	TMap<AActor*, FAnimationNotifyEffects> AppliedEffectDuringAnimation;

	/********************************************************/

	//애니메이션 노티파이에서 생성되는 발사체를 관리할 내용들
	UPROPERTY()
	TMap<AActor*, FAnimationNotifyProjectiles> Projectiles;

	UFUNCTION(BlueprintCallable)
	void AddProjectile(class ABaseCharacter* Owner, class AProjectileActor* Projectile);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<AProjectileActor*> GetProjectiles(ABaseCharacter* Owner);

	UFUNCTION(BlueprintCallable)
	void ClearProjectiles(class ABaseCharacter* Owner);
	UFUNCTION(BlueprintCallable)
	void RemoveProjectile(class AProjectileActor* Projectile);
	UFUNCTION()
	void OnDestroyedProjectile(AActor* DestroyedActor);
};
