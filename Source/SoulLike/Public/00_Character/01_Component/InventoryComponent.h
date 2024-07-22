// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "04_Item/05_Ability/AbilityItemActor.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Display, All);

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

protected:
	//스폰되어 저장된 아이템 액터 인스턴스. 이 액터로 아이템 효과가 적용됨.
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AItemActor> SpawndItemActor;
	UPROPERTY(VisibleAnywhere)
	TSubclassOf<AItemActor> ItemActorObjectClass;
	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<AActor> OwnerActor;

public:
	/**
	 * 아이템 구조체를 구분짓는 유니크 아이디
	 * 이 값으로 서로다른 아이템임을 구분짓습니다.
	 * 
	 * 장비 아이템은 예외없이 모두 이 값이 달라야 합니다.
	 */
	UPROPERTY(VisibleAnywhere)
	FGuid UniqueID;

	//인벤토리에 저장된 아이템 총 갯수
	UPROPERTY(VisibleAnywhere)
	int32 ItemCount = 0;

	FInventoryItem()
	{
	}

	AItemActor* SetItemActor(bool bLoad);
	FInventoryItem(class AActor* Owner, class AItemActor* ItemActor, int32 Count, FGuid NewID);
	FInventoryItem(class AActor* Owner, TSubclassOf<class AItemActor> ItemActorObject, int32 Count, FGuid NewID,
	               bool bLoad = false);


	const FItemInformation* GetItemInformation() const;
	FText GetFormattedDescription() const;
	bool Use(class AActor* Owner) const;
	void UnEquip(AActor* Owner) const;

	//더 누적 가능한지 확인합니다.
	bool CanStack(int32 AddCount) const;

	class AItemActor* GetSpawndItemActor() const { return SpawndItemActor.Get(); }

	bool operator==(const FInventoryItem& Other) const
	{
		return UniqueID == Other.UniqueID;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryWidgetUpdate, const FGuid&, ItemUniqueID, const int32&,
                                             NewCount);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemQuickSlotUpdate, const FGuid&, ItemUniqueID, const int32&,
                                             NewCount);

//누가 획득했는가 / 획득한 아이템 / 획득된 아이템 액터
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAddItem, class ABaseCharacter*, GetBy, const FInventoryItem&,
                                               ItemInfo, class AItemActor*, ItemActor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAddAbilityItem, class ABaseCharacter*, GetBy, const FAbilityInformation&,
											   AbilityItemInfo, class AItemActor*, ItemActor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUseItem, class ABaseCharacter*, UsedBy, const FInventoryItem&, ItemInfo)
;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnEquipItem, class ABaseCharacter*, UsedBy, const FInventoryItem&,
                                             ItemInfo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSaveFieldItemState, class ABaseCharacter*, GetBy, class AItemActor*,
                                             ItemActor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRemoveItem, class ABaseCharacter*, UsedBy, const FGuid&,
                                             RemoveItemUniqueID);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetItem, TSoftObjectPtr<class UTexture2D>, Image, FText, Name,
                                               int32, Count);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateItemQuickSlot, const class UItemData*,Data,int32,SlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRemoveItemQuickSlot, const class UItemData*,Data,int32,SlotIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChangeItemQuickSlot, const FInventoryItem&,Item,int32,SlotIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnUpdateMainAbilityQuickSlot, const FGameplayTag&, Tag, bool, bRemove,
                                               int32, SlotIndex);


