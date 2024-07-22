// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "02_Ability/AbilityEffect.h"
#include "Components/ActorComponent.h"
#include "AbilityTalentComponent.generated.h"

class ABaseMonster;
class APlayerCharacter;
enum class EAttackType : uint8;
//공격력을 받아와 증가시켜 리턴시켜주는 이벤트입니다.
//멀티케스트가 아니기 때문에 한번에 한개의 함수만 바인드 가능합니다.
//피해량, 피해를 준 대상, 피해를 입은 대상
/*DECLARE_DELEGATE_RetVal_ThreeParams(float, FOnIncreaseDamage, const float, class ABaseCharacter*, ABaseCharacter*);*/
DECLARE_DYNAMIC_DELEGATE_RetVal_ThreeParams(float, FOnIncreaseDamage, const float, Damage, class ABaseCharacter*,
                                            DamageBy, ABaseCharacter*, DamagedCharacer);

//받는 피해량이 감소하는 이벤트
DECLARE_DYNAMIC_DELEGATE_RetVal_ThreeParams(float, FOnDecreaseGotHitDamage, const float, Damage, class ABaseCharacter*,
                                            DamageBy, ABaseCharacter*, DamagedCharacer);

//방어력을 변경하는 이벤트
//방어력, 피해를 준 대상, 피해를 입은 대상
DECLARE_DYNAMIC_DELEGATE_RetVal_ThreeParams(float, FOnChangeDefence, const float, Damage, class ABaseCharacter*,
                                            DamageBy,
                                            ABaseCharacter*, DamagedCharacer);

//입는 피해량이 증가하는 이벤트
DECLARE_DYNAMIC_DELEGATE_RetVal_ThreeParams(float, FOnIncreaseGotHitDamage, const float, Damage, class ABaseCharacter*,
                                            DamageBy,
                                            ABaseCharacter*, DamagedCharacer);

//달리는 속도 증가
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(float, FOnIncreaseRunSpeed, const float, Original);
//이동속도 증감
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(float, FOnIncreaseMoveSpeed, const float, Original);
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(float, FOnDecreaseMoveSpeed, const float, Original);

//SP 소모 감소
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(float, FOnDecreaseAbilitySP, const float, Origianl);

//회피거리 증가(회피시 가해지는 힘의 크기를 증가시킵니다)
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(float, FOnIncreaseDodgeDistance, const float, Original);

//SP회복량 증가
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(float, FOnIncreaseRecoverSPAmount, const float, Original);

//회피 무적시간 증가.
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(float, FOnIncreaseDodgeInvincibilityTime, const float, Original);

//경험치 획득량 증가
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(float, FOnIncreaseGetExp, const float, Original);

//피격시 호출되는 이벤트, 피격당한 플레이어/타격한 대상/피해량
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnGotHit, ABaseCharacter*, DamagedCharacer, ABaseCharacter*, DamagedBy,
                                     const float, Damage);

//피격시 호출되는 이벤트, 리턴이 필요할때 사용하세요.
DECLARE_DYNAMIC_DELEGATE_RetVal_ThreeParams(float, FOnGotHitWithReturn, const float, Damage, class ABaseCharacter*,
                                            DamagedBy, ABaseCharacter*, DamagedCharacer);

//회피에 성공하면 실행될 이벤트
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSuccessDodge, ABaseCharacter*, Player);

//회피에 성공하면 다음 피해량이 증가
DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(float, FOnIncreaseDamWhenSuccDodge, APlayerCharacter*, Player, const float,
                                          Origianl);

//공격이 적중하면 사용할 이벤트
//피해량, 피해를 준 대상(플레이어), 피해를 입은 대상
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnSuccessDamageHit, class ABaseCharacter*, DamagedBy, ABaseCharacter*,
                                   DamagedCharacer);


//치유효과 증감
//치유량 / 치유를 받은 캐릭터
DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(float, FOnChangeHealAmount, float, Original, ABaseCharacter*, HealedCharacter)
;

//플레이어가 몬스터를 잡았을 떄 호출됩니다.
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnKillMonster, ABaseCharacter*, Player, ABaseMonster*, KilledMonster);

//회피 최소 SP요구량 설정 이벤트.
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(float, FOnDodgeMinimumSP, const float, Original);


