// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityCue.h"
#include "GameplayTagContainer.h"
#include "00_Character/BaseCharacter.h"
#include "00_Character/00_Player/01_Component/AbilityTalentComponent.h"
#include "Engine/DataTable.h"
#include "AbilityBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAbility, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndAbility);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCommitFailed);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOverrideSelfEffect, const TArray<class UAbilityEffect*>&, SelfEffect,
                                             AActor*, Target);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOverrideTargetEffect, const TArray<class UAbilityEffect*>&,
                                             TargetEffect, AActor*, Target);

USTRUCT(BlueprintType)
struct FAbilityInformation : public FTableRowBase
{
	GENERATED_BODY()

public:

	//어빌리티 클래스
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UAbilityBase> AbilityClass;
	
	//어빌리티 이름
	UPROPERTY(EditAnywhere)
	FText AbilityName;

	//어빌리티 태그
	UPROPERTY(EditAnywhere)
	FGameplayTag AbilityTag;

	//어빌리티 요구사항
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UAbilityRequirement> AbilityRequirement;

	//어빌리티 설명
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MultiLine = "true"))
	FText AbilityDescription;

	//어빌리티 이미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> AbilityImage;

	//이 값이 참이어야 인벤토리의 기술목록에 나타납니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bRegisterInventory = false;
	
	FText GetAbilityReqDescription() const;
	//사용가능한 능력치를 가지고 있는지 확인함.
	bool IsAttributeEnough(class AActor* Owner) const;
};


UCLASS(Abstract)
class SOULLIKE_API UAbilityTalent : public UObject
{
	GENERATED_BODY()

protected:
	//피격당하면 호출되는 이벤트
	FOnGotHit OnGotHit;

	//회복량 증감치를 변경하는 이벤트
	FOnChangeHealAmount OnChangeHealAmount;

	//몬스터 처치시 호출되는 이벤트
	FOnKillMonster OnKillMonster;

	//달리기 SP 소모 감소 이벤트
	FOnDecreaseAbilitySP OnDecreaseRunSP;

	//이 오브젝트가 가진 이벤트에 일단 함수를 바인드 합니다.
	virtual void SelfBind() PURE_VIRTUAL(UAbilityTalent::SelfBind);

public:
	//생성한 이벤트는 전부 여기서 바인드 해야 합니다.
	virtual void Bind(class UAbilityTalentComponent* ATComp) PURE_VIRTUAL(UAbilityTalent::Bind);
	//어빌리티가 종료될 때, 모든 바인드된 함수를 제거합니다.
	virtual void UnBind(class UAbilityTalentComponent* ATComp) PURE_VIRTUAL(UAbilityTalent::UnBind);

	//피격당하면 호출됩니다.
	UFUNCTION(BlueprintImplementableEvent)
	void OnGotHitEvent(ABaseCharacter* GotHitCharacter, ABaseCharacter* DamagedBy, float OriginalDamage);

	//체력을 회복하면 호출됩니다.
	UFUNCTION(BlueprintImplementableEvent)
	float OnChangeHealAmountEvent(float RecoverAmount, ABaseCharacter* RecoveredCharacter);

	//몬스터를 처치하면 호출됩니다.
	UFUNCTION(BlueprintImplementableEvent)
	void OnKillMonsterEvent(ABaseCharacter* Killer, class ABaseMonster* KilledMonster);

	UFUNCTION(BlueprintImplementableEvent)
	float OnDecreaseRunSPEvent(float OriginalStamina);
};

UCLASS(Blueprintable)
class SOULLIKE_API UAbilityDamageTalent : public UAbilityTalent
{
	GENERATED_BODY()

	//피해량 증가 이벤트
	FOnIncreaseDamage OnIncreasePhysicalDamage;
	FOnIncreaseDamage OnIncreaseMagicalDamage;

	//최종 피해량 증가 이벤트
	FOnIncreaseDamage OnIncreaseResultDamage;