/*
 *	아이템을 관리하는 컴포넌트입니다.
 *	아이템 획득 및 사용처리를 합니다.
 *	아직 삭제기능은 없습니다.
 *
 *	모든 아이템은 게임플레이 태그로 종류를 구분 가능합니다.
 *	각각의 아이템은 모두 서로다른 UniqueID를 갖습니다.
 *	유니크 아이디로 같은 아이템이지만 다른 강화수치를 가지는 경우 같은 특수한 상황을 처리할 수 있습니다.
 *
 *	OrbFragment의 경우, 랜덤하게 생성되기 때문에 게임플레이 태그로 유일하게 구분 불가능하므로,
 *	OrbFragmentHandler에게 처리를 양도합니다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULLIKE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class UGameLoadHandler;

public:
	// Sets default values for this component's properties
	UInventoryComponent();

private:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void PostInitProperties() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TMap<FGuid, FInventoryItem> Inventory;
	//필드에서 아이템 형식으로 획득한 어빌리티를 저장하는 맵
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TMap<FGameplayTag,FAbilityInformation> AbilityInventory;
	//EItemType::EQUIPMENT인 모든 장착중인 장비를 저장하는 배열입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TArray<FGuid> EquippedEquipments;

	//장착중인 무기의 유니크 아이디입니다.
	UPROPERTY()
	FGuid EquippedWeapon;

	//장착중인 코어의 유니크 아이디입니다.
	UPROPERTY()
	FGuid EquippedCore;


	/***********************구슬 파편 저장*******************************/
	UPROPERTY(VisibleAnywhere)
	TMap<FGuid, FInventoryItem> Fragments;

public:
	void AddFragment(FGuid Guid, const FInventoryItem& NewFragment, const FItemInformation* ItemInfo);
	const FInventoryItem& GetFragment(FGuid UniqueID) { return Fragments[UniqueID]; }
	bool IsFragmentContains(const FGuid& UniqueID) const;
	bool IsItemContains(const FGuid& UniqueID) const;

	const TArray<FGuid>& GetEquippedItemsID() { return EquippedEquipments; }
	const FInventoryItem& GetEquippedWeapon();
	const FInventoryItem& GetEquippedCore();

	void SetEquippedCoreID(const FGuid& UniqueID) { EquippedCore = UniqueID; }
	const FGuid& GetEquippedCoreID() const { return EquippedCore; }
	void SetEquippedWeaponID(const FGuid& UniqueID) { EquippedWeapon = UniqueID; }
	const FGuid& GetEquippedWeaponID() const { return EquippedWeapon; }


	//아이템이 추가되었을 때 호출되는 이벤트입니다.
	UPROPERTY(BlueprintAssignable)
	FOnAddItem OnAddItem;
	UPROPERTY(BlueprintAssignable)
	FOnAddAbilityItem OnAddAbilityItem;

	
	//필드 아이템 상황을 저장하는 이벤트입니다.
	UPROPERTY(BlueprintAssignable)
	FOnSaveFieldItemState OnSaveFieldItemState;
	//장비 아이템 사용 후 호출되는 이벤트입니다.
	UPROPERTY(BlueprintAssignable)
	FOnUseItem OnUseItem;
	//인벤토리 UI를 업데이트 하는 이벤트입니다.
	UPROPERTY(BlueprintAssignable)
	FOnInventoryWidgetUpdate OnInventoryWidgetUpdate;
	//소비전용 퀵슬롯 UI를 업데이트 하는 이벤트입니다. 아이템이 사용되면 호출됩니다.
	UPROPERTY(BlueprintAssignable)
	FOnItemQuickSlotUpdate OnItemQuickSlotUpdate;
	//인벤토리에서 아이템이 제거될떄 호출되는 이벤트입니다.
	UPROPERTY(BlueprintAssignable)
	FOnRemoveItem OnRemoveItem;
	//아이템을 획득했을때, 획득한 갯수를 이용해 호출되는 델리게이트입니다.
	UPROPERTY(BlueprintAssignable)
	FOnGetItem OnGetItem;
	//장비를 해제했을 때 호출되는 이벤트입니다.
	UPROPERTY(BlueprintAssignable)
	FOnUnEquipItem OnUnEquipItem;

	const TMap<FGuid, FInventoryItem>& GetInventory() { return Inventory; }
	const TMap<FGuid, FInventoryItem>& GetFragments() const { return Fragments; }

	/**
	 * 아이템을 추가합니다.
	 * 아이템을 추가할 때 마다, 저장합니다.
	 * @param ItemActor 아이템 정보를 가지고 있는 아이템 액터
	 * @param bShowPickUpWidget 아이템 획득을 화면에 표시할 것인가?
	 * @return 추가한 아이템의 유니크 아이디, 어빌리티가 추가된 경우는 빈 Guid가 리턴됩니다.
	 */
	UFUNCTION(BlueprintCallable)
	FGuid AddItem(AItemActor* ItemActor, bool bShowPickUpWidget = true);

	/**
	 * 필요에 따라 아이템을 직접 스폰한 뒤, 인벤토리에 추가합니다.
	 * @param ItemToSpawn 
	 * @return 추가한 아이템의 유니크 아이디
	 */
	UFUNCTION(BlueprintCallable)
	FGuid SpawnAndAddItem(TSubclassOf<AItemActor> ItemToSpawn, const int32& ItemCount = 1);

	/**
	 * 아이템을 사용합니다.
	 * @param ItemUniqueID 사용할 아이템의 유니크 아이디 
	 */
	UFUNCTION(BlueprintCallable)
	void UseItem(FGuid ItemUniqueID);
	UFUNCTION(BlueprintCallable)
	void RemoveItem(FGuid ItemUniqueID);

	UFUNCTION(BlueprintCallable)
	bool DecreaseItemCount(FGuid ItemUniqueID, int32 Count = 1);

	//이 아이디를 가진 장비를 찾아서 장착해제합니다.
	UFUNCTION(BlueprintCallable)
	void UnEquip(const FGuid& ItemUniqueID);

	const FInventoryItem& GetInventoryItem(const FGuid& ItemUniqueID);

	TArray<class AItemActor*> GetEquipmentItems();


	//해당 아이디의 "장비"아이템이 장착중인지 확인합니다.
	UFUNCTION(BlueprintPure, BlueprintCallable)
	bool IsEquippedEquipment(const FGuid& ItemUniqueID);


	/**
	 * 해당 태그를 가진 아이템이 하나라도 있는지 확인합니다.
	 * @param ItemTag 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName="HasItem")
	bool K2_HasItem(const FGameplayTag& ItemTag);
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName="HasItemByID")
	bool K2_HasItemByID(const FGuid& UniqueID) const;

	/**
	 * 해당 태그를 가진 아이템중 가장 첫번째로 발견한 아이템을 가져옵니다.
	 * @param ItemTag 
	 * @return 
	 */
	const FInventoryItem* GetItemByTag(const FGameplayTag& ItemTag);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	class AItemActor* GetItemActorByTag(const FGameplayTag& ItemTag);

