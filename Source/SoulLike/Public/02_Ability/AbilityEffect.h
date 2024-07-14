// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityCue.h"
#include "GameplayTagContainer.h"
#include "Engine/HitResult.h"
#include "UObject/NoExportTypes.h"
#include "AbilityEffect.generated.h"

class ABaseCharacter;
DECLARE_LOG_CATEGORY_EXTERN(LogEffect, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogTalent, Log, All);

DECLARE_DYNAMIC_DELEGATE(FOnEffectExpired);

//이 이팩트와 같은 태그를 가진 이팩트가 이미 적용중일 때 어떻게 할 것인가 결정합니다.
UENUM(BlueprintType)
enum class EEffectApplicationDecision : uint8
{
	// 이펙트 적용 건너뛰기
	Skip,
	// 새 이펙트를 건너뛰고, 기존 이팩트 오브젝트로 다시 적용합니다.
	SkipButApply,
	// 이팩트 존재 유무와 관계없이 항상 적용합니다.
	AlwaysApply,
	MAX
};

UENUM(BlueprintType)
enum class EApplyFailedAction : uint8
{
	NoAction,
	//아무것도안함
	EndEffect,
	//이팩트를 종료함
	EndAbility //이 이팩트를 적용한 어빌리티를 종료함.
};

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	None UMETA(DisplayName = "NotAttack"),
	Physical UMETA(DisplayName = "PhysicalAttack"),
	Magical UMETA(DisplayName = "MagicalAttack")
};

//속성 종류를 정의합니다.
UENUM(BlueprintType)
enum class EAttributeType : uint8
{
	NONE,

	HP,
	MaxHP,
	SP,
	MaxSP,
	MP,
	MaxMP,

	RecoverHP,
	RecoverSP,
	RecoverMP,

	PhysicalAttack,
	MagicalAttack,

	PhysicalDefense,
	MagicalDefense,

	ActionSpeed,
	Endurance,

	Strength,
	Dexterity,
	Intelligence,
	Willpower,


	Level,
	EXP,

	//저항력
	PoisonResist,
	DeadlyPoisonResist,
	BurnResist,
	ChillResist,
	BleedResist,
	PetrifactionResist,

	//누적수치
	PoisonAcc,
	DeadlyPoisonAcc,
	BurnAcc,
	ChillAcc,
	BleedAcc,
	PetrifactionAcc,

	//이동속도
	MoveSpeed,

	MAX
};

//속성에 어떤식으로 영향을 줄 것인가 정의합니다.
UENUM(BlueprintType)
enum class EAttributeApplyMethod : uint8
{
	NONE,

	ADD,
	REMOVE,
	MULT,
	DIV,
	SET
};

//영향을 줄 속성을 정의하는 구조체입니다.
USTRUCT(BlueprintType)
struct FAttributeEffect
{
	GENERATED_BODY()

public:
	//어떤 속성을 건드릴지 정의합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttributeType Attribute = EAttributeType::NONE;

	//어떤 효과를 적용할지 정의합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttributeApplyMethod ApplyMethod = EAttributeApplyMethod::NONE;

	//적용될 값입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ApplyValue = 0;

public:
	FAttributeEffect()
	{
	}

	FAttributeEffect(EAttributeType Type, EAttributeApplyMethod Method, float Value): Attribute(Type),
		ApplyMethod(Method), ApplyValue(Value)
	{
	}

	//적용 방법과 적용하려는 어트리뷰트가 같으면, 같은 구조체로 취급합니다.
	bool operator==(const FAttributeEffect& Other) const
	{
		return Other.ApplyMethod == ApplyMethod && Other.Attribute == Attribute;
	}
};


//이팩트가 적용될 방식을 정의합니다.
UENUM(BlueprintType)
enum class EEffectApplyType : uint8
{
	//즉발형
	Instant,
	//지속형
	Duration,
	//n초마다 효과 적용
	DurationWithInterval,
	//n초동안 틱마다 적용
	DurationWithTick,
	//무제한
	Infinite,
	//시간제한이 없이 n초마다 효과 적용
	InfiniteWithInterval,
	//틱마다 적용
	InfiniteWithTick,
	//스택형
	Stack
};

