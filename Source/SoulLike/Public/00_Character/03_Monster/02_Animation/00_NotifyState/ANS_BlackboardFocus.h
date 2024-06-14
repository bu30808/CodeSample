// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_BlackboardFocus.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UANS_BlackboardFocus : public UAnimNotifyState
{
	GENERATED_BODY()
protected:
	UANS_BlackboardFocus();
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere)
	float SmoothFocusInterpSpeed = 30.f;
	UPROPERTY(EditAnywhere)
	bool bUseControllerRotationYaw = true;
};
