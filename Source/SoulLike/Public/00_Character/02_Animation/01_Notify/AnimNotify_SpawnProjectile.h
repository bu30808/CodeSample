// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SpawnProjectile.generated.h"

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
	UPROPERTY(EditAnywhere)
	FName SocketName;


	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UAbilityEffect>> EffectObjects;


	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	
};
