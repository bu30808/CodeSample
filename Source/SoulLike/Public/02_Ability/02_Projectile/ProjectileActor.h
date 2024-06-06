// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "02_Ability/AbilityCue.h"
#include "GameFramework/Actor.h"
#include "ProjectileActor.generated.h"

UENUM(BlueprintType)
enum class EProjectileDirection : uint8
{
	//오너의 전방으로 날아갑니다.
	Forward,
	//블랙보드 타겟을 향해 회전 후 날아갑니다.
	BlackboardTarget
};


UENUM(BlueprintType)
enum class EProjectileShootType:uint8
{
	//즉시 발사되며 회전이 속도를 따릅니다.
	Immediately,
	//즉시 발사되지만, 회전이 속도를 따르진 않습니다.
	ImmediatelyButNotRotationFollowsVelocity,
	//타겟을 바라볼때까지 회전한 후, 발사됩니다.
	DelayUntilRotation,
	//타겟을 바라보기 위해 회전하지만, 시간초과가 된 경우, 그대로 발사합니다.
	DelayUntilRotationWithTimeLimit
};

UENUM(BlueprintType)
enum class EHomingProjectileType :uint8
{
	Homing,
	//유도타입의 프로젝타일이나, 타겟에 너무 근접하면 유도가 꺼집니다.
	HomingButStopTooClosed,
	//일정 범위 안에 들면 유도됩니다.
	HomingWhenClosed,
	//일정범위안에 들면 유도되나, 너무 가까우면 유도가 꺼집니다.
	HomingWhenClosedButStopTooClosed
};

UCLASS()
class SOULLIKE_API AProjectileActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileActor();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* SphereComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UProjectileMovementComponent* ProjectileMovementComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ProjectileSpeed = 2500.f;
	//각종 어빌리티 이팩트가 들어있습니다.
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UAbilityEffect>> ProjectileEffects;

	UPROPERTY()
	TWeakObjectPtr<class UAbilityCueAdditionalInformation> CueAdditionalInformation;
	UPROPERTY(BlueprintReadWrite)
	EProjectileDirection CurProjectileDirectionType;
	UPROPERTY(BlueprintReadWrite)
	EProjectileShootType CurProjectileShootType;
	UPROPERTY(BlueprintReadWrite)
	EHomingProjectileType CurHomingProjectileType;
	
	UPROPERTY(Transient)
	bool bRotationComplete;
	//EProjectileShootType이 DelayUntilRotation인 경우 사용될 보간 속도입니다.
	UPROPERTY(EditAnywhere)
	float InterpSpeed = 5.f;
	UPROPERTY(Transient)
	FRotator TargetRotation;
	//BaesCharacter를 상속받지 않은 대상들과 충돌했을 때, 사용되는 큐입니다.
	UPROPERTY(EditAnywhere)
	FAbilityCueInformation VisibilityHitCue;

	UPROPERTY()
	FTimerHandle DelayLaunchTimerHandle;
	FTimerHandle ForceShootTimerHandle;

	UPROPERTY(Transient)
	float TargetMinimumHomingDistance;


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;
	void DetachifAttached();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	class UProjectileMovementComponent* GetProjectileMovementComponent() const { return ProjectileMovementComponent; }

	/*void LaunchProjectile(const FVector& ForwardVector, EProjectileType ProjectileType, AActor* HomingTarget = nullptr, float MinimumHomingDistance = 0);*/
	UFUNCTION(BlueprintCallable)
	void LaunchProjectile(const FVector& ForwardVector);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void LaunchHomingProjectile(const FVector& ForwardVector,EHomingProjectileType HomingProjectileType,AActor* HomingTarget,float MinimumHomingDistance);
	virtual void LaunchHomingProjectile_Implementation(const FVector& ForwardVector,EHomingProjectileType HomingProjectileType,AActor* HomingTarget,float MinimumHomingDistance);
	
	void ShootSetting(EProjectileDirection ProjectileDirection);

	//P_ShootType이 DelayUntilRotationWithTimeLimit일 경우 LimitTime시간 이후 강제로 발사합니다.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void LaunchProjectileWithOption(EProjectileDirection P_Direction, EProjectileShootType P_ShootType, float LimitTime);
	virtual void LaunchProjectileWithOption_Implementation(EProjectileDirection P_Direction,
	                                                       EProjectileShootType P_ShootType, float LimitTime);

	UFUNCTION()
	void ForceShoot();
	
	//타이머를 이용해서 n초이후에 발사하는 함수입니다.
	//시간값이 0이라면, 옵션 적용후 즉시 발사합니다.
	//P_ShootType이 DelayUntilRotationWithTimeLimit일 경우 LimitTime시간 이후 강제로 발사합니다.
	UFUNCTION(BlueprintCallable)
	void LaunchProjectileDelayWithOption(float DelayTime, EProjectileDirection P_Direction,
	                                     EProjectileShootType P_ShootType, float LimitTime);

	UFUNCTION(BlueprintCallable)
	void SetEffect(const TArray<TSubclassOf<UAbilityEffect>>& Effects);

	UFUNCTION(BlueprintCallable)
	void ApplyEffects(class ABaseCharacter* HitTarget, FVector HitLocation, FVector ImpactNormal);


	//틱마다 호출되는 라인트레이스 히트 결과를 처리하는 함수입니다.
	UFUNCTION(BlueprintCallable)
	void ProcessHit(const TArray<FHitResult>& Hits, bool bManualDestroy = false);

	UFUNCTION(BlueprintCallable,BlueprintPure)
	class UAbilityComponent* GetOwnersAbilityComponent();

	UFUNCTION(BlueprintCallable)
	void ApplyCue(const FAbilityCueInformation& CueInformation);
};
