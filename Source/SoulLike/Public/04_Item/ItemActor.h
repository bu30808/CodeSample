// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "02_Ability/AbilityEffect.h"
#include "95_OrbCreator/OrbMatrix.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "ItemActor.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Common,
	// 일반
	Rare,
	// 레어
	Epic,
	// 에픽
	Legendary // 전설
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	NONE,
	CONSUME,
	EQUIPMENT,
	ABILITY,
	ENHANCEMENT,
	KEY,
	ETC,
};

UENUM(BlueprintType)
enum class EEquipType : uint8
{
	NONE,
	SPIRIT,
	ARMOR,
	RING,
	ORB
};

//특정 강화수치에서 행 이름으로 강화하는데 필요한 소재 요구사항을 정의합니다.
//소재 이름은 GPT에게 추천받았습니다.
USTRUCT(BlueprintType)
struct FEnhancementMaterial : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int32 NeedSouls = 0;
	//특정 태그를 가진 아이템이 몇개나 필요한지 정의합니다.
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, int32> NeedItems;
};

//강화당 증가하는 능력치를 정의합니다.
USTRUCT(BlueprintType)
struct FAttributeValuePerEnhancement : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float AddValue = 0;
};

//강화 수치정보를 저장하는 변수입니다.
USTRUCT(BlueprintType)
struct FEnhancement
{
	GENERATED_BODY()

protected:
	//최대 강화 횟수
	UPROPERTY(EditAnywhere)
	int32 MaxEnhancement = 5;

public:
	//현재 강화 횟수
	UPROPERTY(VisibleAnywhere)
	int32 CurEnhancement = 0;

protected:
	//강화가 허용된 능력치입니다.
	//이 변수로 특정 장비마다 강화 가능한 능력치를 다르게 줄 수 있습니다.
	UPROPERTY(EditAnywhere)
	TArray<EAttributeType> AllowEnhancement;

public:
	//어떤 타입의 어트리뷰트가 몇번 강화되었는지 저장하는 맵.
	UPROPERTY(VisibleAnywhere)
	TMap<EAttributeType, int32> EnhancementMap;

	FEnhancement()
	{
		AllowEnhancement.AddUnique(EAttributeType::MaxHP);
		AllowEnhancement.AddUnique(EAttributeType::MaxMP);
		AllowEnhancement.AddUnique(EAttributeType::MaxSP);

		AllowEnhancement.AddUnique(EAttributeType::Strength);
		AllowEnhancement.AddUnique(EAttributeType::Dexterity);
		AllowEnhancement.AddUnique(EAttributeType::Intelligence);
		AllowEnhancement.AddUnique(EAttributeType::Willpower);

		AllowEnhancement.AddUnique(EAttributeType::ActionSpeed);

		AllowEnhancement.AddUnique(EAttributeType::Endurance);

		for (auto att : AllowEnhancement)
		{
			EnhancementMap.Add(att);
		}
	}

	//강화가 더 가능한가요?
	bool CanEnhance() const
	{
		return MaxEnhancement > CurEnhancement;
	}

	const int32& GetMaxEnhancement() const { return MaxEnhancement; }

	const TArray<EAttributeType>& GetAllowEnhancement() const { return AllowEnhancement; }

	void IncreaseEnhance(EAttributeType IncreaseAttribute)
	{
		CurEnhancement = FMath::Clamp(CurEnhancement + 1, CurEnhancement, MaxEnhancement);
		EnhancementMap[IncreaseAttribute] += 1;
	}

	//능력치 상승 없이 그냥 강화수치만 올립니다. 회복약 강화시 사용됩니다.
	void IncreaseEnhance()
	{
		CurEnhancement = FMath::Clamp(CurEnhancement + 1, CurEnhancement, MaxEnhancement);
	}
};


