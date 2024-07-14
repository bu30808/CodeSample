// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "00_Character/04_NPC/Chest.h"
#include "04_Item/ItemActor.h"
#include "95_OrbCreator/OrbMatrix.h"
#include "Engine/DataTable.h"
#include "GameFramework/SaveGame.h"
#include "SoulLikeSaveGame.generated.h"


enum class EGraphicQuality : uint8;
enum class EDataLayerRuntimeState : uint8;
DECLARE_LOG_CATEGORY_EXTERN(LogSave, Log, All);

UENUM(BlueprintType)
enum class EGameLoadType : uint8
{
	//처음 시작한 경우 보통 사용됨.
	NORMAL,
	//죽었을 때
	DEAD_RESPAWN,
	//다른 화톳불로 이동하는 경우
	TELEPORT_TO_OTHER_BONFIRE,
	//게임 종료후 다시 시작하는 경우.
	RESTART
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

USTRUCT()
struct FBonfireInformation : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	FString OwnersSafeName;
	UPROPERTY(EditAnywhere,Category=Location)
	FText LocationName;
	UPROPERTY(EditAnywhere,Category=Location)
	FVector Location = FVector::ZeroVector;
	UPROPERTY(EditAnywhere,Category=Location)
	TSoftObjectPtr<class UTexture2D> LocationImage;
	UPROPERTY(VisibleAnywhere)
	FString LevelName;
	UPROPERTY(EditAnywhere,Category=Sky)
	float SkyTime = 0;
	UPROPERTY(EditAnywhere,Category=Sky)
	class UDataAsset* Weather;
	//특별히 활성화 해야 하는 레이어
	//key - 레이어 , value - 재귀 여부
	UPROPERTY(EditAnywhere,Category=Layer)
	TMap<TSoftObjectPtr<UDataLayerAsset>,bool> NeedToActivateLayer;
	//특별히 로드만 해도 되는 레이어(화면에 보이지 않음)
	UPROPERTY(EditAnywhere,Category=Layer)
	TMap<TSoftObjectPtr<UDataLayerAsset>,bool> NeedToLoadLayer;
	//이 값이 참이어야 레이어 언로드 기능을 사용합니다.
	UPROPERTY(EditAnywhere,Category=Layer)
	bool bUseLayerUnload = false;
	//이 값이 참이면 언로드할 레이어를 직접 선택합니다. 거짓이면 "활성화, 로드됨" 상태 이외의 모든 레이어를 언로드합니다.
	UPROPERTY(EditAnywhere,meta=(EditCondition = "bUseLayerUnload"),Category=Layer)
	bool bUnloadLayerManually = false;
	UPROPERTY(EditAnywhere,meta=(EditCondition = "bUnloadLayerManually"),Category=Layer)
	bool bUnloadRecursive = false;
	//특별히 언로드해야 할 레이어
	UPROPERTY(EditAnywhere,meta=(EditCondition = "bUseLayerUnload && bUnloadLayerManually"),Category=Layer)
	TMap<TSoftObjectPtr<UDataLayerAsset>,bool> NeedToUnloadLayer;
	
};

/*USTRUCT(BlueprintType)
struct FLastSavePoint
{
	GENERATED_BODY()

public:
	//이 포인트가 있던 레벨 이름
	UPROPERTY(BlueprintReadOnly)
	FString LevelName;
	//마지막 위치정보
	UPROPERTY(BlueprintReadOnly)
	FVector SavedLocation;
	UPROPERTY(BlueprintReadOnly)
	FString SavedBonfireSafeName;
	UPROPERTY(BlueprintReadOnly)
	float SkyTime;
	UPROPERTY(BlueprintReadOnly)
	class UDataAsset* Weather;

	//화톳불 이동 후, 이동한 화톳불에 대한 정보
	FLastSavePoint& operator=(const FBonfireInformation& TeleportedBonfire);
};*/

USTRUCT(BlueprintType)
struct FSoulTombCreateContext
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

USTRUCT(BlueprintType)
struct FPlayerInventoryData
{
	GENERATED_BODY()
public:
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
};