	//공격에 성공하면 호출되는 이벤트
	FOnSuccessDamageHit OnSuccessDamageHit;
	FOnIncreaseDamage OnSuccessDamageHitIncreasePhysicalDam;
	FOnIncreaseDamage OnSuccessDamageHitIncreaseMagicalDam;

protected:
	virtual void SelfBind() override;

public:
	virtual void Bind(class UAbilityTalentComponent* ATComp) override;
	virtual void UnBind(class UAbilityTalentComponent* ATComp) override;
	//기본 물리공격력을 받아와 증가할 수치를 계산해서 리턴합니다.
	//ex) 10의 피해를 줄 때, 특성으로 10%의 피해가 증가해야 할 경우,
	//리턴값으로 10의 10%인 1의 값이 리턴되어야 합니다.
	UFUNCTION(BlueprintImplementableEvent)
	float OnIncreasePhysicalDamageEvent(float Damage, class ABaseCharacter* DamagedBy,
	                                    ABaseCharacter* DamagedCharacter);
	//기본 마법공격력을 받아와 증가할 수치를 계산해서 리턴합니다.
	//ex) 10의 피해를 줄 때, 특성으로 10%의 피해가 증가해야 할 경우,
	//리턴값으로 10의 10%인 1의 값이 리턴되어야 합니다.
	UFUNCTION(BlueprintImplementableEvent)
	float OnIncreaseMagicalDamageEvent(float Damage, class ABaseCharacter* DamagedBy, ABaseCharacter* DamagedCharacter);

	//최종 피해량증가치를 계산해 리턴합니다.
	UFUNCTION(BlueprintImplementableEvent)
	float OnIncreaseResultDamageEvent(float Damage, class ABaseCharacter* DamagedBy, ABaseCharacter* DamagedCharacter);

	//공격에 성공하면 호출되는 이벤트
	UFUNCTION(BlueprintImplementableEvent)
	void OnSuccessDamageHitEvent(class ABaseCharacter* Player, ABaseCharacter* DamagedCharacter);
	//공격에 성공하면 특성으로 증가될 물리피해량을 가져오는 이벤트
	UFUNCTION(BlueprintImplementableEvent)
	float OnSuccessDamageHitIncreasePhysicalDamEvent(float OriginalDamage, class ABaseCharacter* Player,
	                                                 ABaseCharacter* DamagedCharacter);
	//공격에 성공하면 특성으로 증가될 마법피해량을 가져오는 이벤트
	UFUNCTION(BlueprintImplementableEvent)
	float OnSuccessDamageHitIncreaseMagicalDamEvent(float OriginalDamage, class ABaseCharacter* Player,
	                                                ABaseCharacter* DamagedCharacter);
};

UCLASS(Blueprintable)
class SOULLIKE_API UAbilityDefenceTalent : public UAbilityTalent
{
	GENERATED_BODY()

	//입는 피해량 감소
	FOnDecreaseGotHitDamage OnDecreaseGotHitDamage;
	
	FOnDecreaseGotHitDamage OnDecreasePhysicalGotHitDamage;
	FOnDecreaseGotHitDamage OnDecreaseMagicalGotHitDamage;

	//방어력 증감
	FOnChangeDefence OnChangePhysicalDefence;
	FOnChangeDefence OnChangeMagicalDefence;

	//몬스터 타입에 따라서 방어력 증감
	FOnChangeDefence OnChangeDefenceByMonsterType;

	//입는 피해량 증가
	FOnIncreaseGotHitDamage OnIncreaseGotHitDamage;

	//달리는 속도 증가
	FOnIncreaseRunSpeed OnIncreaseRunSpeed;

	FOnIncreaseMoveSpeed OnIncreaseMoveSpeed;
	FOnDecreaseMoveSpeed OnDecreaseMoveSpeed;

	//회피 SP 소모량 감소
	FOnDecreaseAbilitySP OnDecreaseDodgeSP;
	//회피거리 증가
	FOnIncreaseDodgeDistance OnIncreaseDodgeDistance;

	//SP 회복량 증가
	FOnIncreaseRecoverSPAmount OnIncreaseRecoverSPAmount;

	//무적시간 증가
	FOnIncreaseDodgeInvincibilityTime OnIncreaseDodgeInvincibilityTime;

	//피해량의 일부를 MP로 대체
	FOnGotHitWithReturn OnReplaceDamageToMP;

	//회피 성공시 호출되는 이벤트
	FOnSuccessDodge OnSuccessDodge;

