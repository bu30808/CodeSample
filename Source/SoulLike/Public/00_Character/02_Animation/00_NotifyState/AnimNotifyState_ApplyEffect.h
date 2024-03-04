// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_ApplyEffect.generated.h"


/**
 * 특정 구간에 어빌리티 이팩트를 적용하려면 사용하세요
 */
UCLASS()
class SOULLIKE_API UAnimNotifyState_ApplyEffect : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
	UAnimNotifyState_ApplyEffect();

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UAbilityEffect>> EffectObjects;


	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	                         const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                       const FAnimNotifyEventReference& EventReference) override;


	virtual FString GetEditorComment() override { return TEXT("시작에 어빌리티 이팩트를 적용하고, 끝날때 해제합니다."); }
};
