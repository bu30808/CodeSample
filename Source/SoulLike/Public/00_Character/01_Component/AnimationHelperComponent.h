// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "00_Character/02_Animation/00_NotifyState/AnimNotifyState_AddForce.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "AnimationHelperComponent.generated.h"

UENUM(BlueprintType)
enum class EDeadAnimationPlayMode : uint8
{
	Sequence,
	Montage,
	None
};

UENUM(BlueprintType)
enum class EHitAnimationType : uint8
{
	//애니메이션 시퀀스를 이용함. 아직 사용처 및 기능이 없습니다. 몽타주 방식을 이용하세요.
	AnimSequence,
	//몽타주를 이용한 애니메이션 사용
	AnimMontage,
	//피격 애니메이션 없음
	NoHitAnimation
};


//트리거 애니메이션 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTriggerHitAnimation, class ABaseCharacter*, DamagedCharacter,
											 class AActor*, HitBy);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULLIKE_API UAnimationHelperComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAnimationHelperComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void PostInitProperties() override;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class ABaseCharacter> ComponentOwnerCharacter;

	/**************************넉다운*************************/
	//넉다운시 사용되는 몽타주 정보
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="KnockDown")
	UAnimMontage* KnockdownMontage;
	//넉다운이 끝나고 일어났을 때 사용되는 몽타주 정보
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="KnockDown")
	UAnimMontage* GetUpMontage;
	//이 배열에 있는 어빌리티를 넉다운 적용 직전 전부 종료시킵니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="KnockDown")
	TArray<FGameplayTag> KnockDownForceEndAbilityTags;
	
	
	/**************************피격처리*************************/
public:
	UPROPERTY(BlueprintAssignable, Category="Hit")
	FOnTriggerHitAnimation OnTriggerHitAnimationEnter;
	UPROPERTY(BlueprintAssignable, Category="Hit")
	FOnTriggerHitAnimation OnTriggerHitAnimationExit;
protected:
	UPROPERTY(Transient, VisibleAnywhere, Category="Hit")
	bool bIsTriggeredHitAnimationExitEvent = false;