	//회피성공시 다음 피해량 증가
	FOnIncreaseDamWhenSuccDodge OnIncreaseDamWhenSuccDodge;

protected:
	virtual void SelfBind() override;

public:
	virtual void Bind(class UAbilityTalentComponent* ATComp) override;
	virtual void UnBind(class UAbilityTalentComponent* ATComp) override;

protected:
	//입는 피해량 감소
	UFUNCTION(BlueprintImplementableEvent)
	float OnDecreaseGotHitDamageEvent(float Damage, ABaseCharacter* DamageBy, ABaseCharacter* DamagedCharacer);
	UFUNCTION(BlueprintImplementableEvent)
	float OnDecreasePhysicalGotHitDamageEvent(float Damage, class ABaseCharacter* DamagedBy,
	                                          ABaseCharacter* DamagedCharacter);
	UFUNCTION(BlueprintImplementableEvent)
	float OnDecreaseMagicalGotHitDamageEvent(float Damage, class ABaseCharacter* DamagedBy,
	                                         ABaseCharacter* DamagedCharacter);


	//입는 피해량 증가
	UFUNCTION(BlueprintImplementableEvent)
	float OnIncreaseGotHitDamageEvent(float Damage, class ABaseCharacter* DamagedBy, ABaseCharacter* DamagedCharacter);

	//이동속도 증가
	UFUNCTION(BlueprintImplementableEvent)
	float OnIncreaseRunSpeedEvent(const float Original);
	UFUNCTION(BlueprintImplementableEvent)
	float OnIncreaseMoveSpeedEvent(float OriginalSpeed);
	UFUNCTION(BlueprintImplementableEvent)
	float OnDecreaseMoveSpeedEvent(const float Original);

	//회피 SP 감소
	UFUNCTION(BlueprintImplementableEvent)
	float OnDecreaseDodgeSPEvent(float OriginalStamina);

	//회피거리 증가
	UFUNCTION(BlueprintImplementableEvent)
	float OnIncreaseDodgeDistanceEvent(float OriginalForce);

	//SP 회복량 증가
	UFUNCTION(BlueprintImplementableEvent)
	float OnIncreaseRecoverSPAmountEvent(float OriginalRecoverAmount);

	//회피 무적시간 증가
	UFUNCTION(BlueprintImplementableEvent)
	float OnIncreaseDodgeInvincibilityTimeEvent(float OriginalTime);


	//피해를 MP로 대체할 수치
	UFUNCTION(BlueprintImplementableEvent)
	float OnReplaceDamageToMPEvent(float OriginalDamage, class ABaseCharacter* DamagedBy,
	                               ABaseCharacter* DamagedCharacter);

	//회피 성공시 호출될 이벤트
	UFUNCTION(BlueprintImplementableEvent)
	void OnSuccessDodgeEvent(ABaseCharacter* Character);

	//회피 성공시 다음 피해량 증가
	UFUNCTION(BlueprintImplementableEvent)
	float OnIncreaseDamWhenSuccDodgeEvent(APlayerCharacter* Player, float OriginalDamage);


	//물리방어력 증감
	UFUNCTION(BlueprintImplementableEvent)
	float OnChangePhysicalDefenceEvent(float PhysicalDefence, ABaseCharacter* DamagedBy,
	                                   ABaseCharacter* DamagedCharacter);
	//마법방어력 증감
	UFUNCTION(BlueprintImplementableEvent)
	float OnChangeMagicalDefenceEvent(float PhysicalDefence, ABaseCharacter* DamagedBy,
	                                  ABaseCharacter* DamagedCharacter);


	/**
	 * 나에게 피해를 주는 몬스터 타입에 따라 계산된 물리 또는 마법 방어력을 증감함
	 * @param OriginalDefence 방어력 특성이 모두 적용된 후의 최종 물리 혹은 마법 방어력
	 * @param DamagedBy 
	 * @param DamagedCharacter 
	 * @return 증감될 수치
	 */
	UFUNCTION(BlueprintImplementableEvent)
	float OnChangeDefenceByMonsterTypeEvent(float OriginalDefence, ABaseCharacter* DamagedBy,
	                                        ABaseCharacter* DamagedCharacter);
};

UCLASS(Blueprintable)
class SOULLIKE_API UAbilityFreeTalent : public UAbilityTalent
{
	GENERATED_BODY()

	//경험치 획득량 증가
	FOnIncreaseGetExp OnIncreaseGetExp;