USTRUCT(BlueprintType)
struct FPlayerData
{
	GENERATED_BODY()
public:
	//위치 및 회전정보
	UPROPERTY(BlueprintReadOnly)
	FTransform Transform;
	//속성정보
	UPROPERTY(BlueprintReadOnly)
	TMap<EAttributeType, FAttribute> AttributesNotIncludeLevelUpPoint;
	UPROPERTY(BlueprintReadOnly)
	TMap<EAttributeType, FAttribute> LevelUpPointAttributes;

	UPROPERTY(BlueprintReadOnly)
	FPlayerInventoryData InventoryData;

	//사망 정보를 저장하는 구조체
	UPROPERTY(BlueprintReadOnly)
	FSoulTombCreateContext DeadState;
};

USTRUCT(BlueprintType)
struct FWorldData
{
	GENERATED_BODY()
public:
	//레벨에 배치된 아이템들 중, 획득한 아이템 ID
	//key : level Name
	UPROPERTY(BlueprintReadOnly)
	TMap<FString, FSavedActorInfo> PickUppedItems;
	
	//활성화된 화톳불을 저장하는 배열
	//key - levelName
	//value - 해당 레벨에 저장된 화톳불 목록
	UPROPERTY(BlueprintReadOnly)
	TMap<FString, FSavedActorInfo> ActivateBonfire;

	//이미 열린 상자를 저장하는 맵
	//key - level Name
	//value - 해당 레벨에 저장된 상자 목록
	UPROPERTY(BlueprintReadOnly)
	TMap<FString,FSavedActorInfo> OpenedChests;
	
	//이미 열린 상자에서 획득된 아이템 
	//key - level Name
	//value - 해당 레벨에 저장된 상자 목록
	UPROPERTY(BlueprintReadOnly)
	TMap<FString,FSavedActorInfo> EarnedChestItems;
	
	//다음 로드시 이동해야할 화톳불 정보
	UPROPERTY(VisibleAnywhere)
	FBonfireInformation TeleportBonfireInfo;
	
	//현 레이어 상태 정보
	//key - layer path : ex) /Game/Datalayer/MyLayer.MyLayer
	//Value - 레이어 상태
	UPROPERTY(BlueprintReadOnly)
	TMap<FName,EDataLayerRuntimeState> LastSavedLayerState;
	//현재 하늘의 시간값
	UPROPERTY(BlueprintReadOnly)
	float CurrentSkyTime = 13;
	UPROPERTY(BlueprintReadOnly)
	class UDataAsset* CurrentWeather;

	UPROPERTY(BlueprintReadOnly)
	TSet<FGameplayTag> KilledBossMonstersTag;

	//종료시 마지막으로 기억된 몬스터 상태정보입니다.
	//이 값으로 재시작시 몬스터 상태정보를 복구합니다.
	UPROPERTY()
	TMap<FName, FCharacterSave> LastMonsterState;
};

UCLASS()
class UGameLoadHandler : public UObject
{
	GENERATED_BODY()

public:
	/*void LoadLevel(UWorld* World, const FString& MapName);*/

	void LoadOrbMatrix(UInventoryComponent* InventoryComponent, const FGuid& CoreID,
	                   const FOrbMatrixSave& OrbMatrixSave);
	void LoadFragment(UInventoryComponent* InventoryComponent, const TArray<FItemSave>& SaveFragments);
	void LoadCore(UInventoryComponent* InventoryComponent, const FGuid& UniqueID);
	void LoadWeapon(UInventoryComponent* InventoryComponent, const FGuid& UniqueID);
	void LoadEquippedItem(UInventoryComponent* InventoryComponent, TArray<FGuid> EquipUniqueID,
	                      TMap<FGuid, int32> EquipSlotIndexMap);
	
	void RestoreEquipmentEnhancement(UInventoryComponent* InventoryComponent,
	                                 const TMap<FGuid, FEnhancement>& EquipmentEnhancedMap);
	void RestorePotionEnhancement(UInventoryComponent* InventoryComponent,
	                              const TMap<FGuid, FEnhancement>& PotionEnhancementMap);
	class ASoulTomb* CreateSoulTomb(class APlayerCharacter* Player, const TSubclassOf<class ASoulTomb>& TombObject,
	                            const FSoulTombCreateContext& DeadState);
	void RestoreBonfire(APlayerCharacter* Player, const TArray<FString>& SavedBonfire);
	void RestoreDataLayer(APlayerCharacter* Player, const TMap<FName, EDataLayerRuntimeState>& LayerStateMap, UDataTable* LayerTable);
	void RestoreDataLayer(APlayerCharacter* Player, const FName& LayerToLoadRowName, UDataTable* LayerTable);
	void RestoreDataLayer(TObjectPtr<APlayerCharacter> Player, const FBonfireInformation& BonfireInformation, UDataTable* LayerTable);
	
