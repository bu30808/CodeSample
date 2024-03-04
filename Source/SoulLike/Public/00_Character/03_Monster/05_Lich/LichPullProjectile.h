// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "02_Ability/01_Task/GameplayTask_LaunchEvent.h"
#include "02_Ability/02_Projectile/ProjectileActor.h"
#include "LichPullProjectile.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API ALichPullProjectile : public AProjectileActor
{
	GENERATED_BODY()

public:
	ALichPullProjectile();

	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* ProjectileParticle;
	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* TrailParticle;

	UPROPERTY(EditAnywhere)
	FAbilityCueInformation CueInformation;
	UPROPERTY()
	TWeakObjectPtr<class ABaseCharacter> Target;
	UPROPERTY()
	TWeakObjectPtr<class UGameplayTask_LaunchEvent> Task;

	UPROPERTY(EditAnywhere)
	float PullForce = 80000.f;

	UFUNCTION()
	void OnTaskTickEvent();

	UFUNCTION()
	void OnHiEvent(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	virtual void PostInitializeComponents() override;
};