/*
 * 어빌리티로 부여된 각종 특성효과를 관리할 컴포넌트입니다.
 * 대부분은 이벤트 형식으로 저장되고 호출됩니다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULLIKE_API UAbilityTalentComponent : public UActorComponent
{
	GENERATED_BODY()
	friend class UAbilityDamageTalent;
	friend class UAbilityDefenceTalent;
	friend class UAbilityFreeTalent;

public:
	// Sets default values for this component's properties
	UAbilityTalentComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	float PhysicalDamageTraits(class ABaseCharacter* DamagedCharacter, ABaseCharacter* DamagedBy, const float& Damage);
	float MagicalDamageTraits(ABaseCharacter* DamagedCharacter, ABaseCharacter* DamagedBy, const float& Damage);

	//피해량 감소 특성 적용
	float PhysicalDecreaseDamageTraits(ABaseCharacter* DamagedCharacter, ABaseCharacter* DamagedBy,
	                                   const float& Damage);
	float MagicalDecreaseDamageTraits(ABaseCharacter* DamagedCharacter, ABaseCharacter* DamagedBy, const float& Damage);

public:
	/**
		 * 특성으로 증가할 피해량을 계산해 리턴합니다.
		 * @param DamagedBy
		 * @param DamagedCharacter
		 * @param AttackType 
		 * @param Damage 
		 * @return 
		 */
	UFUNCTION(BlueprintCallable)
	float CalculateModifiedDamageWithTraits(class ABaseCharacter* DamagedCharacter, ABaseCharacter* DamagedBy,
	                                        EAttackType AttackType, const float& Damage);

	/**
	 * 최종 피해량 특성으로 증가할 수치를 계산해 리턴합니다.
	 * @param DamagedBy
	 * @param DamagedCharacter
	 * @param AttackType 
	 * @param ResultDamage 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable)
	float CalculateModifiedResultDamageWithTraits(class ABaseCharacter* DamagedCharacter, ABaseCharacter* DamagedBy,
	                                              EAttackType AttackType, const float& ResultDamage);


	//방어 특성으로 감소할 수치를 계산해 리턴합니다.
	UFUNCTION(BlueprintCallable)
	float CommonDecreaseDamageTraits(ABaseCharacter* DamagedCharacter, ABaseCharacter* DamagedBy, float Damage);
	
	/**
	 * 방어력 특성으로 감소할 수치를 계산해 리턴합니다.
	 * @param DamagedCharacter 플레이어
	 * @param DamagedBy 누구에게 피해를 받았는지
	 * @param AttackType 공격타입
	 * @param Damage 기본 피해량
	 * @return 
	 */
	UFUNCTION(BlueprintCallable)
	float CalculateModifiedDecreaseDamageWithTraits(ABaseCharacter* DamagedCharacter, ABaseCharacter* DamagedBy,
	                                                EAttackType AttackType, float Damage);

	//피격 피해량 상승 적용
	UFUNCTION(BlueprintCallable)
	float CalculateModifiedIncreaseGotHitDamageWithTraits(ABaseCharacter* DamagedCharacter, ABaseCharacter* DamagedBy,
	                                                      EAttackType AttackType, float Damage);

	//이동 속도 적용
	UFUNCTION(BlueprintCallable)
	float CalculateModifiedRunSpeedWithTraits(float OriginalSpeed);

	UFUNCTION(BlueprintCallable)
	float CalculateModifiedWalkSpeedWithTraits(float OriginalSpeed);
	
	//회피 SP 적용
	UFUNCTION(BlueprintCallable)
	float CalculateModifiedDodgeSPWithTraits(float OriginalStamina);

	//회피거리 증가
	UFUNCTION(BlueprintCallable)
	float CalculateModifiedDodgeDistanceWithTraits(float OriginalForce);

	//SP 회복량 증가
	UFUNCTION(BlueprintCallable)
	float CalculateModifiedRecoverSpAmountWithTraits(float OriginalAmount);

	//무적시간증가
	UFUNCTION(BlueprintCallable)
	float CalculateModifiedIncreaseDodgeInvincibilityTimeWithTraits(ABaseCharacter* Target, float OriginalTime);

	//경험치 획득량 증가
	UFUNCTION(BlueprintCallable)
	float CalculateModifiedIncreaseGetExpWithTraits(float OriginalExp);

	//피해량의 일부를 MP로 대체
	UFUNCTION(BlueprintCallable)
	float CalculateModifiedDamageToMPWithTraits(ABaseCharacter* DamagedCharacter, ABaseCharacter* DamagedBy,
	                                            float OriginalDamage);

	//회피 성공시 다음 피해량 증가
	UFUNCTION(BlueprintCallable)
	float CalculateModifiedDodgeSuccWithTraits(APlayerCharacter* Player, float OriginalDamage);

	//방어력 증감 특성
	UFUNCTION(BlueprintCallable)
	float CalculateModifiedPhysicalDefenceWithTraits(ABaseCharacter* DamagedBy);
	UFUNCTION(BlueprintCallable)
	float CalculateModifiedMagicalDefenceWithTraits(ABaseCharacter* DamagedBy);
	UFUNCTION(BlueprintCallable)
	float CalculateModifiedDefenceByMonsterTypeWithTraits(float Defence, ABaseCharacter* DamagedBy);

	UFUNCTION(BlueprintCallable)
	void BroadcastOnSuccessDodge(ABaseCharacter* Character);

	UFUNCTION(BlueprintCallable)
	void BroadcastOnSuccessHit(ABaseCharacter* DamagedBy, ABaseCharacter* DamagedCharacter);

	UFUNCTION(BlueprintCallable)
	void BroadcastOnGotHit(ABaseCharacter* DamagedCharacter, ABaseCharacter* DamagedBy, float OriginalDamage);

	UFUNCTION(BlueprintCallable)
	float BroadcastChangeHealAmount(ABaseCharacter* HealedCharacter, float RecoverAmount);

	UFUNCTION(BlueprintCallable)
	void BroadcastOnKillMonster(ABaseCharacter* Killer, ABaseCharacter* Monster);

	UFUNCTION(BlueprintCallable)
	float BroadcastOnDodgeMinimumSP(float OriginalSP);

	//달리기 SP감소
	UFUNCTION(BlueprintCallable)
	float CalculateModifiedRunSPWithTraits(float OriginalStamina);

