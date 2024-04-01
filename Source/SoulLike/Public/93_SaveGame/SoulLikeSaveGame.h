// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "04_Item/ItemActor.h"
#include "95_OrbCreator/OrbMatrix.h"
#include "GameFramework/SaveGame.h"
#include "SoulLikeSaveGame.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogSave, Log, All);

UENUM(BlueprintType)
enum class EGameLoadType : uint8
{
	NORMAL,
	RESPAWN,
	TELEPORT_TO_LAST_SAVEPOINT,
	TELEPORT_TO_OTHER_BONFIRE
};

USTRUCT(BlueprintType)
struct FActorSave
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AActor> ActorPointer;
	UPROPERTY()
	FName ActorSafeName;
	UPROPERTY()
	TSubclassOf<class AActor> ActorClass;
	UPROPERTY()
	FTransform ActorTransform;


	FActorSave()
	{
	}

	FActorSave(AActor* Ptr, const FName& SafeName, TSubclassOf<AActor> Class, const FTransform& Transform):
		ActorPointer(Ptr), ActorSafeName(SafeName), ActorClass(Class), ActorTransform(Transform)
	{
	}

	bool operator==(const FActorSave& Other) const
	{
		return Other.ActorSafeName.IsEqual(ActorSafeName);
	}

	bool operator==(const FName& OtherSafeName) const
	{
		return OtherSafeName.IsEqual(ActorSafeName);
	}

	bool operator==(const FString& OtherSafeName) const
	{
		return OtherSafeName.Equals(ActorSafeName.ToString());
	}

	FORCEINLINE friend uint32 GetTypeHash(const FActorSave& Other)
	{
		return HashCombine(GetTypeHash(Other.ActorSafeName), GetTypeHash(Other.ActorClass));
	}
};

//액터 기본정보에 추가로 어트리뷰트 및 상태를 저장하기 위한 구조체입니다.
USTRUCT(BlueprintType)
struct FCharacterSave : public FActorSave
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<EAttributeType, FAttribute> Attributes;
	UPROPERTY()
	ECharacterState CharacterState = ECharacterState::NORMAL;


	FCharacterSave()
	{
	}

	FCharacterSave(AActor* Ptr, const FName& SafeName, TSubclassOf<AActor> Class, const FTransform& Transform,
	               const TMap<EAttributeType, FAttribute*>& AttributesInfo, ECharacterState State): FActorSave(
		Ptr, SafeName, Class, Transform)
	{
		CharacterState = State;
		//캐릭터 상태가 사망이라면 굳이 어트리뷰트 정보를 저장할 필요가 없습니다.
		if (CharacterState == ECharacterState::DEAD)
		{
			return;
		}

		for (auto iter : AttributesInfo)
		{
			Attributes.Add(iter.Key, *iter.Value);
		}
	}
};

USTRUCT(BlueprintType)
struct FSavedActorInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TSet<FActorSave> Information;
};

USTRUCT(BlueprintType)
struct FItemSave
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FString ItemClassPath;

	//아이템을 구분짓는 유니크 아이디
	UPROPERTY(BlueprintReadOnly)
	FGuid UniqueID;

	//인벤토리에 저장된 아이템 총 갯수
	UPROPERTY(BlueprintReadOnly)
	int32 ItemCount = 0;

	FItemSave()
	{
	}

	FItemSave(FGuid ID, const FString& Path, int32 Count): ItemClassPath(Path), UniqueID(ID), ItemCount(Count)
	{
	}

	bool operator==(const FGuid& Id) const
	{
		return Id == UniqueID;
	}
};

USTRUCT(BlueprintType)
struct FFieldItemDataLayers
{
	GENERATED_BODY()

	UPROPERTY()
	TSet<FString> ItemSafeNames;
};

USTRUCT(BlueprintType)
struct FFieldItem
{
	GENERATED_BODY()

	//key - layer full path
	//value - item safe name array
	UPROPERTY(BlueprintReadOnly)
	TMap<FString, FFieldItemDataLayers> FiledItemList;

	void Add(const FString& LayerAssetFullPath, const FString& SafeName);
};

USTRUCT(BlueprintType)
struct FLastSavePoint
{
	GENERATED_BODY()

public:
	//이 포인트가 있던 레벨 이름
	UPROPERTY(BlueprintReadOnly)
	FString LevelName;
	//마지막 위치정보
	UPROPERTY(BlueprintReadOnly)
	FTransform SavedTransform;
	UPROPERTY(BlueprintReadOnly)
	FString SavedBonfireName;
	UPROPERTY(BlueprintReadOnly)
	TSet<FName> ActivateLayersPath;
};