	//회피 최소 SP요구량 변경
	FOnDodgeMinimumSP OnDodgeMinimumSP;

protected:
	virtual void SelfBind() override;

public:
	virtual void Bind(class UAbilityTalentComponent* ATComp) override;
	virtual void UnBind(class UAbilityTalentComponent* ATComp) override;

	//획득 경험치량 증가
	UFUNCTION(BlueprintImplementableEvent)
	float OnIncreaseGetExpEvent(float OriginalExp);
	UFUNCTION(BlueprintImplementableEvent)
	float OnDodgeMinimumSPEvent(float OriginalSP);
};

/**
 * 어빌리티를 정의하는 오브젝트 클래스입니다.
 * 어빌리티는 다음과 같은 순서로 사용됩니다.
 *
 *	1. TryActivateAbility (어빌리티 사용 시도)
 *	2. CommitAbility (커밋)
 *	3. ActivateAbility (어빌리티 적용)
 *	4. EndAbility (어빌리티 종료)
 *
 *	각 함수는 블루프린트에서 덮어 쓸 수 있습니다.
 */
UCLASS(Blueprintable, DefaultToInstanced)
class SOULLIKE_API UAbilityBase : public UObject
{
	GENERATED_BODY()
#if WITH_EDITOR
	friend class UAbilityCreatorWidget;
#endif

protected:
	//이 어빌리티가 가지는 고유 태그값입니다. 모든 어빌리티는 절대 중복되지 않는 태그값을 갖습니다.
	//절대 비워두면 안 됩니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag AbilityTag;


	//이 데이터 테이블에 모든 어빌리티에 해당하는 정보가 들어있습니다.
	//절대 비워두면 안 됩니다.
	UPROPERTY(EditAnywhere)
	class UDataTable* AbilityInformationTable;

	/*//이 어빌리티를 사용하기 위해 누른 액션정보입니다.
	UPROPERTY()
	TWeakObjectPtr<class UInputAction> AbilityInputAction;*/

	//이 어빌리티 사용중 연계 가능한 다른 키 액션 리스트입니다. 이 리스트에 있는 액션을 대기하게 됩니다.
	UPROPERTY(EditAnywhere)
	TArray<class UInputAction*> WaitInputAction;

	//어빌리티 사용에 몽타주가 필요한 경우 사용하세요
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<class UAnimMontage*> Montages;

	UPROPERTY(Transient, BlueprintReadOnly)
	int32 MontageIndex;
	//지금 재생중인 몽타주입니다.
	UPROPERTY(VisibleAnywhere)
	class UAnimMontage* CurrentMontage;

	//이 어빌리티를 발동할 때, 강제종료시켜야 할 어빌리티가 있다면 사용하세요.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FGameplayTag> ForceEndAbilityTags;
	//이 어빌리티를 발동할 때, 강제종료시켜야 할 이팩트가 있다면 사용하세요.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FGameplayTag> ForceEndEffectTags;


	UPROPERTY(EditAnywhere)
	bool bUseChainSystem = false;
	//공격 배율을 사용하기 위한 태그입니다. 없으면 배율이 적용되지 않습니다.
	UPROPERTY(EditAnywhere, meta=(EditCondition ="bUseChainSystem"))
	FGameplayTag ChainTag;
	//몽타주 번호에 따른 배율입니다.
	UPROPERTY(EditAnywhere, meta=(EditCondition ="bUseChainSystem"))
	TArray<float> ChainValue;