//이팩트에 추가정보가 필요하다면 사용하세요.
UCLASS(Blueprintable)
class UAbilityEffectAdditionalInformation : public UObject
{
	GENERATED_BODY()

public:
	//이 값이 참이면 피격 애니메이션을 사용합니다.
	UPROPERTY(BlueprintReadWrite)
	bool bTriggerHitAnimation = false;
	//누구한테 맞았는지 저장하는 변수입니다.
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> HitBy;
	//타격에 대한 정보를 저장하고 있는 구조체
	UPROPERTY(BlueprintReadWrite)
	FHitResult Hit;
};

USTRUCT(BlueprintType)
struct FAttributeRestoreData
{
	GENERATED_BODY()

public:
	//처음 적용된 정보를 저장합니다. 이 내부 ApplyValue값은 신경쓰지 마십시오.
	UPROPERTY()
	FAttributeEffect Effect;
	UPROPERTY()
	float Value = 0;

	FAttributeRestoreData()
	{
	}

	FAttributeRestoreData(FAttributeEffect E, float AppliedValue): Effect(E), Value(AppliedValue)
	{
	}
};

/**
 * 
 */
UCLASS(Blueprintable, DefaultToInstanced)
class SOULLIKE_API UAbilityEffect : public UObject
{
	GENERATED_BODY()

