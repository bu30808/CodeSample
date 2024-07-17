// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "AttributeComponent.generated.h"

enum class EAttributePointType : uint8;
enum class EAttributeType : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChangedAttribute, float, curValue, float, maxValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangedMaxAttribute, float, maxValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamagedHP, float, damage);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterInformationUpdate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateEXP, float, addExp);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnChangeAttributeEffectAdditionalInformation, const FAttributeEffect&,
                                               Effect, class UAbilityEffectAdditionalInformation*,
                                               AdditionalInformation, float, DeltaTime);



DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangedMoveSpeedAttribute);

USTRUCT()
struct FAttributeInit : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=HP)
	int32 HP = 0;

	UPROPERTY(EditAnywhere, Category=SP)
	int32 SP = 0;

	UPROPERTY(EditAnywhere, Category=MP)
	int32 MP = 0;

	UPROPERTY(EditAnywhere, Category=HP)
	int32 MaxHP = 0;

	UPROPERTY(EditAnywhere, Category=SP)
	int32 MaxSP = 0;

	UPROPERTY(EditAnywhere, Category=MP)
	int32 MaxMP = 0;


	//********************초당 회복량********************

	UPROPERTY(EditAnywhere, Category=HP)
	float RecoverHP = 0;


	UPROPERTY(EditAnywhere, Category=SP)
	float RecoverSP = 0;


	//********************공격력********************
	UPROPERTY(EditAnywhere, Category=Damage)
	float PhysicalAttack = 0;

	UPROPERTY(EditAnywhere, Category=Damage)
	float MagicalAttack = 0;


	//********************방어력********************
	UPROPERTY(EditAnywhere, Category=Defence)
	float PhysicalDefense = 0;

	UPROPERTY(EditAnywhere, Category=Defence)
	float MagicalDefense = 0;


	//********************행동속도********************
	UPROPERTY(EditAnywhere, Category=ActionSpeed)
	float ActionSpeed = 1.f;


	//********************강인도********************
	UPROPERTY(EditAnywhere, Category=Endurance)
	int32 Endurance = 0;


	//********************레벨업시 투자 가능한 스텟*******************
	UPROPERTY(EditAnywhere, Category=Investable)
	int32 Strength = 0;

	UPROPERTY(EditAnywhere, Category=Investable)
	int32 Dexterity = 0;

	UPROPERTY(EditAnywhere, Category=Investable)
	int32 Intelligence = 0;

	UPROPERTY(EditAnywhere, Category=Investable)
	int32 Willpower = 0;

	UPROPERTY(EditAnywhere, Category=Investable)
	int32 MaxHPPoint = 0;

	UPROPERTY(EditAnywhere, Category=Investable)
	int32 MaxSPPoint = 0;

	UPROPERTY(EditAnywhere, Category=Investable)
	int32 MaxMPPoint = 0;

	//********************저항력*******************
	UPROPERTY(EditAnywhere, Category="Resistance")
	int32 PoisonResist = 0;
	UPROPERTY(EditAnywhere, Category="Resistance")
	int32 DeadlyPoisonResist = 0;
	UPROPERTY(EditAnywhere, Category="Resistance")
	int32 BurnResist = 0;
	UPROPERTY(EditAnywhere, Category="Resistance")
	int32 ChillResist = 0;
	UPROPERTY(EditAnywhere, Category="Resistance")
	int32 BleedResist = 0;
	UPROPERTY(EditAnywhere, Category="Resistance")
	int32 PetrifactionResist = 0;

	//********************이동속도*******************
	UPROPERTY(EditAnywhere, Category="MoveSpeed")
	float MoveSpeed = 400;

	void Override(const FAttributeInit& Other)
	{
		HP = Other.HP;
		MP = Other.MP;
		SP = Other.SP;

		MaxHP = Other.MaxHP;
		MaxMP = Other.MaxMP;
		MaxSP = Other.MaxSP;

		RecoverHP = Other.RecoverHP;
		RecoverSP = Other.RecoverSP;

		PhysicalAttack = Other.PhysicalAttack;
		MagicalAttack = Other.MagicalAttack;

		PhysicalDefense = Other.PhysicalDefense;
		MagicalDefense = Other.MagicalDefense;

		ActionSpeed = Other.ActionSpeed;

		Endurance = Other.Endurance;

		Strength = Other.Strength;
		Dexterity = Other.Dexterity;
		Intelligence = Other.Intelligence;
		Willpower = Other.Willpower;

		MaxHPPoint = Other.MaxHPPoint;
		MaxMPPoint = Other.MaxMPPoint;
		MaxSPPoint = Other.MaxSPPoint;


		PoisonResist = Other.PoisonResist;
		DeadlyPoisonResist = Other.DeadlyPoisonResist;
		BurnResist = Other.BurnResist;
		ChillResist = Other.ChillResist;
		BleedResist = Other.BleedResist;
		PetrifactionResist = Other.PetrifactionResist;

		MoveSpeed = Other.MoveSpeed;
	}
};