protected:
	/**
	 * 해당 게임플레이 태그와 일치하는 아이템이 있는지 확인합니다.
	 * @param ItemTag 
	 * @return 
	 */
	FInventoryItem* HasItem(FGameplayTag ItemTag);

	const FGuid AddNewItemToInventory(const FItemInformation* ItemInfo, AItemActor* ItemActor, int32 ItemCount);

	/***********************퀵슬롯 관리*******************************/

	//지금 표시중인 인덱스
	UPROPERTY(VisibleAnywhere, meta=(ClampMin = -1, ClampMax = 9))
	int32 CurItemQuickSlotIndex = -1;


	//퀵슬롯에 저장된 아이템 유니크 아이디입니다.
	UPROPERTY(VisibleAnywhere)
	TArray<FGuid> ItemQuickSlotUniqueIDs = {
		FGuid(), FGuid(), FGuid(), FGuid(), FGuid(), FGuid(), FGuid(), FGuid(), FGuid(), FGuid()
	};

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnUpdateItemQuickSlot OnFirstUpdateMainItemQuickSlot;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnChangeItemQuickSlot OnChangeItemQuickSlot;
	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FOnRemoveItemQuickSlot OnRemoveItemQuickSlot;

	
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnUpdateItemQuickSlot OnAddItemQuickSlot;

	void AddQuickSlotItem(UItemData* Data, int32 Index);
	void RemoveQuickSlotItem(UItemData* Data, int32 Index);
	void NextConsumeQuickSlot();
	bool IsRegistered(const FGuid& ID) const;
	void UseConsumeQuickSlot();


	
	//아이템 버튼이 아이템리스트에서 생성되면 호출됩니다.
	UFUNCTION()
	void OnItemButtonWidgetGeneratedEvent(UUserWidget* UserWidget);

	//태그에 해당하는 열쇠를 가지고 있는가 확인합니다. hasitem함수와 같은 기능입니다.
	UFUNCTION(BlueprintCallable,BlueprintPure)
	bool HasKey(FGameplayTag KeyTag);
};