USTRUCT()
struct FBonfireInformation : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	FString OwnersSafeName;
	UPROPERTY(EditAnywhere)
	FText LocationName;
	UPROPERTY(EditAnywhere)
	FVector Location = FVector::ZeroVector;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<class UTexture2D> LocationImage;
	UPROPERTY(VisibleAnywhere)
	FString LevelName;
	UPROPERTY(EditAnywhere)
	float SkyTime = 0;
	UPROPERTY(EditAnywhere)
	TArray<TSoftObjectPtr<UDataLayerAsset>> NeedToLoadLayer;
};



USTRUCT(BlueprintType)
struct FDeadState
{
	GENERATED_BODY()

public:
	//사망 위치
	UPROPERTY(BlueprintReadOnly)
	FVector DeadLocation = FVector::ZeroVector;
	//사망 후 떨군 EXP
	UPROPERTY(BlueprintReadOnly)
	float EXP = 0.f;
	//이 값이 참이라면, 다음 로드때 사망위치에 떨군 경험치 액터를 생성합니다.
	UPROPERTY(BlueprintReadOnly)
	bool bShouldCreateSoulTomb = false;
};



UCLASS()
class UGameLoadHandler : public UObject
{
	GENERATED_BODY()

public:
	/*void LoadLevel(UWorld* World, const FString& MapName);*/
	void LoadInventory(class UInventoryComponent* InventoryComponent, const TArray<FItemSave>& SaveInventory);
	void LoadOrbMatrix(UInventoryComponent* InventoryComponent, const FGuid& CoreID,
	                   const FOrbMatrixSave& OrbMatrixSave);
	void LoadFragment(UInventoryComponent* InventoryComponent, const TArray<FItemSave>& SaveFragments);
	void LoadCore(UInventoryComponent* InventoryComponent, const FGuid& UniqueID);
	void LoadWeapon(UInventoryComponent* InventoryComponent, const FGuid& UniqueID);
	void LoadEquippedItem(UInventoryComponent* InventoryComponent, TArray<FGuid> EquipUniqueID,
	                      TMap<FGuid, int32> EquipSlotIndexMap);
	void LoadAttribute(class UAttributeComponent* AttributeComponent,
	                   const TMap<EAttributeType, FAttribute>& AttributesNotIncludeLevelUpPointMap, const
	                   TMap<EAttributeType, FAttribute>& LevelUpPointAttributesMap, bool bIsRespawn);
	void RestoreEquipmentEnhancement(UInventoryComponent* InventoryComponent,
	                                 const TMap<FGuid, FEnhancement>& EquipmentEnhancedMap);
	void RestorePotionEnhancement(UInventoryComponent* InventoryComponent,
	                              const TMap<FGuid, FEnhancement>& PotionEnhancementMap);
	void RestoreQuickSlotState(APlayerCharacter* Player, const TMap<int32, FGuid>& ItemQuickSlotMap,
	                           const TMap<int32, FGameplayTag>& AbilityQuickSlotMap, int32 SelectedConsumeQuickSlot,
	                           int32 SelectedAbilityQuickSlot, const TSet<FGuid>& EquippedQuickSlotConsumeItems,
	                           const TSet<FGameplayTag>& EquippedQuickSlotAbilities);
	void CreateSoulTomb(class APlayerCharacter* Player, TSubclassOf<class ASoulTomb> TombObject,
	                    const FDeadState& DeadState);
	void RestoreBonfire(APlayerCharacter* Player, const TArray<FString>& SavedBonfire);
	void RestoreDataLayer(APlayerCharacter* Player, const TSet<FName>& ActivateLayersPath, UDataTable* LayerTable);
	void RestoreDataLayer(APlayerCharacter* Player, const FName& LayerToLoadRowName, UDataTable* LayerTable);
	void RestoreDataLayer(APlayerCharacter* Player, const TArray<TSoftObjectPtr<UDataLayerAsset>>& Layers);
	void RestoreSky(APlayerCharacter* Player, float CurrentSkyTime);
};

/**
 * 
 */