USTRUCT()
struct FAttributePerPoint : public FTableRowBase
{
	GENERATED_BODY()

public:
	/***********힘*************/
	UPROPERTY(EditAnywhere, Category=Str)
	float PhysicalAttackPerStrength = 0;

	/***********민*************/
	UPROPERTY(EditAnywhere, Category=Dex)
	float PhysicalAttackPerDexterity = 0;
	UPROPERTY(EditAnywhere, Category=Dex)
	float MagicalAttackPerDexterity = 0;
	UPROPERTY(EditAnywhere, Category=Dex)
	float ActionSpeedPerDexterity = 0;

	/***********지*************/
	UPROPERTY(EditAnywhere, Category=Int)
	float MaxMPPerIntelligence = 0;
	UPROPERTY(EditAnywhere, Category=Int)
	float MagicalAttackPerIntelligence = 0;

	/***********의*************/
	UPROPERTY(EditAnywhere, Category=Will)
	float MaxHPPerWillpower = 0;
	UPROPERTY(EditAnywhere, Category=Will)
	float MaxMPPerWillpower = 0;
	UPROPERTY(EditAnywhere, Category=Will)
	float MaxSPPerWillpower = 0;

	UPROPERTY(EditAnywhere, Category=Will)
	float PhysicalDefensePerWillpower = 0;
	UPROPERTY(EditAnywhere, Category=Will)
	float MagicalDefensePerWillpower = 0;
	UPROPERTY(EditAnywhere, Category=Will)
	float ActionSpeedPerWillpower = 0;
	UPROPERTY(EditAnywhere, Category=Will)
	float EndurancePerWillpower = 0;
	UPROPERTY(EditAnywhere, Category=Will)
	float ResistPerWillpower = 0;

	/***********최대치*************/
	UPROPERTY(EditAnywhere, Category=Max)
	float MaxHPPerPoint = 0;
	UPROPERTY(EditAnywhere, Category=Max)
	float MaxSPPerPoint = 0;
	UPROPERTY(EditAnywhere, Category=Max)
	float MaxMPPerPoint = 0;
};

USTRUCT(BlueprintType)
struct FAttribute
{
	GENERATED_BODY()

protected:
	//기본값.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Value")
	float Base = 0;

	//다른 이팩트에 영향을 받은 현재값.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Value")
	float Current = 0;

public:
	float GetBase() const { return Base; }
	float GetCurrent() const { return Current; }

	void SetBase(float Val) { Base = Val; }

	void SetCurrent(float Val)
	{
		Current = Val;
		if (Current < 0)
		{
			Current = 0;
		}
	}

	void Init(float Val)
	{
		SetBase(Val);
		SetCurrent(Val);
	}

	void Load(const FAttribute& LoadValue)
	{
		this->Base = LoadValue.Base;
		this->Current = LoadValue.Current;
	}

	void LevelUp(const int32& AddLevel)
	{
		Base += AddLevel;
		Current += AddLevel;
	}
};

#define ATTRIBUTE_STRUCTURE_GETTER(attributeName) public: const FAttribute& Get##attributeName##Attribute() const {return attributeName;}

#define ATTRIBUTE_GETTER(attributeName) public:	float Get##attributeName() const {return attributeName.GetCurrent();}
#define ATTRIBUTE_SETTER(attributeName) public: void Set##attributeName(float NewValue){ attributeName.SetCurrent(NewValue);}
#define ATTRIBUTE_INITTER(attributeName) private: void Init##attributeName(float NewValue){ attributeName.Init(NewValue);}
#define ATTRIBUTE_GET_SET(attributeName) ATTRIBUTE_GETTER(attributeName); ATTRIBUTE_SETTER(attributeName); ATTRIBUTE_INITTER(attributeName); ATTRIBUTE_STRUCTURE_GETTER(attributeName); protected:

