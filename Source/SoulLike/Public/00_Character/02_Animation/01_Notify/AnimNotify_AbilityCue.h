// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "02_Ability/AbilityCue.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_AbilityCue.generated.h"

/**
 * 특정 구간에 어빌리티 큐를 적용해야 할 경우 사용하세요.
 */
UCLASS()
class SOULLIKE_API UAnimNotify_AbilityCue : public UAnimNotify
{
	GENERATED_BODY()

protected:
	UAnimNotify_AbilityCue();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                    const FAnimNotifyEventReference& EventReference) override;
	//virtual void BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload) override;

	virtual FString GetEditorComment() override;


	UPROPERTY(EditAnywhere)
	FAbilityCueInformation CueInformation;
};
