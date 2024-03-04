// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "02_Ability/AbilityBase.h"
#include "04_Item/ItemActor.h"
#include "MerchantComponent.generated.h"

enum class EEquipType : uint8;
enum class EItemType : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBuyItemFromPlayer,class APlayerCharacter*,InteractPlayer,const FGuid&,ItemID);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSellItemToPlayer,class APlayerCharacter*,InteractPlayer,const FGuid&,ItemID);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuyAbilityFromPlayer,class APlayerCharacter*,InteractPlayer);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSellAbilityToPlayer,class APlayerCharacter*,InteractPlayer);

USTRUCT()
struct FBaseMerchandiseData : public FTableRowBase
{
	GENERATED_BODY()

	//판매상품의 태그
	UPROPERTY(EditAnywhere, Category="Common")
	FGameplayTag Tag;

	//판매가격
	UPROPERTY(EditAnywhere, Category="Common")
	int32 Price = 0;

	UPROPERTY(EditAnywhere, Category="Common")
	bool bSellInfinite = false;

	//몇개나 팔 것인가
	UPROPERTY(EditAnywhere, meta=(EditCondition="bSellInfinite ==  false"), Category="Common")
	int32 Count = 0;


	FBaseMerchandiseData()
	{
	}

	FBaseMerchandiseData(FGameplayTag MerchandiseTag, int32 MerchandisePrice,
	                     int32 MerchandiseCount) : Tag(MerchandiseTag), Price(MerchandisePrice)
	{
	}
};

USTRUCT()
struct FMerchandiseItemData : public FBaseMerchandiseData
{
	GENERATED_BODY()


	//아이템 정보를 가지고 있는 오브젝트, 태그값과 일치해야 합니다.
	UPROPERTY(EditAnywhere, Category="Item")
	TSoftClassPtr<class AItemActor> ItemActorObject;


	FMerchandiseItemData()
	{
	}

	FMerchandiseItemData(FGameplayTag MerchandiseTag, const AItemActor* ItemActor, int32 MerchandisePrice,
	                     int32 MerchandiseCount) : FBaseMerchandiseData(MerchandiseTag, MerchandisePrice,
	                                                                    MerchandiseCount)
	{
		ItemActorObject = ItemActor->GetClass();
	}
};

USTRUCT()
struct FMerchandiseAbilityData : public FBaseMerchandiseData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Ability")
	TSoftClassPtr<class UAbilityBase> AbilityObject;

	FMerchandiseAbilityData()
	{
	}

	FMerchandiseAbilityData(FGameplayTag MerchandiseTag, const UAbilityBase* Ability, int32 MerchandisePrice,
	                        int32 MerchandiseCount) : FBaseMerchandiseData(
		MerchandiseTag, MerchandisePrice, MerchandiseCount)
	{
		AbilityObject = Ability->GetClass();
	}
};

USTRUCT(BlueprintType)
struct FMerchandiseItem
{
	GENERATED_BODY()

protected:
	//스폰되어 저장된 아이템 액터 인스턴스. 이 액터로 아이템 효과가 적용됨.
	UPROPERTY()
	TWeakObjectPtr<class AItemActor> SpawndItemActor;

public:
	/**
	 * 아이템 구조체를 구분짓는 유니크 아이디
	 * 이 값으로 서로다른 아이템임을 구분짓습니다.
	 * 
	 * 장비 아이템은 예외없이 모두 이 값이 달라야 합니다.
	 */
	UPROPERTY(VisibleAnywhere)
	FGuid UniqueID;
	UPROPERTY(VisibleAnywhere)
	FMerchandiseItemData MerchandiseData;
	//이 상품을 판매한 NPC 정보
	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<class ANPCBase> MerchantNPC;

	FMerchandiseItem()
	{
	}

	FMerchandiseItem(class ANPCBase* Owner, FGuid Guid, const FMerchandiseItemData& Goods);

	const FItemInformation* GetItemInformation() const;
	FString GetFormattedDescription() const;
	class AItemActor* GetSpawndItemActor() const { return SpawndItemActor.Get(); }
};

USTRUCT(BlueprintType)
struct FMerchandiseAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	FMerchandiseAbilityData MerchandiseAbilityData;
	//이 상품을 판매한 NPC 정보
	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<class ANPCBase> MerchantNPC;

	FMerchandiseAbility()
	{
	}

	FMerchandiseAbility(class ANPCBase* Owner, const FMerchandiseAbilityData& AbilityData);


	const FAbilityInformation* GetAbilityInformation() const;
	FString GetAbilityDescription() const;
};

/**
 * 거래 기능에 필요한 기능들을 모아놓은 컴포넌트입니다.
 */