UENUM(BlueprintType)
enum class EStatusEffect : uint8
{
	//독
	POISON,
	//맹독
	DEADLY_POISON,
	//화상
	BURN,
	//한기
	CHILL,
	//빙결
	FREEZE,
	//출혈
	BLEED,
	//석화
	PETRIFACTION,
	MAX
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnUpdateStatusEffect, EStatusEffect, StatusEffect, float, Value, float,
                                               ResistValue);

UCLASS()
class SOULLIKE_API UStatusEffectValueHandler : public UObject
{
	GENERATED_BODY()

public:
	UStatusEffectValueHandler();

protected:
	//상태이상 누적치를 얼마만큼씩 감소시킬지 정의하는 맵입니다.
	UPROPERTY()
	TMap<EStatusEffect, float> ReduceValueDefineMap;

public:
	UPROPERTY()
	EStatusEffect TargetStatusEffect;
	UPROPERTY()
	class UAttributeComponent* AttributeComponent;


	void Init(class UAttributeComponent* AttComp, EStatusEffect StatusEffect);
	UFUNCTION()
	void ReduceAccValue(float DeltaTime);
};

UCLASS()
class SOULLIKE_API UStatusEffectData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TMap<EStatusEffect, TSubclassOf<class UAbilityEffect>> StatusEffect;
};