	void LoadInventory(UInventoryComponent* InventoryComponent, const FPlayerInventoryData& InventoryData);
	void LoadAttribute(UAttributeComponent* AttributeComponent, const FPlayerData& PlayerData, bool bIsRespawn);
	void RestoreQuickSlotState(const TObjectPtr<APlayerCharacter>& Player, const FPlayerInventoryData& InventoryData);
	void RestoreSky(const TObjectPtr<APlayerCharacter>& Player, const FWorldData& WorldData);
	void RestoreSky(const TObjectPtr<APlayerCharacter>& Player, const FBonfireInformation& BonfireInformation);
	void RestoreMonsterState(const TObjectPtr<APlayerCharacter>& Player, const TMap<FName, FCharacterSave>& LastMonsterState);
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
	FBonfireInformation LastCheckpoint;

	//플레이어 정보
	UPROPERTY(BlueprintReadOnly)
	FPlayerData PlayerData;

	//세계정보
	UPROPERTY(BlueprintReadOnly)
	FWorldData WorldData;

	//게임 종료시 저장합니다.
	void SaveWhenEndPlay(APlayerCharacter* Player, UDataTable* LayerTable);
	//현재 레벨을 저장합니다.
	void SaveLevelName(APlayerCharacter* Player);
	//트렌스폼을 저장합니다.
	void SaveTransform(APlayerCharacter* Player);
	//어트리뷰트를 저장합니다.
	void SaveAttribute(APlayerCharacter* Player);
	//경험치 정보만 저장합니다.
	void SaveExp(const FAttribute& ExpAttribute);
	//파편 정보를 저장합니다.
	//void SaveFragment(APlayerCharacter* Player);
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
	void SaveLastCheckpoint(const FBonfireInformation& BonfireInformation);

	//레벨에 배치된 아이템 중, 플레이어가 획득한 아이템임을 저장하기 위해 사용됩니다.
	void SavePlacementItemState(class APlayerCharacter* Player, AItemActor* ItemActor);

	//아이템을 획득하면 호출됩니다.
	void SaveAddedItem(const FInventoryItem& ItemInfo);
	//아이템을 사용하면 호출됩니다.
	void SaveUsedItem(APlayerCharacter* UsedBy, const FInventoryItem& ItemInfo);
	//아이템이 지워지면 호출됩니다.
	void SaveRemovedItem(const FGuid& RemoveItemUniqueID);
	//퀵슬롯 상태를 저장하기 위해 호출됩니다.
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
	void SaveDataLayer(APlayerCharacter* Player, UDataTable* LayerTable);
	//현재 하늘의 시각을 저장합니다.
	void SaveSky(APlayerCharacter* Player);
	//처치한 우두머리를 기록합니다.
	void SaveKilledBoss(const FGameplayTag& BossTag);

	void SaveAddItemQuickSlot(const class UItemData* Data, int32 SelectedIndex);
	void SaveRemovedItemQuickSlot(const UItemData* Data, int32 RemovedIndex);

	//상자의 열림 상태 및 내부 아이템 획득 상황을 저장합니다.
	void SaveChest(class AChest* Chest, bool bEaredChestItem);

	


	UPROPERTY()
	TMap<FGameplayTag, FNPCState> NPCState;
	//NPC관련 정보를 저장합니다.
	void SaveNPC(class ANPCBase* NPC, bool bIsDestroyed);
	void LoadNPC(class ANPCBase* NPC);


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


UENUM(BlueprintType)
enum class ELanguageType:uint8
{
	EN,
	KOR
};

USTRUCT(BlueprintType)
struct FGameOption
{
	GENERATED_BODY()
public:
	UPROPERTY()
	ELanguageType Language;

	UPROPERTY()
	float BGMVolume = 1;
	UPROPERTY()
	float SFXVolume = 1;
	UPROPERTY()
	float MouseSensitivity = 1.f;
	UPROPERTY()
	int32 TexturePoolSize = 500;
};

UCLASS()
class SOULLIKE_API USoulLikeOptionSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FGameOption GameOption;
};