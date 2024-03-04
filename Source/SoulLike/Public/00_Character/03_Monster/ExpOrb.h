// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/BaseCharacter.h"
#include "02_Ability/AbilityCue.h"
#include "GameFramework/Actor.h"
#include "ExpOrb.generated.h"

/*
 *	몬스터가 사망했을 떄, 경험치를 플레이어에게 전달하는 프로젝타일 오브입니다.
 *	이 프로젝타일이 플레이어와 접촉하면, 플레이어는 경험치를 획득합니다.
 *	접촉하기 전에 비 정상적으로 종료되거나 해도, 플레이어에게 경험치를 제공합니다.
 */
UCLASS()
class SOULLIKE_API AExpOrb : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AExpOrb();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* SphereComponent;
	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* NiagaraComponent;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	//이 오브젝트가 가지고 있는 경험치
	UPROPERTY(VisibleAnywhere,Transient)
	float Exp = 0;

	//플레이어가 계속 움직이는 경우, 유도 프로젝타일이라도 제대로 따라가지 못하고 빙빙 도는 경우가 있기 때문에,
	//타이머를 이용해서 중간중간 멈추고 다시 플레이어를 따라가도록 합니다.
	UPROPERTY()
	FTimerHandle HomingResetTimerHandle;

	//플레이어와 접촉했을때 사용할 큐 정보
	UPROPERTY(EditAnywhere)
	FAbilityCueInformation OverlapExplosionCueInfo;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnActorBeginOverlapEvent(AActor* OverlappedActor, AActor* OtherActor);

public:
	void SetExp(float value) { Exp = value; }

	virtual void Destroyed() override;

	/**
	 * 주어진 대상을 따라가게 합니다.
	 * @param Target 따라갈 대상
	 */
	void StartProjectile(ABaseCharacter* Target);
};