	//이 값이 참이면, 어빌리티 정보를 이용해 아이콘을 만들어 버프아이콘을 표시합니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bShouldAddBuffIcon = false;
public:
	bool GetUseChainSystem() const { return bUseChainSystem; }
	const FGameplayTag& GetChainTag() const { return ChainTag; };
	void AddBuffIcon();
	void RemoveBuffIcon() const;
protected:
	//사용자 자신에게 적용할 이팩트 정보들
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true), Category="Effect")
	TArray<TSubclassOf<class UAbilityEffect>> SelfEffect;

	//타겟에게 적용할 이팩트 정보들
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true), Category="Effect")
	TArray<TSubclassOf<class UAbilityEffect>> TargetEffect;

	//어빌리티를 발동할 때, 자신에게 적용할 큐 
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true), Category="Cue")
	TArray<FAbilityCueInformation> SelfCue;

	//어빌리티를 발동할 때, 타겟에게 적용할 큐
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true), Category="Cue")
	TArray<FAbilityCueInformation> TargetCue;

	//어빌리티 특성을 가지고 있는 오브젝트입니다.
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UAbilityTalent>> AbilityTalent;

	//적용된 특성의 인스턴스를 저장하는 변수입니다.
	UPROPERTY()
	TArray<class UAbilityTalent*> AbilityTalentInstance;

	//이 어빌리티가 적용한 큐
	UPROPERTY(Transient)
	TArray<class AAbilityCue*> AppliedSelfCue;
	UPROPERTY(Transient)
	TArray<class AAbilityCue*> AppliedTargetCue;


	//컴포넌트에서 강제로 이 어빌리티를 종료한 경우 참이 됩니다.
	UPROPERTY(BlueprintReadWrite)
	bool bAlreadyEndAbility;


	UFUNCTION(BlueprintCallable, BlueprintPure)
	class UAnimMontage* GetNextMontage();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	class UAnimMontage* GetRandomMontage();
	UFUNCTION(BlueprintCallable)
	float PlayMontageWithCustomChain(ABaseCharacter* Target, class UAnimMontage* Montage, float CustomChainValue);

private:
	//SetAbilityInformation함수로 저장되는 이 어빌리티의 기본정보입니다.
	UPROPERTY(VisibleAnywhere)
	FAbilityInformation AbilityInformation;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FAbilityInformation& GetAbilityInformation() { return AbilityInformation; }

	void OverrideTag(FGameplayTag GameplayTag) { AbilityTag = GameplayTag; }
	void SetAlreadyEndByForceEndAbilityReq(bool bCond) { bAlreadyEndAbility = bCond; }

protected:
	//이 어빌리티를 발동한 대상입니다.
	UPROPERTY(BlueprintReadOnly, Transient)
	TWeakObjectPtr<class ABaseCharacter> AbilityOwner;

	//이 어빌리티의 효과를 적용할 대상입니다.
	UPROPERTY(BlueprintReadOnly, Transient)
	TWeakObjectPtr<AActor> AbilityTarget;

	//적용된 이팩트를 수동으로 종료하려면 참을 주세요.
	//아니라면 어빌리티가 종료될 때 같이 종료됩니다.
	UPROPERTY(EditAnywhere)
	bool bEndAppliedEffectManually = false;

	//이 어빌리티가 자신과 타겟에게 적용할 이팩트 인스턴스 리스트
	UPROPERTY(BlueprintReadOnly, Transient)
	TArray<class UAbilityEffect*> SelfEffectInstance;
	UPROPERTY(BlueprintReadOnly, Transient)
	TArray<class UAbilityEffect*> TargetEffectInstance;

public:
	//이 이벤트로 이팩트 내부 값을 덮어 쓸 수 있습니다.
	UPROPERTY(BlueprintAssignable)
	FOnOverrideSelfEffect OnOverrideSelfEffect;
	UPROPERTY(BlueprintAssignable)
	FOnOverrideTargetEffect OnOverrideTargetEffect;

	//이 어빌리티가 적용한 이팩트가 이 어빌리티를 통하지 않고 종료되었을 때, 이 어빌리티내부에서 할 행동을 정의합니다.
	//필요하다면 블루프린트에서 덮어쓰세요.
	UFUNCTION(BlueprintImplementableEvent)
	void OnExpiredSelfEffectEvent();
	UFUNCTION(BlueprintImplementableEvent)
	void OnExpiredTargetEffect();

	

	//외부에서 어빌리티가 종료될 때 무언가 하고싶다면 사용하세요.
	UPROPERTY(BlueprintAssignable)
	FOnEndAbility OnEndAbility;

	//최종적으로 커밋이 실패하면 호출되는 델리게이트입니다.
	UPROPERTY(BlueprintAssignable)
	FOnCommitFailed OnCommitResultFailed;

	//코스트가 모자라서 실패하면 호출됩니다.
	UPROPERTY(BlueprintAssignable)
	FOnCommitFailed OnCommitFailedByCost;
	//쿨타임이 적용중이어서 실패하면 호출됩니다.
	UPROPERTY(BlueprintAssignable)
	FOnCommitFailed OnCommitFailedByCooldown;

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnCommitResultFailedEvent();

	virtual void OnCommitResultFailedEvent_Implementation()
	{
	}

	UFUNCTION(BlueprintNativeEvent)
	void OnCommitFailedByCostEvent();

	virtual void OnCommitFailedByCostEvent_Implementation()
	{
	}

	UFUNCTION(BlueprintNativeEvent)
	void OnCommitFailedByCooldownEvent();

	virtual void OnCommitFailedByCooldownEvent_Implementation()
	{
	}

	//데이터테이블에서 정보를 읽어와 설정합니다.
	UFUNCTION(BlueprintCallable, CallInEditor)
	void SetAbilityInformation();


	//데이터테이블에서 읽어온 정보를 에디터 및 어빌리티 생성시 자동으로 할당되게 합니다.
	virtual void PostInitProperties() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