USTRUCT(BlueprintType)
struct FItemInformation : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<class AItemActor> ItemActorObject;

	//절대 중복되면 안 됨.
	UPROPERTY(EditAnywhere)
	FGameplayTag Item_Tag = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere)
	EItemType Item_Type = EItemType::NONE;
	//아이템 이미지
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<class UTexture2D> Item_Image;
	//아이템 이름
	UPROPERTY(EditAnywhere)
	FText Item_Name;

	//아이템 설명(ex. 체력을 {0}만큼 회복합니다)
	UPROPERTY(EditAnywhere,meta=(MultiLine = "true"))
	FText Item_Description;

	//아이템 가격
	UPROPERTY(EditAnywhere)
	int32 Item_Price = 0;
	//같은 아이템인 경우 갯수만 누적시킬 것인지?
	UPROPERTY(EditAnywhere)
	bool bStackable = true;


	//최대 겹침 갯수를 정합니다.
	//-1인 경우 무한대로 겹칩니다.
	UPROPERTY(EditAnywhere, meta=(EditCondition = "bStackable == true"))
	int32 MaxStack = -1;


	//카운트가 0이 되었을 때 인벤토리에서 이 아이템을 제거할 것인가?
	UPROPERTY(EditAnywhere)
	bool bRemoveWhenCountZero = false;

	//팔수 있는가?
	UPROPERTY(EditAnywhere)
	bool bSellable = true;

	//이 값이 참이면 인벤토리에서 직접 버릴 수 없습니다.
	UPROPERTY(EditAnywhere)
	bool bRemovable = true;


	//이 아이템을 사용했을 때, 바로 사용할 어빌리티
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UAbilityBase>> Abilities;
};

USTRUCT()
struct FEquipmentAttribute
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	EAttributeType AttributeType = EAttributeType::NONE;
	UPROPERTY(EditAnywhere)
	float Value = 0;

	FEquipmentAttribute()
	{
	}

	FEquipmentAttribute(EAttributeType Attribute, float RandValue): AttributeType(Attribute), Value(RandValue)
	{
	}

	bool operator==(const EAttributeType Type) const
	{
		return AttributeType == Type;
	}
};

USTRUCT(BlueprintType)
struct FEquipmentInformation : public FItemInformation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EEquipType EquipType = EEquipType::NONE;

	//어느 능력치를 얼마나 상승시키는가 정의하는 변수입니다.
	//이 변수를 가지고 장비 어빌리티 내부의 SelfEffect를 생성 및 할당합니다.
	UPROPERTY(EditAnywhere)
	TArray<FEquipmentAttribute> EquipmentAttribute;

	//실제 강화 수치는 장비 아이템 액터 내부 Enhancement변수가 들고 있습니다.
	//GetItemInfo로 가져온 이 변수 내부의 값은 비어있습니다.
	UPROPERTY(EditAnywhere)
	FEnhancement Enhancement;

	FEquipmentInformation()
	{
		Item_Type = EItemType::EQUIPMENT;
		bStackable = false;
	}
};


USTRUCT(BlueprintType)
struct FWeaponInformation : public FEquipmentInformation
{
	GENERATED_BODY()

	//무기에 사용할 에니메이션 인스턴스
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UAnimInstance> AnimationSet = nullptr;

	//부여할 어빌리티, 부여만 하지 즉시 사용하진 않습니다.
	UPROPERTY(EditAnywhere)
	TArray<TSoftClassPtr<class UAbilityBase>> GiveAbilities;

	FWeaponInformation()
	{
		EquipType = EEquipType::SPIRIT;
	}
};


USTRUCT(BlueprintType)
struct FConsumeInformation : public FItemInformation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Value)
	int32 EffectValue = 0;

	FConsumeInformation()
	{
		Item_Type = EItemType::CONSUME;
	}
};


