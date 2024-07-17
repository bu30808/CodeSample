// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "02_Ability/AbilityBase.h"
#include "02_Ability/AbilityCue.h"
#include "02_Ability/AbilityEffect.h"
#include "AbilityComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAbilityComponent, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddAbility, const FAbilityInformation&, AbilityInformation);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemoveAbility, const FGameplayTag&, AbilityTag);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnApplyEffect, const FGameplayTagContainer&, EffectTag);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndEffect, const FGameplayTagContainer&, EffectTag);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActivatedAbility, const FAbilityInformation&, AbilityInformation);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddCue, const FGameplayTag&, CueTag);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemoveCue, const FGameplayTag&, CueTag);

DECLARE_DYNAMIC_DELEGATE(FAdditionalOnEndAbility);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeAbilityQuickSlot, const FAbilityInformation&,AbilityInformation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFirstUpdateMainAbilityQuickSlot,const FAbilityInformation&,AbilityInformation);


UCLASS(Blueprintable)
class UAbilityAdditionalInformation : public UObject
{
	GENERATED_BODY()

public:
	FAdditionalOnEndAbility OnEndAbility;
};

UCLASS(Blueprintable)
class UAbilityCueAdditionalInformation : public UObject
{
	GENERATED_BODY()

public:
	FVector HitLocation;
	FVector ImpactNormal;
	void UpdateCueLocation(TArray<FAbilityCueInformation>& CueInfos) const;
};

