// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseAnimInstance.generated.h"

enum class ECombatState : uint8;
enum class ECharacterState : uint8;
/**
 * 가장 기본이 되는 애니메이션 인스턴스.
 * 파라곤 애셋의 애니메이션 인스턴스를 코드로 옮겼을 뿐입니다.
 * 필요하다면 상속받아서 재 구현 하세요.
 */
UCLASS()
class SOULLIKE_API UBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	//이 인스턴스의 오너의 BeginPlay가 호출될 때 같이 호출됩니다.
	virtual void NativeBeginPlay() override;
	//애니메이션 초기화 이벤트
	virtual void NativeInitializeAnimation() override;
	//틱마다 호출되는 업데이트 이벤트
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;


	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float Speed;
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	bool IsFalling;
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float Pitch;
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float Roll;
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float Yaw;

	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	TWeakObjectPtr<class ABaseCharacter> Character;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float Direction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	FVector Velocity;

	//피격 당했으면 참.
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	ECharacterState CharacterState;
	//피격 당한 좌표와의 각도
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float HitDegree;

	UPROPERTY(Transient, BlueprintReadOnly)
	bool bModifyBoneTransform;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BoneTransformLerpSpeed = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bShouldMove;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool bMirrorAnimation = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ECombatState CombatState;
public:
	bool IsMirrorAnimation() const {return bMirrorAnimation;}
	UFUNCTION(BlueprintCallable)
	void SetMirrorAnimation(bool bValue){bMirrorAnimation = bValue;}

	
	UFUNCTION(BlueprintNativeEvent)
	void AnimNotify_OnHitEnter();
	virtual void AnimNotify_OnHitEnter_Implementation();
	UFUNCTION(BlueprintNativeEvent)
	void AnimNotify_OnHitExit();
	virtual void AnimNotify_OnHitExit_Implementation();

protected:
	UFUNCTION(BlueprintNativeEvent)
	void ChangeBoneTransform(float DeltaTime);

	virtual void ChangeBoneTransform_Implementation(float DeltaTime)
	{
	}

	UFUNCTION(BlueprintNativeEvent)
	void ClearBoneTransform(float DeltaTime);

	virtual void ClearBoneTransform_Implementation(float DeltaTime)
	{
	}
};