UCLASS()
class SOULLIKE_API USoulLikeSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	USoulLikeSaveGame();

	//로드시 사용할 타입 정보. 노말이 아니면 다른 행동을 합니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EGameLoadType GameLoadType = EGameLoadType::NORMAL;

	//마지막으로 종료한 레벨 정보
	UPROPERTY(BlueprintReadOnly)
	FString SavedMapName;
	//마지막으로 휴식한 저장포인트
	UPROPERTY(BlueprintReadOnly)
	FLastSavePoint SavedLastSavePoint;
	//위치 및 회전정보
	UPROPERTY(BlueprintReadOnly)
	FTransform Transform;
	//속성정보
	UPROPERTY(BlueprintReadOnly)
	TMap<EAttributeType, FAttribute> AttributesNotIncludeLevelUpPoint;
	UPROPERTY(BlueprintReadOnly)
	TMap<EAttributeType, FAttribute> LevelUpPointAttributes;

	//인벤토리 내부 아이템 정보
	UPROPERTY(BlueprintReadOnly)
	TArray<FItemSave> InventoryItem;
	//파편 정보
	UPROPERTY(BlueprintReadOnly)
	TArray<FItemSave> Fragments;
	//장착중인 아이템 아이디
	UPROPERTY(BlueprintReadOnly)
	TArray<FGuid> EquippedItemID;
	//장착중인 아이템의 슬롯 번호입니다.
	UPROPERTY(BlueprintReadOnly)
	TMap<FGuid, int32> EquippedWidgetIndexMap;

	//퀵슬롯 상태
	//key - 슬롯번호 / value - 아이템 아이디
	UPROPERTY(BlueprintReadOnly)
	TMap<int32, FGuid> ItemQuickSlotMap;
	UPROPERTY(BlueprintReadOnly)
	TMap<int32, FGameplayTag> AbilityQuickSlotMap;

	//마지막으로 선택된 퀵슬롯 번호
	UPROPERTY(BlueprintReadOnly)
	int32 SelectedConsumeQuickSlot;
	UPROPERTY(BlueprintReadOnly)
	int32 SelectedAbilityQuickSlot;

	UPROPERTY(BlueprintReadOnly)
	TSet<FGuid> EquippedQuickSlotItems;
	UPROPERTY(BlueprintReadOnly)
	TSet<FGameplayTag> EquippedQuickSlotAbilities;

	//장비의 강화수치를 저장하는 변수입니다.
	UPROPERTY(BlueprintReadOnly)
	TMap<FGuid, FEnhancement> EquipEnhancementMap;
	//물약의 강화수치를 저장하는 변수입니다.
	UPROPERTY(BlueprintReadOnly)
	TMap<FGuid, FEnhancement> PotionEnhancementMap;


	UPROPERTY(BlueprintReadOnly)
	FGuid WeaponID;
	UPROPERTY(BlueprintReadOnly)
	FGuid CoreID;

	//신력구슬 상태
	UPROPERTY(BlueprintReadOnly)
	FOrbMatrixSave OrbMatrix;

	//레벨에 배치된 아이템들 중, 획득한 아이템 ID
	//key : level Name
	UPROPERTY(BlueprintReadOnly)
	TMap<FString, FSavedActorInfo> PickUppedItems;

	//사망 정보를 저장하는 구조체
	UPROPERTY(BlueprintReadOnly)
	FDeadState DeadState;

	//활성화된 화톳불을 저장하는 배열
	//key - levelName
	//value - 해당 레벨에 저장된 화톳불 목록
	UPROPERTY(BlueprintReadOnly)
	TMap<FString, FSavedActorInfo> ActivateBonfire;

	//다음 로드시 이동해야할 화톳불 정보
	UPROPERTY(VisibleAnywhere)
	FBonfireInformation TeleportBonfireInfo;

	//지금 활성화된 월드 레이어 정보입니다.
	UPROPERTY(BlueprintReadOnly)
	TSet<FName> ActivateLayersPath;
	//현재 하늘의 시간값
	UPROPERTY(BlueprintReadOnly)
	float CurrentSkyTime = 13;

	UPROPERTY(BlueprintReadOnly)
	TSet<FGameplayTag> KilledBossMonstersTag;


	//현재 레벨을 저장합니다.
	void SaveLevelName(APlayerCharacter* Player);
	//트렌스폼을 저장합니다.
	void SaveTransform(APlayerCharacter* Player);
	//어트리뷰트를 저장합니다.
	void SaveAttribute(APlayerCharacter* Player);

	//파편 정보를 저장합니다.
	void SaveFragment(APlayerCharacter* Player);
	//장비 정보를 저장합니다.
	void SaveEquipSlot(APlayerCharacter* Player);
	//장비 강화 정보를 저장합니다.
	void SaveEquipEnhancement(APlayerCharacter* Player);
	//물약의 강화 정보를 저장합니다.
	void SavePotionEnhancement(APlayerCharacter* Player);
	//오브 메트릭스 상태를 저장합니다.
	void SaveOrbMatrix(APlayerCharacter* Player);
	void SaveOrbMatrix(class UOrbMatrix* Matrix);
	/*//사망한 필드 몬스터 정보를 저장하기 위해 사용됩니다. 휴식이후 초기화 해야 합니다.
	void SaveDeadMonster(APlayerCharacter* Player);
	//저장된 사망 몬스터 목록을 날립니다.
	void ClearDeadMonster();*/
	//마지막 건드린 세이브 포인트 지점을 저장합니다.
	void SaveLastSavePoint(const FString& LevelName, const FTransform& LastTransform,
	                      class UBonfireComponent* BonfireComponent);


	//레벨에 배치된 아이템 중, 플레이어가 획득한 아이템임을 저장하기 위해 사용됩니다.
	void SavePlacementItemState(class APlayerCharacter* Player, AItemActor* ItemActor);

	//아이템을 획득하면 호출됩니다.
	void SaveAddedItem(const FInventoryItem& ItemInfo);
	//아이템을 사용하면 호출됩니다.
	void SaveUsedItem(APlayerCharacter* UsedBy, const FInventoryItem& ItemInfo);
	//아이템이 지워지면 호출됩니다.
	void SaveRemovedItem(const FGuid& RemoveItemUniqueID);
	//퀵슬롯 상태를 저장하기 위해 호출됩니다.
	void SaveRegisterItemQuickSlotState(class UInventoryData* Data, int32 Index, const TSet<FGuid>& QuickSlotItems);
	void SaveRegisterAbilityQuickSlotState(UInventoryData* Data, int32 Index,
	                                       const TSet<FGameplayTag>& QuickSlotAbilities);
	void SaveUnRegisterItemQuickSlotState(class UInventoryData* Data, int32 Index);
	void SaveUnRegisterAbilityQuickSlotState(class UInventoryData* Data, int32 Index);
	void SaveAllQuickSlotState(APlayerCharacter* Player);
	//장비가 강화되면 호출됩니다.
	void SaveUpgradeEquipmentItem(const FGuid& ID, const FEnhancement& Enhancement);
	//포션이 강화되면 호출됩니다.
	void SaveUpgradePotionItem(const FGuid& ID, const FEnhancement& PotionEnhancement);
	//장비 해제시 저장 데이터 갱신을 위해 호출됩니다.
	void SaveUnEquipItem(const FInventoryItem& ItemInfo);
	//플레이어가 아이템을 팔았을 때, 저장된 아이템 목록을 갱신하는 함수입니다.
	void UpdateSoldItemInSaveFile(class UInventoryComponent* InventoryComponent, const FGuid& ItemID);
	//플레이어 사망시 정보를 저장합니다.
	void SaveDeadState(APlayerCharacter* Player);
	//저장 내용물을 전부 날립니다.
	void Clear();
	//활성화한 화톳불 정보를 저장합니다.
	void SaveActivateBonfire(class ABonfire* Bonfire);
	//다음 로드시 이동해야 할 화톳불 정보를 저장합니다.
	void SaveTeleportBonfireInfo(const FBonfireInformation& BonfireInfo);
	//인벤토리 상태를 저장합니다.
	void SaveInventory(APlayerCharacter* Player);
	//월드의 레이어 상태를 저장합니다.
	void SaveDataLayer(APlayerCharacter* Player);
	//현재 하늘의 시각을 저장합니다.
	void SaveSkyTime(APlayerCharacter* Player);
	//처치한 우두머리를 기록합니다.
	void SaveKilledBoss(const FGameplayTag& BossTag);

	void SaveSelectedConsumeQuickSlotIndex(int32 SelectedIndex);
	void SaveSelectedAbilityQuickSlotIndex(int32 SelectedIndex);


	UPROPERTY()
	TArray<FName> ReadTutorialActors;
	//이미 읽은 튜토리얼 액터를 저장합니다.
	void SaveReadTutorialActor(class ATutorialActor* TutorialActor);

	UPROPERTY()
	TSet<FGameplayTag> ReadTutorialTags;
	//이미 읽은 튜토리얼 태그를 저장합니다 (액터가 아닙니다)
	void SaveReadTutorial(FGameplayTag TutorialTag);
	bool IsSkippedTutorial(FGameplayTag TutorialTag);


	/*static FString UAIDParser(class UObject* Object);
	static FString UAIDParser(const FString& SafeName);*/
private:
	class APlayerCharacter* GetPlayerCharacter();
};
