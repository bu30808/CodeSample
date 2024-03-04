// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_FootStep.generated.h"


/**
 * 발소리 관련 처리를 담당하는 노티파이입니다.
 */
UCLASS()
class SOULLIKE_API UAnimNotify_FootStep : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                    const FAnimNotifyEventReference& EventReference) override;


	UPROPERTY(EditAnywhere)
	FName SocketName;
};
