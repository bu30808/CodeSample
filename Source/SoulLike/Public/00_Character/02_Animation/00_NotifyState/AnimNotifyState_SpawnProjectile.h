// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_SpawnProjectile.generated.h"

enum class EProjectileDirection : uint8;
enum class EProjectileShootType : uint8;
/**
 * 비긴에 스폰
 * 엔드에 프로젝타일 스타트
 */
UCLASS()
class SOULLIKE_API UAnimNotifyState_SpawnProjectile : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
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
	EAttachmentRule AttachmentRule;
	UPROPERTY(EditAnywhere)
	EDetachmentRule DetachmentRule;

	UAnimNotifyState_SpawnProjectile();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	                         const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                       const FAnimNotifyEventReference& EventReference) override;
};
