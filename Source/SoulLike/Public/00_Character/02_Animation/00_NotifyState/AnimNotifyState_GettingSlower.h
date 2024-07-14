// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Components/TimelineComponent.h"
#include "AnimNotifyState_GettingSlower.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UAnimNotifyState_GettingSlower : public UAnimNotifyState
{
	GENERATED_BODY()
protected:


	UPROPERTY(EditAnywhere)
	class UCurveFloat* SpeedCurve;

	FOnTimelineFloat UpdateSpeed;
	FOnTimelineEvent OnFinishSpeed;
	UPROPERTY(Transient)
	TObjectPtr<class UTimelineComponent> TimelineComp;
	UPROPERTY(Transient)
	TObjectPtr<class ABaseCharacter> Character;

	UAnimNotifyState_GettingSlower();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
	UFUNCTION()
	void OnUpdateSpeedTimeLine(float Output);
	UFUNCTION()
	void OnFinishSpeedTimeLine();
};
