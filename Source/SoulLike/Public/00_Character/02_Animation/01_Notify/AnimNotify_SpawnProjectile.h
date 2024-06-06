// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SpawnProjectile.generated.h"

enum class EProjectileType : uint8;
enum class EProjectileShootType : uint8;
enum class EProjectileDirection : uint8;
/**
 * 
 */
UCLASS()
class SOULLIKE_API UAnimNotify_SpawnProjectile : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_SpawnProjectile();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileActor> ProjectileObject;

	//이 값이 참이면 블랙보드의 Target에 저장된 대상을 향해서 이동하게 만듭니다.
	UPROPERTY(EditAnywhere)
	bool bUseBlackboard;
	UPROPERTY(EditAnywhere, meta=(EditCondition="bUseBlackboard"))
	EProjectileDirection ProjectileDirection;
	UPROPERTY(EditAnywhere)
	EProjectileShootType ProjectileShootType;
	//ProjectileShootType의 값이 DelayUntilRotationWithTimeLimit일때, 이 시간 이후에 강제발사합니다.
	UPROPERTY(EditAnywhere,meta=(EditCondition="ProjectileShootType == EProjectileShootType::DelayUntilRotationWithTimeLimit"))
	float TimeLimit;
	UPROPERTY(EditAnywhere)
	FName SocketName;


	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UAbilityEffect>> EffectObjects;


	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	
};