private:
	//피해량 증가 특성
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnIncreaseDamage> OnIncreasePhysicalDamage;
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnIncreaseDamage> OnIncreaseMagicalDamage;
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnIncreaseDamage> OnIncreaseResultDamage;


	//확률에 따라 피해 증가
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnIncreaseDamage> OnIncreaseDamageByChance;

	//입는 피해량 증가 특성
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnIncreaseGotHitDamage> OnIncreaseGotHitDamage;

	//달리는 속도 증가
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnIncreaseRunSpeed> OnIncreaseRunSpeed;
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnIncreaseMoveSpeed>  OnIncreaseMoveSpeed;
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnDecreaseMoveSpeed>  OnDecreaseMoveSpeed;
	
	//회피 SP감소
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnDecreaseAbilitySP> OnDecreaseDodgeSP;
	//회피거리 증가
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnIncreaseDodgeDistance> OnIncreaseDodgeDistance;

	//SP회복량 증가
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnIncreaseRecoverSPAmount> OnIncreaseRecoverSPAmount;

	//회피 무적시간 증가
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnIncreaseDodgeInvincibilityTime> OnIncreaseDodgeInvincibilityTime;

	//경험치 획득량 증가
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnIncreaseGetExp> OnIncreaseGetExp;


	//피해량의 일부를 MP로 대체
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnGotHitWithReturn> OnReplaceDamageToMP;

	//회피에 성공하면 실행되는 이벤트
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnSuccessDodge> OnSuccessDodge;
	//회피에 성공하면 다음 피해량 증가
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnIncreaseDamWhenSuccDodge> OnIncreaseDamWhenSuccDodge;

	//공격에 성공하면 실행되는 이벤트
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnSuccessDamageHit> OnSuccessDamageHit;
	//공격에 성공하면 물리피해량을 증가시키는 이벤트
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnIncreaseDamage> OnSuccessDamageHitIncreasePhysicalDam;
	//공격에 성공하면 마법피해량을 증가시키는 이벤트
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnIncreaseDamage> OnSuccessDamageHitIncreaseMagicalDam;

	//피격당하면 호출되는 이벤트
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnGotHit> OnGotHit;

	//받는 피해량이 감소하는 이벤트
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnDecreaseGotHitDamage> OnDecreaseGotHitDamage;
	//받는 물리 피해량이 감소하는 이벤트.
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnDecreaseGotHitDamage> OnDecreasePhysicalGotHitDamage;
	//받는 마법 피해량이 감소하는 이벤트.
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnDecreaseGotHitDamage> OnDecreaseMagicalGotHitDamage;

	//물리 방어력을 증감하는 이벤트.
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnChangeDefence> OnChangePhysicalDefence;
	//마법 방어력을 증감하는 이벤트
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnChangeDefence> OnChangeMagicalDefence;
	//몬스터 타입에따라 방어력 증감하는 이벤트
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnChangeDefence> OnChangeDefenceByMonsterType;


	//치유량을 증감하는 이벤트
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnChangeHealAmount> OnChangeHealAmount;

	//몬스터 처치시 호출되는 이벤트
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnKillMonster> OnKillMonster;


	//달리기 SP 감소
	UPROPERTY()
	TMap<class UAbilityTalent*, FOnDecreaseAbilitySP> OnDecreaseRunSP;

	//회피 최소 SP요구량 설정
	UPROPERTY()
	FOnDodgeMinimumSP OnDodgeMinimumSP;
};
