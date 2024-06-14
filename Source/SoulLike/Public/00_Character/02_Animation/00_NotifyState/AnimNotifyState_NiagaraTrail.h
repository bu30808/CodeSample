// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyState_TimedNiagaraEffect.h"
#include "AnimNotifyState_NiagaraTrail.generated.h"

/**
 * 특정 구간동안 나이아가라 트레일 이팩트가 필요하다면 사용하세요
 */
UCLASS()
class SOULLIKE_API UAnimNotifyState_NiagaraTrail : public UAnimNotifyState_TimedNiagaraEffect
{
	GENERATED_BODY()

protected:
	UAnimNotifyState_NiagaraTrail();

	//이 값이 참이면 ILinetraceAbilityEffectInterface인터페이스 함수를 호출합니다.
	UPROPERTY(EditAnywhere)
	bool bUseOtherComponent = false;
	UPROPERTY(EditAnywhere)
	FName Start;
	UPROPERTY(EditAnywhere)
	FName End;
	UPROPERTY(EditAnywhere)
	float Width = 5.f;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
	                        const FAnimNotifyEventReference& EventReference) override;
};