#endif
	
	
public:
	const FGameplayTag& GetAbilityTag() { return AbilityTag; }


	/**
	 * @brief 어빌리티를 초기화 합니다.
	 *
	 * 어빌리티 타겟과, 어빌리티 사용자를 구분해야 합니다.
	 *
	 *	예를들어 상대방에게 힐을 하는 경우 -> AbilityTarget == 상대방
	 *	어빌리티 사용자 -> 힐을 사용하는 사람
	 *
	 *	단순하게 캐릭터가 검을 휘두르는 어빌리티인 경우
	 *	어빌리티 사용자 == 어빌리티 타겟
	 *	
	 * @param Owner 
	 * @param Target
	 * @param InputAction 
	 */
	void InitAbility(TWeakObjectPtr<ABaseCharacter> Owner,
	                 AActor* Target/*, class UInputAction* InputAction = nullptr*/);


	/**
	 * @brief 어빌리티 사용시도 합니다.
	 * 반드시 CommitAbility를 호출해서 어빌리티를 사용 가능한지 확인해야 합니다.
	 * 커밋에 성공한 경우, ActivateAbility를 호출합니다.
	 * 필요한 경우 인자로 추가 오브젝트를 넘겨주십시오.
	 *
	 * @param AdditionalInfo 추가로 전달할 정보가 있으면 넘겨주세요.
	 */
	UFUNCTION(BlueprintNativeEvent)
	void TryActivateAbility(UObject* AdditionalInfo);