/*
USTRUCT(BlueprintType)
struct FPotionEnhancement
{
	GENERATED_BODY()

protected:
	//최대 강화 횟수
	UPROPERTY(EditAnywhere)
	int32 MaxEnhancement = 10;
	//강화에 필요한 소재를 정의합니다.
	UPROPERTY(EditAnywhere)
	TArray<FGameplayTag> EnhancementRequireItemTag;

public:
	//현재 강화 횟수
	UPROPERTY(VisibleAnywhere)
	int32 CurEnhancement = 0;

	//강화가 더 가능한가요?
	
bool CanEnhance(UInventoryComponent* InventoryComponent) const
{
	if (InventoryComponent)
	{
		//최대강화수치보다 현 강화수치가 작을 경우,
		if (MaxEnhancement > CurEnhancement)
		{
			for (const auto& tag : EnhancementRequireItemTag)
			{
				//모든 강화소재를 다 가지고 있을 때,
				if (!InventoryComponent->K2_HasItem(tag))
				{
					return false;
				}
			}

			return true;
		}
	}
	return false;
}


	const int32& GetMaxEnhancement() const { return MaxEnhancement; }


void IncreaseEnhance()
{
	CurEnhancement = FMath::Clamp(CurEnhancement + 1, CurEnhancement, MaxEnhancement);
}
};
*/

USTRUCT(BlueprintType)
struct FPotionInformation : public FConsumeInformation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FEnhancement Enhancement;
};


USTRUCT(BlueprintType)
struct FOrbInformation : public FEquipmentInformation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EOrbType OrbType = EOrbType::CORE;
};


USTRUCT(BlueprintType)
struct FOrbCoreInformation : public FOrbInformation
{
	GENERATED_BODY()
	/*friend class AEquipmentItemActor_OrbCore;*/

public:
	//원본입니다. 직접 불러 쓰지 마세요.
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<class UOrbMatrix> OrbMatrix_Original;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UOrbMatrix> OrbMatrix = nullptr;

	UPROPERTY(EditAnywhere, meta=(EditCondition= "OrbType == EOrbType::CORE"))
	FLinearColor CoreColor = FLinearColor::White;

	FOrbCoreInformation()
	{
		Item_Type = EItemType::EQUIPMENT;
		EquipType = EEquipType::ORB;
		OrbType = EOrbType::CORE;
		bStackable = false;

		Item_Image = FSoftObjectPath("/Script/Engine.Texture2D'/Game/CommonResource/Icon/Orb/orbCore.orbCore'");
	}

	//class UOrbMatrix* GetLoadMatrix() const {return OrbMatrixObject;}
};

USTRUCT(BlueprintType)
struct FOrbFragmentInformation : public FOrbInformation
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	EOrbMatrixSlotType SlotType = EOrbMatrixSlotType::NONE;
	UPROPERTY(EditAnywhere)
	EItemRarity Rarity = EItemRarity::Common;

	FOrbFragmentInformation()
	{
		Item_Type = EItemType::EQUIPMENT;
		EquipType = EEquipType::ORB;
		OrbType = EOrbType::FRAGMENT;
		bStackable = false;

		Item_Image = FSoftObjectPath("/Script/Engine.Texture2D'/Game/CommonResource/Icon/Orb/orbFragment.orbFragment'");
	}
};


UCLASS()
class SOULLIKE_API AItemActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItemActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//오너가 플레이어가 아닐 때(필드 아이템) 이미 획득한 상태라면 제거합니다.
	void DestroyIfPlayerAlreadyGetThisItemFromField();

	UPROPERTY(VisibleAnywhere, Category=Component, meta=(AllowPrivateAccess="true"))
	class USphereComponent* SphereComponent;
	UPROPERTY(VisibleAnywhere, Category=Component, meta=(AllowPrivateAccess="true"))
	class UNiagaraComponent* NiagaraComponent;
	UPROPERTY(VisibleAnywhere, Category=Component, meta=(AllowPrivateAccess="true"))
	class UBillboardComponent* BillboardComponent;

	UPROPERTY(EditAnywhere, Category=Info)
	FGameplayTag ItemTag;
	UPROPERTY(EditAnywhere, Category=Info)
	int32 ItemCount = 1;
	UPROPERTY(VisibleAnywhere, Category=Info)
	FText NotFormattedDescription;
	//아이템 사용시 사용자에게 등록한 어빌리티가 저장되는 변수입니다.
	UPROPERTY()
	TArray<class UAbilityBase*> GivenAbility;

	//인벤토리 컴포넌트를 통해 아이템을 사용하기 위해 인벤토리아이템 구조체로부터 받아오는 유니크 아이디 값입니다.
	UPROPERTY()
	FGuid ItemUniqueID;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void PostInitializeComponents() override;

