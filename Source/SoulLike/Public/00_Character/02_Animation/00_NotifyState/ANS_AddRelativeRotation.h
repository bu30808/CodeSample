// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_AddRelativeRotation.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UANS_AddRelativeRotation : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
	//노티파이 구간동안 더하고 싶은 총 상대 로테이션
	UPROPERTY(EditAnywhere)
	FRotator RelativeRotationToWantTotalAdd;
	//이 값이 참이면 노티파이가 끝날때 원래 로테이션으로 되돌립니다.
	UPROPERTY(EditAnywhere)
	bool bShouldRestoreWhenNotifyEnd = false;

	UPROPERTY()
	float TotalNotifyTime;
	UPROPERTY()
	FRotator OriginalRelativeRot;

	UANS_AddRelativeRotation();
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	                         const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
	                        const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                       const FAnimNotifyEventReference& EventReference) override;
};