UCLASS()
class SOULLIKE_API UMerchantComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UMerchantComponent();

	//아이템 판매 리스트
	UPROPERTY(VisibleAnywhere,Transient)
	TMap<FGuid, FMerchandiseItem> MerchandiseItem;
	//어빌리티 판매 리스트
	UPROPERTY(VisibleAnywhere,Transient)
	TMap<FGameplayTag, FMerchandiseAbility> MerchandiseAbility;

	//재구매 리스트
	UPROPERTY(VisibleAnywhere,Transient)
	TMap<FGuid, FMerchandiseItem> RepurchaseMerchandiseItem;
	//어빌리티 재구매 리스트
	UPROPERTY(VisibleAnywhere,Transient)
	TMap<FGameplayTag, FMerchandiseAbility> RepurchaseMerchandiseAbility;


	//상점 위젯
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMerchantWidget> MerchantWidgetObject;
	UPROPERTY(Transient)
	TWeakObjectPtr<class UMerchantWidget> MerchantWidget;

	UPROPERTY(EditAnywhere)
	class UDataTable* MerchandiseItemTable = nullptr;
	UPROPERTY(EditAnywhere)
	class UDataTable* MerchandiseAbilityTable = nullptr;

	virtual void BeginPlay() override;

public:
	//플레이어에게 아이템을 구입 한 후 호출되는 델리게이트
	FOnBuyItemFromPlayer OnBuyItemFromPlayer;
	//플레이어에게 아이템을 판 후 호출되는 델리게이트
	FOnSellItemToPlayer OnSellItemToPlayer;

	//플레이어에게 어빌리티를 구입 한 후 호출되는 델리게이트
	FOnBuyAbilityFromPlayer OnBuyAbilityFromPlayer;
	//플레이어에게 어빌리티를 판 후 호출되는 델리게이트
	FOnSellAbilityToPlayer OnSellAbilityToPlayer;


	void CreateSellItemList(class UDataTable* ItemTable);
	void CreateSellAbilityList(class UDataTable* AbilityTable);
	
	void CreateMerchantWidget(const ABaseCharacter* InteractPlayer);
	//위젯을 생성하고 표시합니다.
	UFUNCTION(BlueprintCallable)
	void ShowMerchantWidget(const ABaseCharacter* InteractPlayer);
	UFUNCTION()
	void OnMerchantWidgetVisibilityChangedEvent(ESlateVisibility InVisibility);

	//판매할 상품을 추가합니다.
	void AddMerchandise(const FMerchandiseItemData& Goods);
	void AddMerchandise(const FMerchandiseAbilityData& Ability);

	const TMap<FGuid, FMerchandiseItem>& GetMerchandiseItem() { return MerchandiseItem; }
	const TMap<FGameplayTag, FMerchandiseAbility>& GetMerchandiseAbility() { return MerchandiseAbility; }

	//플레이어가 판매한 아이템을 재구매 리스트에 추가합니다.
	void AddRepurchaseItem(const FInventoryItem& Item, int32 BuyCount);
	void AddRepurchaseAbility(const FMerchandiseAbility& Ability);


	const TMap<FGuid, FMerchandiseItem>& GetRepurchaseItems() { return RepurchaseMerchandiseItem; }
	const TMap<FGameplayTag, FMerchandiseAbility>& GetRepurchaseAbility() { return RepurchaseMerchandiseAbility; }

	//MerchandiseItem 있는 아이템인지 확인합니다.
	bool IsMerchandiseItem(FGuid ItemUniqueID);
	//RepurchaseMerchandiseItem에 있는 아이템인지 확인합니다.
	bool IsRepurchaseMerchandiseItem(FGuid ItemUniqueID);

	const FMerchandiseItem& GetMerchandiseItem(FGuid ItemUniqueID);
	const FMerchandiseItem& GetRepurchaseMerchandiseItem(FGuid ItemUniqueID);
	bool DecreaseMerchandiseItemCount(FGuid ItemUniqueID, int32 Count);
	bool DecreaseRepurchaseMerchandiseItemCount(FGuid ItemUniqueID, int32 Count);


	//판매 가능한 어빌리티상품인지 확인함.
	bool IsMerchandiseAbility(FGameplayTag AbilityTag);
	bool IsRepurchaseMerchandiseAbility(FGameplayTag AbilityTag);

	const FMerchandiseAbility& GetMerchandiseAbility(FGameplayTag AbilityTag);
	const FMerchandiseAbility& GetRepurchaseMerchandiseAbility(FGameplayTag AbilityTag);
	bool DecreaseMerchandiseAbilityCount(FGameplayTag AbilityTag, int32 Count);
	bool DecreaseRepurchaseMerchandiseAbilityCount(FGameplayTag AbilityTag, int32 Count);
};
