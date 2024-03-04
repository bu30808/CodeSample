// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ChangeMaterialParam.generated.h"

/**
 * 특정 구간에 머터리얼 파라미터를 변경해야 할 경우 사용하세요.
 */
UCLASS()
class SOULLIKE_API UAnimNotify_ChangeMaterialParam : public UAnimNotify
{
	GENERATED_BODY()

protected:
	UAnimNotify_ChangeMaterialParam();

	UPROPERTY(EditAnywhere)
	FName ParamName = NAME_None;
	UPROPERTY(EditAnywhere)
	float Value = 0;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                    const FAnimNotifyEventReference& EventReference) override;
};
