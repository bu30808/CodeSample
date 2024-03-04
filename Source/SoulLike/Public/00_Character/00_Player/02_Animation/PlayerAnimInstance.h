// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/00_Player/01_Component/LadderMovementComponent.h"
#include "00_Character/02_Animation/BaseAnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UPlayerAnimInstance : public UBaseAnimInstance
{
	GENERATED_BODY()
	
	//이 값이 변경되면 애니메이션 블루프린트 내부 본 트렌스폼 변경에 사용됩니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	FRotator SpineRotation;
	//사다리를 타는 중이라면 참입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	bool bUseLadder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	FVector LadderRightHandLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	FVector LadderLeftHandLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class ULadderMovementComponent> LadderMovementComponent;

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	virtual void ChangeBoneTransform_Implementation(float DeltaTime) override;

protected:
	UFUNCTION(BlueprintNativeEvent)
	void AnimNotify_OnLadderIdleEnter();
	virtual void AnimNotify_OnLadderIdleEnter_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void AnimNotify_OnLadderIdleExit();
	virtual void AnimNotify_OnLadderIdleExit_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void AnimNotify_OnLadderRightExit();
	virtual void AnimNotify_OnLadderRightExit_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void AnimNotify_OnLadderLeftExit();
	virtual void AnimNotify_OnLadderLeftExit_Implementation();
};