/*
 * 어빌리티, 어빌리티 이팩트, 어빌리티 큐를 관리하는 컴포넌트입니다.
 * 대부분 게임플레이 태그를 이용해 관리하게 됩니다.
 * 
 * GiveAbility 함수로 어빌리티를 컴포넌트에 등록하고,
 * ActivateAbility 함수로 어빌리티를 사용합니다.
 *
 */

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULLIKE_API UAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAbilityComponent();


	/*이 아래 이벤트들은 주로 디버깅을 위해 사용됩니다.*/

	//어빌리티가 추가될 떄 호출되는 이벤트입니다.
	UPROPERTY(BlueprintAssignable)
	FOnAddAbility OnAddAbility;
	//어빌리티가 제거될 떄 호출되는 이벤트입니다.
	UPROPERTY(BlueprintAssignable)
	FOnRemoveAbility OnRemoveAbility;

	//어빌리티 사용됐을 때,
	UPROPERTY(BlueprintAssignable)
	FOnActivatedAbility OnActivatedAbility;
	//어빌리티 사용 끝났을 때
	UPROPERTY(BlueprintAssignable)
	FOnEndAbility OnEndAbility;

	//이펙트가 적용될 때 
	UPROPERTY(BlueprintAssignable)
	FOnApplyEffect OnApplyEffect;
	//이펙트가 제거될 때
	UPROPERTY(BlueprintAssignable)
	FOnEndEffect OnEndEffect;

	//큐 추가시
	UPROPERTY(BlueprintAssignable)
	FOnAddCue OnAddCue;
	//큐 제거시
	UPROPERTY(BlueprintAssignable)
	FOnRemoveCue OnRemoveCue;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//이 목록에 있는 어빌리티만 사용 가능합니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient)
	TArray<class UAbilityBase*> AvailableAbilities;

	//현재 사용(발동)중인 어빌리티의 태그입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient)
	FGameplayTagContainer ActivatedAbilityTags;

	/**
	 * @brief 이 컴포넌트에 적용중인 이팩트 태그를 저장하는 변수입니다.
	 *
	 * 예시 ) 쿨다운 적용시 쿨다운 태그를 이 변수에 저장합니다.
	 * 이 변수 내부에 쿨다운 태그가 존재하는 경우, 쿨타임이 적용중인 상태가 됩니다.
	 * 쿨타임이 끝났을 때, 쿨다운 태그를 이 변수에서 제거합니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient)
	FGameplayTagContainer AppliedEffectTags;

	//적용된 이팩트를 저장하는 입니다.
	UPROPERTY(VisibleAnywhere, Transient)
	TArray<class UAbilityEffect*> AppliedEffects;

	//재 사용 가능한 어빌리티 큐를 저장하는 맵입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient)
	TMap<FGameplayTag, class AAbilityCue*> ReusableCues;

	//지속시간형 어빌리티 큐를 저장하는 맵입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient)
	TMap<FGameplayTag, class AAbilityCue*> DurableCues;

public:
	const TArray<class UAbilityBase*>& GetAvailableAbilities() const { return AvailableAbilities; }
	const FGameplayTagContainer& GetActivatedAbilityTags() { return ActivatedAbilityTags; }
	/**
	 * @brief 등록된 어빌리티중에 전달된 태그에 해당하는 어빌리티를 찾습니다.
	 * @param AbilityTag 찾고자 하는 어빌리티 태그
	 * @return 찾지 못한 경우 nullptr
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAbilityBase* GetAbilityByTag(FGameplayTag AbilityTag);

	/**
	 * @brief 등록된 어빌리티중 이 태그를 가진 어빌리티가 있는지 확인합니다.
	 * @param AbilityTag 확인할 어빌리티 태그
	 * @return 있으면 참. 없으면 거짓
	 */
	UFUNCTION(BlueprintPure, BlueprintCallable)
	bool HasAbility(FGameplayTag AbilityTag);


	/**
	 * @brief 어빌리티를 컴포넌트에 등록합니다.
	 * @param Ability 등록할 어빌리티 정보
	 * @return 등록에 실패하면 거짓.
	 */
	UFUNCTION(BlueprintCallable)
	bool GiveAbility(TSubclassOf<class UAbilityBase> Ability);

	/**
	 * @brief 해당 태그를 가진 어빌리티를 찾아 목록에서 제거합니다.
	 * 다시 사용하기 위해서는 다시 등록해야 합니다.
	 * @param AbilityTag 
	 */
	UFUNCTION(BlueprintCallable)
	void RemoveAbility(FGameplayTag AbilityTag);

	/**
	 * @brief 태그에 해당하는 어빌리티가 이미 사용중인지 확인합니다.
	 * @param AbilityTag 
	 * @return 이미 사용중인 상태라면 참.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsAlreadyActivated(FGameplayTag AbilityTag);

	/**
	 * @brief 어빌리티를 사용 시도 합니다.
	 * @param AbilityTag 사용할 어빌리티 태그
	 * @param AbilityTarget 어빌리티가 적용될 대상
	 * @param InputAction 어떤 액션 키로 사용되는가
	 * @param AdditionalInfo 추가로 전달할 정보가 있으면 넘겨주세요.
	 */
	UFUNCTION(BlueprintCallable)
	bool ActivateAbility(FGameplayTag AbilityTag, class AActor* AbilityTarget,
	                     /*class UInputAction* InputAction = nullptr,*/ UObject* AdditionalInfo = nullptr);


	/**
	 * @brief 사용한 어빌리티를 ActivatedAbilityTags에 등록합니다.
	 * @param ActivateAbilityTag 
	 */
	UFUNCTION(BlueprintCallable)
	void RegisterAbilityTag(FGameplayTag ActivateAbilityTag);

	/**
	 * @brief 사용이 끝난 어빌리티를 ActivatedAbilityTags에서 제거합니다.
	 * @param ActivateAbilityTag 
	 */
	UFUNCTION(BlueprintCallable)
	void UnRegisterAbilityTag(FGameplayTag ActivateAbilityTag);

	/**
 * @brief 특정 태그가 ActivatedAbilityTags에 하나라도 있는가 확인합니다.
 * @param Tags 
 * @return 있으면 참.
 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasAnyActivateAbilityTags(const FGameplayTagContainer& Tags);

	/**
	 * @brief 모든 태그가 ActivatedAbilityTags에 있는가 확인합니다.
	 * @param Tags 
	 * @return 있으면 참.
	 */
	bool HasAllActivateAbilityTags(const FGameplayTagContainer& Tags);

	/*********************** 어빌리티 이팩트 관련 *****************************/

	//AppliedEffectTags변수 안에 해당 태그가 있는지 확인합니다. 있으면 참.
	UFUNCTION(BlueprintPure, BlueprintCallable)
	bool HasEffectTag(FGameplayTag GameplayTag);

	/**
	 * @brief AppliedEffectTags변수 내부에 전달받은 태그가 전부 있는가 확인합니다.
	 * @param NeedEffectTags 
	 * @return 
	 */
	UFUNCTION(BlueprintPure, BlueprintCallable)
	bool HasAllEffectTag(const FGameplayTagContainer& Tags);

	/**
	 * @brief 특정 태그가 AppliedEffectTags변수에 하나라도 있는가 확인합니다.
	 * @param Tags 
	 * @return 있으면 참.
	 */
	UFUNCTION(BlueprintPure, BlueprintCallable)
	bool HasAnyEffectTag(const FGameplayTagContainer& Tags);


	UFUNCTION(BlueprintCallable)
	void RegisterEffectTag(const FGameplayTag& EffectTag);

	UFUNCTION(BlueprintCallable)
	void RegisterEffectTags(const FGameplayTagContainer& EffectTags);


	UFUNCTION(BlueprintCallable)
	void UnRegisterEffectTag(const FGameplayTag& EffectTag);

	UFUNCTION(BlueprintCallable)
	void UnRegisterEffectTags(const FGameplayTagContainer& EffectTags);


	/**
	 * 어빌리티 이팩트를 적용합니다.
	 * @param Effect 적용할 이팩트
	 * @param CallAbility 이 함수를 호출한 어빌리티. 비워도 됩니다.
	 * 비워둘 경우 어빌리티 내부 EffectExpired가 바인드되지 않습니다.
	 */
	UFUNCTION(BlueprintCallable)
	void K2_ApplyEffect(TSubclassOf<UAbilityEffect> Effect, AActor* EffectBy, FOnEffectExpired OnEffectExpired,
	                    UObject* AdditionalData = nullptr);
	/**
	 * 이 함수는 내부에서 전달받은 이팩트 오브젝트를 복제하지 않습니다. COD를 직접 전달하지 마십시오.
	 */
	UFUNCTION(BlueprintCallable)
	void ApplyEffect(class UAbilityEffect* Effect, AActor* EffectBy, FOnEffectExpired OnEffectExpired,
	                 UAbilityBase* From, UObject*
		                 AdditionalData = nullptr);
	UFUNCTION(BlueprintCallable)
	void ApplyEffects(const TArray<UAbilityEffect*>& Effects, AActor* EffectBy, FOnEffectExpired OnEffectExpired,
	                  UAbilityBase* From, UObject
	                  * AdditionalData);
	//내부적으로 전달받은 이팩트를 복제해서 사용합니다.
	UFUNCTION(BlueprintCallable)
	TArray<class UAbilityEffect*> K2_ApplyEffectsWithReturn(const TArray<TSubclassOf<class UAbilityEffect>>& Effects,
	                                                        class AActor* EffectBy = nullptr,
	                                                        UObject* AdditionalData = nullptr);
	//이미 외부에서 복사된 이팩트 배열을 받아 사용합니다. 절대 COD를 직접 전달하지 마십시오.
	TArray<class UAbilityEffect*> ApplyEffectsWithReturn(const TArray<class UAbilityEffect*>& Effects,
	                                                     class AActor* EffectBy = nullptr,
	                                                     UObject* AdditionalData = nullptr);

	//어빌리티 이팩트를 종료합니다.
	UFUNCTION(BlueprintCallable)
	void EndEffect(class UAbilityEffect* Effect);
	UFUNCTION(BlueprintCallable)
	void EndEffects(const TArray<UAbilityEffect*>& Effects);
	UFUNCTION(BlueprintCallable)
	void EndEffectByTag(FGameplayTag Tag);
	UFUNCTION(BlueprintCallable)
	void RemoveSavedCues(const TArray<FAbilityCueInformation>& Cues);
	//저장되어 있는 큐 중에 해당 태그와 같은 큐가 있다면 제거합니다.
	UFUNCTION(BlueprintCallable)
	void RemoveSavedCueByTag(const FGameplayTagContainer& CueTags);
	//무적 태그가 있는지 확인합니다.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsInvincible();


	UPROPERTY()
	FTimerHandle CueClearTimerHandle;
	UFUNCTION(BlueprintCallable)
	AAbilityCue* ApplyCue(FAbilityCueInformation CueInformation);
	UFUNCTION(BlueprintCallable)
	const TArray<AAbilityCue*> ApplyCues(const TArray<FAbilityCueInformation>& CueInformations);

	//특정 어빌리티를 강제종료합니다.
	UFUNCTION(BlueprintCallable)
	void ForceEndAbility(const FGameplayTag& AbilityTag);


	UFUNCTION()
	void OnEndAbilityEvent();

	UFUNCTION(BlueprintCallable)
	void ClearReusableCue();
	void DestroyReusableCues();


	//어빌리티에 추가적으로 전달된 정보를 처리하는 함수입니다.
	void HandleAbilityAdditionalData(UAbilityBase* Ability, UObject* Object);

	UFUNCTION()
	void RemoveFromAppliedEffectsArray(class UAbilityEffect* EffectToRemove);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	class UAbilityEffect* GetAppliedEffectByTag(FGameplayTag TagToFind);

	//적용되는 특성들은 GC방지처리 되어있기 때문에, 게임플레이 종료시 GC방지설정을 해제합니다.
	void UnRootAppliedTalents();
	//적용되는 이팩트는 GC방지처리 되어있기 때문에, 게임플레이 종료시 GC방지설정을 해제합니다.
	void UnRootAppliedEffects();

	virtual void DestroyComponent(bool bPromoteChildren) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	//지금 표시중인 인덱스
	UPROPERTY(VisibleAnywhere, meta=(ClampMin = -1, ClampMax = 9))
	int32 CurAbilityQuickSlotIndex = -1;
	
	UPROPERTY(VisibleAnywhere)
	TArray<FGameplayTag> AbilityQuickSlotTags = {
		FGameplayTag::EmptyTag, FGameplayTag::EmptyTag, FGameplayTag::EmptyTag, FGameplayTag::EmptyTag,
		FGameplayTag::EmptyTag, FGameplayTag::EmptyTag, FGameplayTag::EmptyTag, FGameplayTag::EmptyTag,
		FGameplayTag::EmptyTag, FGameplayTag::EmptyTag
	};

public:
	UPROPERTY()
	FOnChangeAbilityQuickSlot OnChangeAbilityQuickSlot;
	UPROPERTY()
	FOnFirstUpdateMainAbilityQuickSlot OnFirstUpdateMainAbilityQuickSlot;
	
	const TArray<FGameplayTag>& GetAbilityQuickSlot(){return AbilityQuickSlotTags;}
	bool IsRegistered(const FGameplayTag& AbilityTag);
	//어빌리티 퀵슬롯에서 비어있지 않은 가장 첫 슬롯을 찾아 할당합니다.
	void AddQuickSlotAbility(const FGameplayTag& AbilityTag);
	void OverrideAbilityQuickSlotArray(const TArray<FGameplayTag>& AbilitySlotArray){AbilityQuickSlotTags = AbilitySlotArray;}
	void InitAbilityQuickSlotIndex();

	void UseAbilityQuickSlot();
	void NextAbilityQuickSlot();
	void ClearMainAbiltiyQuickSlot();
};