public:
	class USphereComponent* GetSphereComponent() { return SphereComponent; }
	const int32& GetItemCount() const { return ItemCount; }
	void SetItemCount(int32 NewCount) { ItemCount = NewCount; }

	//아이템 정보를 서브시스템으로부터 읽어옵니다. 아이템 종류별로 덮어쓰세요.
	virtual const FItemInformation* GetItemInformation();

	//이 아이템의 태그값을 가져옵니다.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FGameplayTag& GetItemTag() { return ItemTag; }

	//아이템의 유니크 아이디값을 가져옵니다.
	/*UFUNCTION(BlueprintCallable,BlueprintPure)
	const FGuid& GetUniqueID(){return GetItemInformation()->UniqueID;}*/

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FText GetNotFormattedDescription();

	//포멧화된 아이템 설명을 리턴합니다. 필요에 의해 블루프린트에서 덮어쓰세요.
	UFUNCTION(BlueprintNativeEvent)
	FText GetFormattedDescription();
	virtual FText GetFormattedDescription_Implementation() { return NotFormattedDescription; }


	//아이템 사용 효과를 적용합니다. 필요에 의해 덮어쓰세요.
	//어빌리티를 등록하는 경우 해당 어빌리티가 끝나고 반드시 컴포넌트에서 제거해야 합니다.
	UFUNCTION(BlueprintNativeEvent)
	bool UseItem(AActor* Target, const FGuid& ThisItemsUniqueID);
	virtual bool UseItem_Implementation(AActor* Target, const FGuid& ThisItemsUniqueID);


	//어빌리티 내부 셀프 이팩트를 덮어쓰려면 구현하세요.
	UFUNCTION(BlueprintNativeEvent)
	void OnOverrideSelfEffectEvent(const TArray<class UAbilityEffect*>& SelfEffect, AActor* Target);

	virtual void OnOverrideSelfEffectEvent_Implementation(const TArray<UAbilityEffect*>& SelfEffect, AActor* Target)
	{
	}

	//어빌리티 내부 타겟 이팩트를 덮어쓰려면 구현하세요.
	UFUNCTION(BlueprintNativeEvent)
	void OnOverrideTargetEffectEvent(const TArray<class UAbilityEffect*>& TargetEffect, AActor* Target);

	virtual void OnOverrideTargetEffectEvent_Implementation(const TArray<class UAbilityEffect*>& TargetEffect,
	                                                        AActor* Target)
	{
	}


	//아이템 어빌리티가 끝났을때 무엇인가 하려면 덮어쓰세요.
	UFUNCTION(BlueprintNativeEvent)
	void OnEndAbilityEvent();
	virtual void OnEndAbilityEvent_Implementation();

	//장비아이템인경우 장착해제합니다.
	UFUNCTION(BlueprintNativeEvent)
	void UnEquip(AActor* Target, const FGuid& ThisItemsUniqueID);

	virtual void UnEquip_Implementation(AActor* Target, const FGuid& ThisItemsUniqueID)
	{
	}

	//스폰전에 사전설정이 필요하다면 덮어쓰세요.
	//지금은 오브 프래그먼트 사전설정을 위해 사용합니다.
	virtual void SpawnPreSetting()
	{
	}

	void SetItemTag(const FGameplayTag& NewTag);

	//이 아이템이 스폰되어 몬스터에 들러붙어있는 도중에, 몬스터가 Destroy된 경우에 호출됩니다.
	UFUNCTION()
	void OnAttachedOwnerDestroyEvent(AActor* DestroyedActor);
};