/*
 *	캐릭터의 속성을 정의하고 관리하는 컴포넌트입니다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULLIKE_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()
#if  WITH_EDITOR
	friend class UAttributeHandlerWidget;
#endif

	friend class UGameLoadHandler;

public:
	// Sets default values for this component's properties
	UAttributeComponent();

protected:
	
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void PostInitProperties() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	UPROPERTY(EditAnywhere)
	class UDataTable* AttributeInitTable;

	UPROPERTY(EditAnywhere)
	class UDataTable* AttributePerPointDefineTable;

	//어트리뷰트당 증가하는 능력치를 저장하는 변수입니다.
	UPROPERTY(VisibleAnywhere)
	FAttributePerPoint AttributePerPoint;

	UPROPERTY(EditAnywhere)
	class UStatusEffectData* StatusEffectData;

	//키에 해당하는 능력치에 투자하면 어떤 다른 능력치가 얼마나 오르는지 기억하고 있는 변수입니다.
	TMap<EAttributeType, TArray<TPair<EAttributeType, float>>> AttributePerPointMap;

public:
	const TMap<EAttributeType, TArray<TPair<EAttributeType, float>>>& GetAttributePerPointMap()
	{
		return AttributePerPointMap;
	}

	const FAttributePerPoint& GetAttributePerPoint() { return AttributePerPoint; }

	//필요한 어트리뷰트를 초기화 합니다.
	void InitAttributes();
	//레벨업 포인트를 제외한 어트리뷰트를 복구합니다.
	void LoadAttributeNotIncludeLevelUpPoint(const TMap<EAttributeType, FAttribute>& SavedAttribute,
	                                         bool ShouldUpdateProgressBar, bool bIsRespawn);
	//레벨업 포인트를 복구합니다.
	void LoadLevelUpPointAttributes(const TMap<EAttributeType, FAttribute>& LevelUpPoint);
	//모든 상태이상을 제거하고, 누적수치를 초기화 합니다.
	void ClearStatusEffect();

	UFUNCTION(BlueprintCallable)
	void RemoveStatusEffect(EStatusEffect StatusEffectToRemove);
	
protected:
	//포인트당 속성 증가치를 가져옵니다.
	void InitAttributePerPoint();
	//기본 속성치를 가져옵니다.
	void InitDefaultAttribute();

public:
	//레벨업시 어떤 어트리뷰트에 몇 포인트를 투자했는지 받아와서 반영합니다.
	void AddLevelUpPoint(EAttributePointType AttributePointType, const int32& Point);

	//장비나 아이템에 의해서 Cur값이 변경되었을 때, 갱신합니다.
	//void RecalculateAttributeValues();
protected:
	//편의를 위해 어트리뷰트를 타입별로 구분해 저장하는 변수입니다. 절대 덮어쓰지 마세요.
	//컴포넌트 초기화시 할당됩니다.
	TMap<EAttributeType, FAttribute*> AttributesNotIncludeLevelUpPoint;
	//레벨업시 투자한 포인트 정보를 담은 맵입니다. 절대 덮어쓰지 마세요.
	TMap<EAttributeType, FAttribute*> LevelUpPointAttributes;

public:
	const TMap<EAttributeType, FAttribute*>& GetAllAttributeNotIncludeLevelUpPoint()
	{
		return AttributesNotIncludeLevelUpPoint;
	}

	const FAttribute* GetAttributeByType(EAttributeType Type);

	const TMap<EAttributeType, FAttribute*>& GetAllLevelUpAttribute() { return LevelUpPointAttributes; }
	const FAttribute* GetLevelUpAttributeByType(EAttributeType Type);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute Level;
	ATTRIBUTE_GET_SET(Level)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute EXP;
	ATTRIBUTE_GET_SET(EXP)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute HP;
	ATTRIBUTE_GET_SET(HP)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute SP;
	ATTRIBUTE_GET_SET(SP)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute MP;
	ATTRIBUTE_GET_SET(MP)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute MaxHP;
	ATTRIBUTE_GET_SET(MaxHP)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute MaxSP;
	ATTRIBUTE_GET_SET(MaxSP)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute MaxMP;
	ATTRIBUTE_GET_SET(MaxMP)
	//********************초당 회복량********************

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute RecoverHP;
	ATTRIBUTE_GET_SET(RecoverHP)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute RecoverSP;
	ATTRIBUTE_GET_SET(RecoverSP)
	//********************타격시 회복되는 신력량********************
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute RecoverHitMP;
	ATTRIBUTE_GET_SET(RecoverHitMP)
	//********************공격력********************
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute PhysicalAttack;
	ATTRIBUTE_GET_SET(PhysicalAttack)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute MagicalAttack;
	ATTRIBUTE_GET_SET(MagicalAttack)
	//********************방어력********************
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute PhysicalDefense;
	ATTRIBUTE_GET_SET(PhysicalDefense)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute MagicalDefense;
	ATTRIBUTE_GET_SET(MagicalDefense)
	//********************행동속도********************
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute ActionSpeed;
	ATTRIBUTE_GET_SET(ActionSpeed)
	//********************강인도********************
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute Endurance;
	ATTRIBUTE_GET_SET(Endurance)
	//********************주요 4속성*******************
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute Strength;
	ATTRIBUTE_GET_SET(Strength)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute Dexterity;
	ATTRIBUTE_GET_SET(Dexterity)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute Intelligence;
	ATTRIBUTE_GET_SET(Intelligence)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute Willpower;
	ATTRIBUTE_GET_SET(Willpower)
	//********************레벨업시 투자 가능한 스텟*******************
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute StrengthPoint;
	ATTRIBUTE_GET_SET(StrengthPoint)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute DexterityPoint;
	ATTRIBUTE_GET_SET(DexterityPoint)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute IntelligencePoint;
	ATTRIBUTE_GET_SET(IntelligencePoint)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute WillpowerPoint;
	ATTRIBUTE_GET_SET(WillpowerPoint)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute MaxHPPoint;
	ATTRIBUTE_GET_SET(MaxHPPoint)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute MaxSPPoint;
	ATTRIBUTE_GET_SET(MaxSPPoint)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute MaxMPPoint;
	ATTRIBUTE_GET_SET(MaxMPPoint)
	//********************저항력*******************
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resist")
	FAttribute PoisonResist;
	ATTRIBUTE_GET_SET(PoisonResist)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resist")
	FAttribute DeadlyPoisonResist;
	ATTRIBUTE_GET_SET(DeadlyPoisonResist)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resist")
	FAttribute BurnResist;
	ATTRIBUTE_GET_SET(BurnResist)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resist")
	FAttribute ChillResist;
	ATTRIBUTE_GET_SET(ChillResist)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resist")
	FAttribute BleedResist;
	ATTRIBUTE_GET_SET(BleedResist)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resist")
	FAttribute PetrifactionResist;
	ATTRIBUTE_GET_SET(PetrifactionResist)
	//********************누적된 상태이상 수치 이 값이 저항력과 같아지면 상태이상을 적용합니다. *******************
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="StatusEffectAcc")
	FAttribute PoisonAcc;
	ATTRIBUTE_GET_SET(PoisonAcc)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="StatusEffectAcc")
	FAttribute DeadlyPoisonAcc;
	ATTRIBUTE_GET_SET(DeadlyPoisonAcc)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="StatusEffectAcc")
	FAttribute BurnAcc;
	ATTRIBUTE_GET_SET(BurnAcc)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="StatusEffectAcc")
	FAttribute ChillAcc;
	ATTRIBUTE_GET_SET(ChillAcc)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="StatusEffectAcc")
	FAttribute BleedAcc;
	ATTRIBUTE_GET_SET(BleedAcc)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="StatusEffectAcc")
	FAttribute PetrifactionAcc;
	ATTRIBUTE_GET_SET(PetrifactionAcc)
	//누적값 감소용 타이머를 저장하는 맵
	UPROPERTY()
	TMap<EStatusEffect, TWeakObjectPtr<class UGameplayTask_LaunchEvent>> DecreaseAccTasks;

	//********************이동속도*******************
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute MoveSpeed;
	ATTRIBUTE_GET_SET(MoveSpeed)
	
	//********************기술 슬롯*******************
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	FAttribute MemorySlot;
	ATTRIBUTE_GET_SET(MemorySlot)
public:
	//위젯에서 호출됩니다. 바인딩된 이벤트를 한번 내부에서 호출해서 프로그래스바를 갱신합니다.
	void InitProgressWidget() const;
	/*******************이벤트***************************/
	//체력값이 변하면 호출되는 이벤트입니다.
	FOnChangedAttribute OnChangeHPValue;
	//마력값이 변하면 호출되는 이벤트입니다.
	FOnChangedAttribute OnChangeMPValue;
	//스테값이 변하면 호출되는 이벤트입니다.
	FOnChangedAttribute OnChangeSPValue;


	//피해를 받아 HP가 변하면 호출됩니다.
	FOnDamagedHP OnDamagedHP;
	

	//최대 체력값이 변하면 호출되는 이벤트입니다.
	FOnChangedMaxAttribute OnChangeMaxHPValue;
	//최대 마력값이 변하면 호출되는 이벤트입니다.
	FOnChangedMaxAttribute OnChangeMaxMPValue;
	//최대 스테값이 변하면 호출되는 이벤트입니다.
	FOnChangedMaxAttribute OnChangeMaxSPValue;

	//위젯에 캐릭터 정보를 연동하는 이벤트입니다.
	FOnCharacterInformationUpdate OnCharacterInformationUpdate;
	//스테이터스 이팩트가 업데이트되면 호출되는 이벤트입니다.
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnUpdateStatusEffect OnUpdateStatusEffect;

	//경험치 획득시 호출됩니다.
	FOnUpdateEXP OnUpdateExp;

	//이동속도 어트리뷰트가 프로세서를 통해 변경되었을 때 호출됩니다.
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnChangedMoveSpeedAttribute OnChangedMoveSpeedAttribute;

	FOnChangeAttributeEffectAdditionalInformation OnAddAttributeEffectAdditionalInformation;
	//어빌리티 이팩트가 적용되어 속성값이 제거될 때, 추가정보를 받은 경우 호출되는 이벤트입니다.
	//주로 피격처리를 처리하기 위해 만들어졌습니다.
	FOnChangeAttributeEffectAdditionalInformation OnRemoveAttributeEffectAdditionalInformation;
	FOnChangeAttributeEffectAdditionalInformation OnMultiplicationAttributeEffectAdditionalInformation;
	FOnChangeAttributeEffectAdditionalInformation OnDivisionAttributeEffectAdditionalInformation;
	FOnChangeAttributeEffectAdditionalInformation OnSetAttributeEffectAdditionalInformation;

	void BroadcastHPEvent(float Damage = 0) const;
	void BroadcastMPEvent() const;
	void BroadcastSPEvent() const;

	void BroadcastMaxHPEvent() const;
	void BroadcastMaxMPEvent() const;
	void BroadcastMaxSPEvent() const;

	UFUNCTION()
	void OnUpdateStatusEffectEvent(EStatusEffect StatusEffect, float Value, float ResistValue);

	//경험치 획득시 저장하기 위해 사용도비니다.
	UFUNCTION()
	void OnUpdateExpEvent(float AddExp);
};