protected:
	/**
	 * @brief 이 어빌리티를 사용 가능한지 확인합니다.
	 * 블루프린트에서 덮어쓸 수 있습니다.
	 * 어빌리티 요구사항이 없다면 무조건 참입니다.
	 * @return 가능하면 참.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CommitAbility();
	virtual bool CommitAbility_Implementation();
	/**
	 * @brief 어빌리티를 발동합니다.
	 * 블루프린트에서 이 함수를 재 정의하는 것으로 어빌리티를 구현하세요.
	 * 명시적으로 EndAbility를 호출하여 어빌리티를 끝내야 합니다.
	 * 필요에 의해 추가 정보를 담은 오브젝트를 파라미터로 받습니다.
	 *
	 * @param AdditionalInfo 추가로 전달할 정보가 있으면 넘겨주세요.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ActivateAbility(UObject* AdditionalInfo);
	virtual void ActivateAbility_Implementation(UObject* AdditionalInfo);

public:
	/**
	 * @brief 어빌리티를 취소합니다.
	 * 기본적으로 이 어빌리티를 종료하게 되어 있습니다.
	 * 추가로 해야 할 일이 있다면 재정의하세요.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CancelAbility();
	virtual void CancelAbility_Implementation();

	/**
	 * @brief 어빌리티를 종료합니다.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EndAbility();
	virtual void EndAbility_Implementation();

	UFUNCTION(BlueprintPure, BlueprintCallable)
	class UAbilityComponent* GetAbilityComponent(class ABaseCharacter* Character);

protected:
	//필요하다면, 어빌리티 오너의 상태를 설정합니다.
	//코드에는 Hit애니메이션 발동을 막기 위한 설정이 되어있습니다.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetAbilityOwnerState();
	virtual void SetAbilityOwnerState_Implementation();

	//어빌리티 요구사항에 할당되어있는 이팩트를 적용합니다.
	//TryActivateAbility 함수 내부에서 호출됩니다.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ApplyEffect();
	virtual void ApplyEffect_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnEffectEndEvent();
	virtual void OnEffectEndEvent_Implementation();
	UFUNCTION(BlueprintCallable)
	void ApplySelfEffect(ABaseCharacter* Character);
	UFUNCTION(BlueprintCallable)
	void ApplyTargetEffect(ABaseCharacter* Character);

	UFUNCTION(BlueprintCallable)
	void EndSelfEffect(ABaseCharacter* Character) const;
	UFUNCTION(BlueprintCallable)
	void EndTargetEffect(ABaseCharacter* Character) const;

	//Self, Target 이펙트를 수동으로 종료할 때 사용하세요.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EndEffect();
	virtual void EndEffect_Implementation();


	//이 어빌리티 사용자의 어빌리티 컴포넌트에 이 어빌리티의 태그를 사용중인 어빌리티태그 변수에 등록합니다.
	UFUNCTION(BlueprintCallable)
	void RegisterAbilityTag() const;
	//코스트를 적용합니다.
	UFUNCTION(BlueprintCallable)
	void ApplyCost();
	//쿨다운을 적용합니다.
	void ApplyCooldown();

	UFUNCTION(BlueprintCallable)
	void ApplyCues();

	//큐를 제거합니다. 재 사용 가능한 큐는 제거하지 않습니다.
	UFUNCTION(BlueprintCallable)
	void RemoveCues();

	//어빌리티를 발동한 키를 다시 눌렀을 때, 무엇인가 하려면 덮어쓰세요.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnPressedActionKey(const FKeyPressedInfo& Action);

	virtual void OnPressedActionKey_Implementation(const FKeyPressedInfo& Action)
	{
	}

	//어빌리티를 발동한 키를 땠을 때, 무엇인가 하려면 덮어쓰세요.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnReleasedActionKey(const FKeyPressedInfo& Action);

	virtual void OnReleasedActionKey_Implementation(const FKeyPressedInfo& Action)
	{
	}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnNotKeyPressed();
	virtual void OnNotKeyPressed_Implementation();
	//이 어빌리티 사용중 특정 키를 눌렀을 때 할 행동을 바인드합니다.
	void BindActionEvent();

	//바인드 한 키 액션을 제거합니다.
	UFUNCTION(BlueprintCallable)
	void UnBindActionEvent() const;
	//특성을 정의하는 어빌리티인 경우, 특성정보를 바인드 합니다. AbilityTalentComponet에 저장됩니다.
	void BindTalent();

public:
	void UnBindTalent();

protected:
	//사망시 호출할 이벤트를 바인드할것인가?
	UPROPERTY(EditAnywhere)
	bool bShouldBindDeadEvent = false;

	void BindDeadEvent();
	void UnBindDeadEvent();

	//어빌리티의 오너가 사망했을때의 처리를 정의합니다.
	//코드상으로는 이 어빌리티를 취소하게 되어있습니다.
	//어빌리티를 종료하지 않기를 원한다면, 덮어쓰세요.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnAbilityOwnerDeadEvent(AActor* Who, AActor* DeadBy, EDeadReason DeadReason);
	virtual void OnAbilityOwnerDeadEvent_Implementation(AActor* Who, AActor* DeadBy, EDeadReason DeadReason);

	//몽타주에 필요한 설정을 합니다
	//ex) 이벤트 바인딩
	UFUNCTION(BlueprintCallable)
	void BindMontageEvent();
	UFUNCTION(BlueprintCallable)
	void UnBindMontageEvent();

	//몽타주가 끝났을때 사용할 이벤트
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnMontageBlendingOutEvent(UAnimMontage* Montage, bool bInterrupted);

	virtual void OnMontageBlendingOutEvent_Implementation(UAnimMontage* Montage, bool bInterrupted)
	{
	}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnMontageEndedEvent(UAnimMontage* Montage, bool bInterrupted);

	virtual void OnMontageEndedEvent_Implementation(UAnimMontage* Montage, bool bInterrupted)
	{
	}


	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsCostEnough();
	

	UFUNCTION(BlueprintCallable)
	AActor* SpawnActor(TSubclassOf<class AActor> ActorObject);

	/**
	 * 이 어빌리티를 사용하기 전, 종료해야 할 어빌리티들을 종료합니다.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ForceEndAbilities();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ForceEndEffects();

	UFUNCTION(BlueprintCallable)
	void HandleOnTriggerHitAnimationNotEntered();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetOwnersActionSpeed();


	virtual UWorld* GetWorld() const override;
};