	friend class UAbilityComponent;
	friend class AEquipmentItemActor;
	friend class ASoulItemActor;
#if WITH_EDITOR
	friend class UAbilityCreatorWidget_Req;
	friend class UAbilityEffectCreatorWidget;
	friend class UAbilityEffectHandlerWidget;
#endif

protected:
	//이 이팩트가 적용될 방식을 설정합니다.
	UPROPERTY(EditAnywhere)
	EEffectApplyType EffectApplyType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta=(EditCondition =
			"EffectApplyType == EEffectApplyType::Duration || EffectApplyType == EEffectApplyType::DurationWithInterval|| EffectApplyType == EEffectApplyType::DurationWithTick"
		))
	float Time;

	//몇초마다 영향을 줄 것인가 설정합니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly,
		meta=(EditCondition =
			"EffectApplyType == EEffectApplyType::DurationWithInterval || EffectApplyType == EEffectApplyType::InfiniteWithInterval"
		))
	float IntervalTime = 0;

	//틱 주기를 설정합니다. 기본값은 -1입니다.
	//-1은 주기를 조정하지 않고 매 프레임 호출하겠다는 뜻입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta=(EditCondition =
			"EffectApplyType == EEffectApplyType::DurationWithTick || EffectApplyType == EEffectApplyType::InfiniteWithTick"
		))
	float TickRate = -1;

	//이팩트 적용 조건에 맞지 않을 떄 할 행동을 정의합니다.
	UPROPERTY(EditAnywhere)
	EApplyFailedAction ApplyFailedAction;

	//이 이팩트가 타겟에게 영향을 주어야 할 스테이터스에 대한 정보가 담겨 있습니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAttributeEffect> AttributeEffects;


	//이팩트의 오너의 스텟정보에 영향을 받아 적용될 스테이터스에 대한 정보가 담겨 있습니다.
	//절대값을 구조체 내부 Value값으로 설정해주면, 그 값에 스텟정보에 의해 상승되어야 할 값이 더해져 최종값이 결정됩니다.
	//최종값을 결정하는 과정을 UpdateAttributeEffectsAffectedByOwnersAttribute 함수에서 계산합니다.
	//필요하다면 UpdateAttributeEffectsAffectedByOwnersAttribute함수를 블루프린트에서 덮어 쓸 수 있습니다.
	//ex) sp회복량은 의지와 민첩의 영향을 받습니다.
	//절대값 0.1 + 스텟으로 추가 상승되는값 2 => 최종 회복량 2.1
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FAttributeEffect> AttributeEffectsAffectedByOwnersAttribute;

	//위 배열이 업데이트 되고 난 이후 갱신된 값이 저장되는 배열입니다.
	UPROPERTY(Transient)
	TArray<FAttributeEffect> UpdatedAttributeEffectsAffectedByOwnersAttribute;

	//이 값이 참이라면, 엔드이팩트때 변경한 속성값을 되돌립니다
	//ex) 힘을 10추가했을 경우, 이 변수가 참이면 끝날 때, 힘 10을 제거합니다.
	UPROPERTY(EditAnywhere)
	bool bRestoreAttributeOnEnd = false;

	//AttributeEffects로 변경된 수치가 저장될 맵입니다.
	UPROPERTY(Transient)
	TMap<EAttributeType, FAttributeRestoreData> AffectedValueFromAttributeEffects;
	//AttributeEffectsAffectedByOwnersAttribute로 변경될 수치가 저장될 맵입니다.
	UPROPERTY(Transient)
	TMap<EAttributeType, FAttributeRestoreData> AffectedValueAttributeEffectsAffectedByOwnersAttribute;


	//이 이팩트 오브젝트를 구분짓는 유일한 태그입니다.
	//이 값으로 이팩트가 생성한 어빌리티 큐를 찾아올 수 있습니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag UniqueEffectTag;

	//이 이팩트가 적용되는 대상에게 등록할 추가 이팩트 태그입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer AdditionalEffectTags;

	//이 태그가 하나도 없어야 효과가 적용됩니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer BannedEffectTags;

	//이 태그가 모두 있어야 효과가 적용됩니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer NeedEffectTags;

	//이 태그가 하나도 없어야 효과가 적용됩니다.
	//즉, 발동중인 어빌리티중에 이 태그를 가진 어빌리티가 없어야 합니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer BannedActivateAbilityTags;

	//이 태그가 모두 있어야 효과가 적용됩니다.
	//즉, 발동중인 어빌리티중에 이 태그를 가진 어빌리티가 전부 있어야 합니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer NeedActivateAbilityTags;


	//이팩트 타겟에게 단발성으로 적용할 큐 정보입니다.
	UPROPERTY(EditAnywhere, Category="Cue",BlueprintReadWrite)
	TArray<FAbilityCueInformation> InstanceAbilityCues;
	//일정 주기로 큐를 적용할 경우에 사용하세요
	UPROPERTY(EditAnywhere, Category="Cue",BlueprintReadWrite)
	TArray<FAbilityCueInformation> IntervalAbilityCues;
	//영구 혹은 지속시간동안 큐를 적용해야 할 경우 사용하세요.
	UPROPERTY(EditAnywhere, Category="Cue",BlueprintReadWrite)
	TArray<FAbilityCueInformation> InfiniteAbilityCues;
	//이팩트가 종료될 때 적용할 큐가 있다면 사용하세요
	UPROPERTY(EditAnywhere, Category="Cue",BlueprintReadWrite)
	TArray<FAbilityCueInformation> InstanceEndAbilityCues;

	//틱마다 사용되는 태스크 
	UPROPERTY(Transient)
	TWeakObjectPtr<class UGameplayTask_LaunchEvent> TickTask;

	//이 값이 참이라면, 어빌리티와 관계없이 배율 시스템을 적용합니다.
	//다만 이렇게 적용할 경우, 하나의 정해진 값만 적용 가능합니다.
	UPROPERTY(EditAnywhere)
	bool bUseSelfChainSystem = false;
	UPROPERTY(EditAnywhere,meta=(EditCondition="bUseSelfChainSystem"))
	FGameplayTag SelfChainTag;
	UPROPERTY(EditAnywhere,meta=(EditCondition="bUseSelfChainSystem"))
	float SelfChainValue = 0;
	//공격배율을 사용합니까?
	//공격배율은 어빌리티 내부에서 설정됩니다.
	//이팩트가 적용될 때, 어빌리티로부터 가져옵니다.
	//몽타주 내부에서 이팩트가 발생되는 경우와 같은 특수한 경우, 이 값을 참으로 준 뒤 ChainTagFromAbility변수를 어빌리티 내부 배율 태그와 같은 값을 주세요.
	UPROPERTY(EditAnywhere,meta=(EditCondition="!bUseSelfChainSystem"))
	bool bUseChainSystemFromAbility = false;
	//공격 배율을 사용하기 위한 태그입니다. 없으면 배율이 적용되지 않습니다
	//이팩트가 적용될 때, 어빌리티로부터 가져옵니다.
	//몽타주 내부에서 이팩트가 발생되는 경우와 같은 특수한 경우, 어빌리티 내부 배율 태그와 같은 값을 주세요.s
	UPROPERTY(EditAnywhere,meta=(EditCondition="bUseChainSystemFromAbility"))
	FGameplayTag ChainTagFromAbility;

	

	//이 이팩트가 타격으로 피해를 주기 위한 이팩트일경우 설정하세요
	//이 변수가 참인 경우에는, 무적 태그가 적용되어있는동안 이 이팩트가 적용되지 않는다는 뜻이기도 합니다.
	UPROPERTY(EditAnywhere)
	bool bIsHitDamageEffect;
	UPROPERTY(EditAnywhere, meta=(EditCondition ="bIsHitDamageEffect"))
	EAttackType AttackType;
	//이 이팩트로 주는 피해가 피해 애니메이션을 발동할 수 있는가?
	UPROPERTY(EditAnywhere)
	bool bTriggerHitAnimation = false;

	//이 이팩트와 같은 태그를 가진 이팩트가 이미 적용중일 때 어떻게 할 것인가 결정합니다.
	UPROPERTY(EditAnywhere)
	EEffectApplicationDecision EffectApplicationDecision = EEffectApplicationDecision::AlwaysApply;

	//이팩트 내부에서 셀프로 종료되었을 때, 호출되는 델리게이트입니다.
	//보통 어빌리티 내부에서 바인드 됩니다.
	UPROPERTY(Transient)
	FOnEffectExpired OnEffectExpired;

	//이 이팩트로 생성된 큐를 저장하는 변수
	//재 사용 가능한 큐도 저장됩니다.
	//주로 일회성 큐를 즉시 제거하기 위해 사용합니다.
	//필요하다면 RemoveAppliedCue 함수를 호출해서 수동으로 제거하세요.
	UPROPERTY(BlueprintReadWrite,Transient)
	TArray<class AAbilityCue*> AppliedCues;

	//틱 이팩트인 경우, 델타타임이 외부에서 필요한 경우 사용하세요.
	UPROPERTY(BlueprintReadOnly,Transient)
	float TickEffectDeltaTime;

	UPROPERTY(Transient)
	TObjectPtr<class UAbilityBase> CauseFromThisAbility;

