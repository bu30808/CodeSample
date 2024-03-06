// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_AddForce.generated.h"
//Common.PlayerInput.Ignore.Move.FromAnimationNotify
enum class EDirection : uint8;

UENUM(BlueprintType)
enum class EForceApplicationMode : uint8
{
	// 입력 방향을 기준으로 힘을 가함
	InputDirection UMETA(DisplayName = "Input Direction"),
	//ForceDirection 변수에 따라서 힘을 가함
	CustomDirectionSetting UMETA(DisplayName = "Custom Direction Setting")
};

/**
 * 특정 구간에 수동으로 힘을 가하려면 사용하세요
 * 플레이어 캐릭터 전용입니다. 다른 캐릭터에도 사용하려면 코드를 수정해야 합니다.
 */
UCLASS()
class SOULLIKE_API UAnimNotifyState_AddForce : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UAnimNotifyState_AddForce();

protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	                         const FAnimNotifyEventReference& EventReference) override;
	void StopMovementWhenFalling();
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
	                        const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                       const FAnimNotifyEventReference& EventReference) override;

	bool IsFallOfTheCliff() const;

	UPROPERTY()
	TWeakObjectPtr<class ABaseCharacter> Owner;
	//이 값이 참이면 절벽검사를 합니다.
	UPROPERTY(EditAnywhere)
	bool bCheckCliff = true;
	//어느 기준으로 힘을 가할것인지?
	UPROPERTY(EditAnywhere)
	EForceApplicationMode ForceApplicationMode;
	//미는 방향
	UPROPERTY(EditAnywhere,
		meta=(EditCondition = "ForceApplicationMode == EForceApplicationMode::CustomDirectionSetting"))
	EDirection ForceDirection;
	//참이면 컨트롤 로테이션을 기준으로 방향을 구합니다.
	UPROPERTY(EditAnywhere,
	meta=(EditCondition = "ForceApplicationMode == EForceApplicationMode::CustomDirectionSetting"))
	bool bUseControllerDir = true;

	/*//이 값이 참이면, 무브먼트의 속력수치를 사용합니다. 거짓이라면 플레이어의 입력 방향백터를 이용하게 됩니다.
	UPROPERTY(EditAnywhere,meta=(EditCondition = "ForceApplicationMode == EForceApplicationMode::InputDirection"))
	bool bUseMovementVelocity = false;*/
	UPROPERTY(EditAnywhere)
	float Power = 1000000;
	UPROPERTY(EditAnywhere)
	bool bVelocityChange;
	UPROPERTY(EditAnywhere)
	bool bDisableMoveInput = true;
	UPROPERTY()
	FVector MoveDirectionVector;


	void SaveStartControllerInput();
	void ApplyForceByInputDirection(float DeltaTime) const;
	void ApplyForceByDirectionValue(float DeltaTime) const;
	float ApplyTalent() const;

	bool IsPlayerCharacter() const;
	FVector GetMoveDirFromForceDirectionValue();
	void SaveMoveDirection();
};