public:
	//이 값으로 애니메이션 인스턴스 내부에서 블랜드 스페이스 값을 변경합니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Transient, Category="Hit")
	float HitDegree = 0.f;

	UPROPERTY(EditAnywhere,Category="Animation")
	EHitAnimationType HitAnimationType;
	//이 값이 참이면 히트 몽타주 재생과 동시에 MightyTime동안 슈퍼아머를 부여합니다.
	//연속으로 히트 애니메이션 재생으로 몬스터가 아무것도 못 하는것을 방지하기 좋습니다.
	//오너의 DefaultAbility 변수에 Mighty 어빌리티가 포함되어 있어야 합니다.
	UPROPERTY(EditAnywhere,Category="Animation",meta=(EditCondition = "HitAnimationType == EHitAnimationType::AnimMontage"))
	bool bUseHitMighty = false;
	UPROPERTY(EditAnywhere,Category="Animation",meta=(EditCondition = "HitAnimationType == EHitAnimationType::AnimMontage && bUseHitMighty"))
	float HitMightyTime = 3.f;
	
	UPROPERTY()
	FTimerHandle HitMightyTimerHandle;
	//슈퍼아머를 부여합니다.
	UFUNCTION(BlueprintCallable)
	void ActivateHitMightyAbility(float MightyDeactivateTime);
	


	//방향별 몽타주를 사용하려면 참을 주세요.
	UPROPERTY(EditAnywhere,Category="Animation",meta=(EditCondition = "HitAnimationType == EHitAnimationType::AnimMontage"))
	bool bUseDirection = true;
	//히트 애니메이션 처리를 몽타주로 하려면 사용하세요.
	//ex) 때릴때마다, 히트 애니메이션을 다시 재생하려면 이 변수를 사용합니다.
	UPROPERTY(EditAnywhere,Category="Animation",meta=(EditCondition = "HitAnimationType == EHitAnimationType::AnimMontage && bUseDirection == true"))
	TMap<EDirection, UAnimMontage*> HitMontageByDirection;
	UPROPERTY(EditAnywhere,Category="Animation",meta=(EditCondition = "bUseDirection == false"))
	UAnimMontage* HitMontage;
	
	//참이면 다시 히트될때마다 재생중이던 몽타주를 정지합니다.
	UPROPERTY(EditAnywhere,Category="Animation",meta=(EditCondition = "HitAnimationType == EHitAnimationType::AnimMontage"))
	bool bResetHitMontageWhenHit;

	void PlayHitMontage();
	void PlayHitMontageByDirection();

	UPROPERTY(EditAnywhere,Category="Animation")
	UAnimMontage* GuardHitMontage;
	//가드중일떄 피격당하면 호출됩니다.
	void PlayGuardHitMontage();

	/**
	 * HitMontages에 포함된 몽타주가 지금 재생중인가요?
	 * @return 
	 */
	bool IsPlayHitMontage();
	bool IsPlayGuardHitMontage() const;

	//이 몽타주가 HitMontage중 하나인가요?
	bool IsHitMontage(const UAnimMontage* Montage);
	bool IsGuradHitMontage(const UAnimMontage* Montage);

	//플레이중이던 히트 몽타주가 끝나면 호출됩니다.
	UFUNCTION()
	void OnHitMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnHitMontageBlendOut(UAnimMontage* Montage, bool bInterrupted);
	
	//이 값이 참이면 본 트렌스폼을 변경하는코드를 실행합니다.
	UPROPERTY(Transient, BlueprintReadOnly)
	bool bModifySkeletonTransform;
	
	//캐릭터의 허리 회전을 조정하기 위한 변수입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Transient, Category="AnimInstance")
	FRotator SpineRotation;
	/**
	 * OnTriggerHitAnimationExit델리게이트가 브로드케스트 되었는지 확인합니다.
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsTriggeredHitAnimationExitEvent() { return bIsTriggeredHitAnimationExitEvent; }
	UFUNCTION(BlueprintCallable)
	void SetIsTriggeredHitAnimationExitEvent(bool newVal){bIsTriggeredHitAnimationExitEvent = newVal;}

	void StartDeadDissolve();


	/**************************사망처리*************************/
	//사망시 플레이할 애니메이션들입니다.
	//몽타주와 애니메이션애셋중 하나를 골라서 사용하도록 하세요.
	UPROPERTY(EditAnywhere, Category=Default)
	EDeadAnimationPlayMode DeadAnimationPlayMode = EDeadAnimationPlayMode::None;
	UPROPERTY(EditAnywhere, Category=Default,
		meta=(EditCondition="DeadAnimationPlayMode == EDeadAnimationPlayMode::Montage"))
	TArray<class UAnimMontage*> DeadMontages;
	UPROPERTY(EditAnywhere, Category=Default,
		meta=(EditCondition="DeadAnimationPlayMode == EDeadAnimationPlayMode::Sequence"))
	TArray<class UAnimationAsset*> DeadAnimations;

	//플레이 모드에 따라 사망 애니메이션을 재생합니다.
	UFUNCTION()
	void PlayDeadAnimationByMode();

	void PlayDeadAnimationSequence();
	UAnimMontage* GetRandomDeadAnimationMontage();
	
	
	UPROPERTY(EditAnywhere, Category = "Timeline")
	class UCurveFloat* DissolveCurve;
	//디졸브를 적용할 시간을 결정합니다.
	UPROPERTY(EditAnywhere, Category = "Timeline")
	float DissolveTime = 3.f;
	UPROPERTY(EditAnywhere, Category = "Timeline")
	UNiagaraSystem* DissolveParticle;
	UPROPERTY(EditAnywhere, Category = "Timeline")
	FLinearColor DissolveColor;

	FOnTimelineFloat UpdateDissolve;
	FOnTimelineEvent OnFinishDissolve;
	
	bool CanApplyRagdoll();
};