public:
	//틱 이펙트인 경우, TickEffectDeltaTime에 저장된 값을 가져옵니다.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetDeltaTime() { return TickEffectDeltaTime; }

	const FGameplayTag& GetEffectTag() { return UniqueEffectTag; }
	//지속시간을 리턴합니다.
	const float& GetDurationTime() { return Time; }

	//이 이팩트가 적용하려고 하는 속성정보 리스트를 가져옵니다.
	const TArray<FAttributeEffect>& GetAttributeEffects() { return AttributeEffects; }
	void SetExpiredDelegate(const FOnEffectExpired& OnEffectExpiredDel);

protected:
	//이팩트 태그를 등록합니다.
	UFUNCTION(BlueprintCallable)
	void RegisterEffectTag(ABaseCharacter* Target);
	//이팩트 태그를 등록 해제합니다.
	UFUNCTION(BlueprintCallable)
	void UnRegisterEffectTag(ABaseCharacter* Target);
	
	void ChainSetting(ABaseCharacter* Target, UAbilityBase* From);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ProcessEffect(ABaseCharacter* Target, AActor* EffectBy, UAbilityBase* From, UObject* AdditionalData = nullptr);
	virtual void ProcessEffect_Implementation(ABaseCharacter* Target, AActor* EffectBy, UAbilityBase* From,
	                                          UObject* AdditionalData = nullptr);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EndEffect(ABaseCharacter* Target);
	virtual void EndEffect_Implementation(ABaseCharacter* Target);

	//이 이팩트를 어빌리티 컴포넌트에서 제거합니다.
	UFUNCTION(BlueprintCallable)
	void RemoveEffectFromAbilityComponent(ABaseCharacter* Target);

	/**
	 * 어트리뷰트값이 적용된 수치를 계산된 이후 호출됩니다.
	 * 위 결과를 가지고 특성을 적용한 값을 계산합니다.
	 * @param DamagedCharacter 피해를 입은 대상
	 * @param DamagedBy  피해를 준 대상
	 * @param AttributeEffect
	 */
	virtual void ApplyAbilityDamageTalent(ABaseCharacter* DamagedCharacter, ABaseCharacter* DamagedBy, FAttributeEffect& AttributeEffect);


	/**
	 * 피해감소 특성을 적용합니다.( 방어력 증감 특성과 다릅니다 )
	 * @param DamagedCharacter 피해를 입은 대상
	 * @param DamagedBy 피해를 준 대상
	 * @param AttributeEffect
	 * @param EffectsByTalent
	 */
	virtual void ApplyAbilityDecreaseDamageTalent(ABaseCharacter* DamagedCharacter, ABaseCharacter* DamagedBy, FAttributeEffect& AttributeEffect, TArray<FAttributeEffect
	                                              >& EffectsByTalent);


	/**
	 * 캐릭터가 피해를 받는 경우 발동하는 특성을 처리합니다.
	 * @param DamagedCharacter 피해를 받은 플레이어
	 * @param OriginalDamage 피해량
	 * @param AdditionalInfo 누가 피해를 줬는지에 대한 정보가 들어있는 오브젝트
	 */
	void ApplyGotHitTalent(ABaseCharacter* DamagedCharacter, float OriginalDamage, UObject* AdditionalInfo);


	/**
	 * HP 회복량 증감 특성을 적용합니다.
	 * @param Target 
	 * @param AttributeEffect 
	 * @return 
	 */
	float ApplyChangeHealAmountTalent(ABaseCharacter* Target, FAttributeEffect AttributeEffect) const;
	//얼마나 지속되었는가 확인할 타이머 핸들
	FTimerHandle DurationTimerHandle;
	//일정 주기마다 반복할 타이머 핸들
	FTimerHandle IntervalTimerHandle;
	//주기마다 큐 생성을 반복할 타이머 핸들
	FTimerHandle IntervalCueTimerHandle;

