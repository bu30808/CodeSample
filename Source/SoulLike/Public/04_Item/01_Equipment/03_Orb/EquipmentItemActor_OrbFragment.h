// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "04_Item/01_Equipment/EquipmentItemActor.h"
#include "Engine/DataAsset.h"
#include "EquipmentItemActor_OrbFragment.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogFragment, Log, All);

USTRUCT(BlueprintType)
struct FRandomAbilityArray
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UAbilityBase>> Ability;
};

UCLASS(BlueprintType)
class SOULLIKE_API URandomAbilityDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EOrbMatrixSlotType, FRandomAbilityArray> RandomAbility;
};

/**
 * 
 */
UCLASS()
class SOULLIKE_API AEquipmentItemActor_OrbFragment : public AEquipmentItemActor
{
	GENERATED_BODY()

protected:
	AEquipmentItemActor_OrbFragment();
	//이 값이 참일때만 프래그먼트 정보를 생성합니다.
	//인벤토리 내부에서 아이템 효과 적용을 위해 스폰된 경우, 이 변수를 통해서 쓸때없이 새 정보를 생성하지 못하게 합니다.
	UPROPERTY(VisibleAnywhere)
	bool bCreateInfo = true;

	//생성된 파편 정보가 저장되는 변수입니다.
	//TSharedPtr<FOrbFragmentInformation> FragmentInformation;
	/*
	//이 값이 참이면 랜덤한 능력치를 가진 프래그먼트가 됩니다.
	UPROPERTY(EditAnywhere)
	bool bRandomFragment = true;
	//이 값이 참이면 연관된 값들 사이에서 랜덤값이 설정됩니다 (ex)힘 슬롯인 경우 힘 관련 능력치가 붙습니다.
	UPROPERTY(EditAnywhere, meta=(EditCondition="bRandomFragment"))
	bool bRelativeRandom = true;
	//연관된 슬롯 타입에 따라 랜덤으로 제공할 어빌리티 목록입니다.
	UPROPERTY(EditAnywhere, meta=(EditCondition="bRandomFragment"))
	class URandomAbilityDataAsset* RandomAbilityDataAsset


	//랜덤으로 주어진 어빌리티중, 성공적으로 이 프래그먼트에 의해 등록된 어빌리티의 태그 목록입니다. 이 프래그먼트 장착 해제시 사용됩니다.
	UPROPERTY(VisibleAnywhere)
	TArray<FGameplayTag> GivenRandomAbilitiesByFragment;


	//랜덤하게 생성된 파편 정보를 저장하는 공유포인터변수입니다. 랜덤생성이 아닐 경우 사용되지 않습니다.
	TSharedPtr<FOrbFragmentInformation> FragmentInformation;

	TArray<TSubclassOf<UAbilityBase>> GetRandomAbilities(EOrbMatrixSlotType SlotType);

	EItemRarity GetRandomRarity();
	EOrbMatrixSlotType GetRandomSlotType();
	int32 GetAttributeCountByRarity(EItemRarity Rarity);
	float GetRandomAttributeValue(EAttributeType RandAttribute, EItemRarity Rarity);
	EAttributeType GetRandomAttribute();
	EAttributeType GetRandomRelativeAttribute(EOrbMatrixSlotType SlotType);
	TArray<FEquipmentAttribute> GetRandomEquipAttribute(EOrbMatrixSlotType SlotType, EItemRarity Rarity);

	//랜덤으로 생성되어있던 스텟 및 어빌리티를 부여합니다.
	void ApplyRandomFragment();
	void UnApplyRandomFragment();
*/

	virtual void PostInitializeComponents() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void BeginPlay() override;

	//virtual FString GetFormattedDescription_Implementation() override;
	//void CreateFragmentInfo();

	virtual const FItemInformation* GetItemInformation() override;


	virtual bool UseItem_Implementation(AActor* Target, const FGuid& ThisItemsUniqueID) override;
	virtual void UnEquip_Implementation(AActor* Target, const FGuid& ThisItemsUniqueID) override;

public:
	//아이템 획득시 기존 랜덤정보가 들어있던 액터는 제거되기때문에, 정보를 덮어써야 합니다.
//	void OverrideInfo(const FItemInformation* ItemInfo);
	virtual void SpawnPreSetting() override;

private:
	
};