protected:
	//void ApplyAbilityDecreaseDamageTalent(ABaseCharacter* Target);
	//영향을 받은 어트리뷰트를 되돌립니다.
	UFUNCTION(BlueprintNativeEvent)
	void RestoreAttribute(ABaseCharacter* Target);
	virtual void RestoreAttribute_Implementation(ABaseCharacter* Target);

	//캐릭터 정보를 표시하는 위젯의 내용을 업데이트 합니다.
	void UpdateCharacterInfoWidget(ABaseCharacter* Target);
	//즉발성으로 효과를 줍니다.
	//추가로 전달할 정보가 있다면 두번째 파라미터에 담아 전달하세요.
	//성공적으로 적용되었으면, 참을 리턴합니다.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool ApplyInstantEffect(ABaseCharacter* Target, UObject* AdditionalInfo = nullptr, float DeltaTime = 1);
	virtual bool ApplyInstantEffect_Implementation(ABaseCharacter* Target, UObject* AdditionalInfo = nullptr,
	                                               float DeltaTime = 1);
	
	void ProcessDamageAttributeEffects(const TArray<FAttributeEffect>& AttributeEffectsArray, class ABaseCharacter* Target, class UObject* AdditionalInfo, float
	                                   DeltaTime);
	void ProcessAttributeEffects(const TArray<FAttributeEffect>& AttributeEffectsArray, class ABaseCharacter* Target, class UObject* AdditionalInfo, float
	                             DeltaTime);

	
	//ApplyInstantEffect가 적용되고, 일정시간 후 종료됩니다.
	void ApplyDurationEffect(ABaseCharacter* Target);
	//일정 주기로 ApplyInstantEffect가 호출됩니다.
	void ApplyIntervalEffect(ABaseCharacter* Target);
	//ApplyDurationEffect와 같으나, 종료되지 않습니다.
	void ApplyInfiniteEffect(ABaseCharacter* Target);
	//틱마다 ApplyInstantEffect가 적용됩니다.
	void ApplyTickEffect(ABaseCharacter* Target);

	//누적된 스택 수치를 저장할 스텍입니다
	//내부 값과 관계 없이 그냥 갯수만 확인하면 됩니다.
	UPROPERTY(VisibleAnywhere,Transient)
	TArray<bool> Stack;
	//스택을 쌓습니다.
	void AddStack(UAbilityBase* From);

	//배율을 꺼내와 적용합니다. 플레이어만 해당합니다.
	float ApplyAttackChain(const ABaseCharacter* Target, float Original, ABaseCharacter* Causer) const;
	//이 이팩트가 피해 이팩트인 경우, 피해량에 방어력을 적용합니다.
	float ApplyDefence(ABaseCharacter* DamagedCharacter, UObject* AdditionalInfo, const float& Damage);

	//대상에게 이팩트를 적용할수 있는지 확인합니다.
	//적용 가능하다면 참입니다.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanApplyEffect(ABaseCharacter* Target, bool bShowLog = true) const;
	virtual bool CanApplyEffect_Implementation(ABaseCharacter* Target, bool bShowLog = true) const;

private:
	//이팩트 태그 유무에 대해서 검사합니다.
	bool HasAnyBannedEffectTags(class UAbilityComponent* AbComp) const;
	bool HasAllNeedEffectTags(class UAbilityComponent* AbComp) const;


	//어빌리티 태그 유무에 대해서 검사합니다.
	bool HasAnyBannedAbilityTags(class UAbilityComponent* AbComp) const;
	bool HasAllNeedAbilityTags(class UAbilityComponent* AbComp) const;


	//속성 메니저를 가져옵니다.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	class UAttributeProcessSubsystem* GetAttributeProcessSubsystem(ABaseCharacter* Target);

protected:
	//오너의 스텟에 영향을 받은 어트리뷰트 이팩트 값을 갱신합니다.
	//AttributeEffectsAffectedByOwnersAttribute변수가 비어있으면 아무것도 안 합니다.
	UFUNCTION(BlueprintNativeEvent)
	bool UpdateAttributeEffectsAffectedByOwnersAttribute(ABaseCharacter* Target);
	virtual bool UpdateAttributeEffectsAffectedByOwnersAttribute_Implementation(
		ABaseCharacter* Target);


	//AttributeEffects에 담긴 내용을 재 정의 해야할 필요가 있을 경우 덮어쓰세요.
	UFUNCTION(BlueprintNativeEvent)
	void OverrideAttributeEffects(ABaseCharacter* Target, AActor* EffectBy);
	virtual void OverrideAttributeEffects_Implementation(ABaseCharacter* Target, AActor* EffectBy);

	//이 이팩트의 Time값을 수정할 일이 있다면 사용하세요.
	UFUNCTION(BlueprintNativeEvent)
	void OverrideTime(ABaseCharacter* Target);

	virtual void OverrideTime_Implementation(ABaseCharacter* Target)
	{
	}

	//틱을 사용할때만 사용할 변수입니다.
	UPROPERTY(BlueprintReadOnly,Transient)
	TWeakObjectPtr<ABaseCharacter> TickOwner;
	//틱 태스크에서 틱마다 호출될 이벤트입니다.
	UFUNCTION(BlueprintNativeEvent)
	void OnTaskTickEvent(float DeltaTime);
	virtual void OnTaskTickEvent_Implementation(float DeltaTime);
	void UpdateCueDataFromAdditionalData(UObject* AdditionalData, TArray<FAbilityCueInformation>& CueInfos);

	UFUNCTION(BlueprintCallable)
	void ApplyCueInstance(class ABaseCharacter* Target, UObject* AdditionalData);
	UFUNCTION(BlueprintCallable)
	void ApplyCueInterval(class ABaseCharacter* Target, UObject* AdditionalData);
	UFUNCTION(BlueprintCallable)
	void ApplyCueInfinite(ABaseCharacter* Target, UObject* AdditionalData);
	UFUNCTION(BlueprintCallable)
	void ApplyEndInstanceCue(ABaseCharacter* Target, UObject* AdditionalData);

	void WhenProcessEffectFailed(ABaseCharacter* Target);

	UFUNCTION(BlueprintNativeEvent)
	class UAbilityEffectAdditionalInformation* CreateAdditionalInfo();
	virtual UAbilityEffectAdditionalInformation* CreateAdditionalInfo_Implementation();

	//적용된 큐를 모두 제거합니다.
	//재 사용 가능한 큐는 제거하지 않고, 비활성화 합니다.
	UFUNCTION(BlueprintCallable)
	void RemoveAppliedCues(ABaseCharacter* Target);


	//만약 이 이팩트 타입이 지속시간 형일때, 적용되던 타이머를 재 시작합니다.
	UFUNCTION(BlueprintCallable)
	void ResetDurationTimer(ABaseCharacter* Target);


	UPROPERTY(Transient)
	TObjectPtr<UWorld> World;

	virtual void BeginDestroy() override;
	virtual UWorld* GetWorld() const override;
};
