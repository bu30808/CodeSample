// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "00_Character/04_NPC/Bonfire.h"
#include "01_GameMode/SoulLikeGameMode.h"
#include "04_Item/ItemActor.h"
#include "04_Item/00_Consume/00_Potion/PotionItemActor.h"
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

	FItemSave(){ }
	FItemSave(FGuid ID, const FString& Path, int32 Count):ItemClassPath(Path),UniqueID(ID),ItemCount(Count){ }
	
	bool operator==(const FGuid& Id) const
	{
		return Id == UniqueID;
	}
	
};

USTRUCT(BlueprintType)
struct FSafeNameSave
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly)
	TArray<FString> SafeNameList;
	
	void Add(const FString& SafeName);
	FString Get(const FString& SafeName);
};

USTRUCT(BlueprintType)
struct FMonsterRespawnInfos
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	TArray<FMonsterRespawnInfo> DeadInfo;
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
};

USTRUCT(BlueprintType)
struct FTeleportBonfireInfo
{
	GENERATED_BODY()
public:
	//이동할 화톳불의 레벨 이름
	UPROPERTY(BlueprintReadOnly)
	FString LevelName;
	//이동할 화톳불의 세이프네임
	UPROPERTY(BlueprintReadOnly)
	FString SafeName;
};


USTRUCT(BlueprintType)
struct FDeadState
{
	GENERATED_BODY()
public:
	//사망 위치
	UPROPERTY(BlueprintReadOnly)
	FVector DeadLocation= FVector::ZeroVector;
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
	void LoadLevel(UWorld* World, const FString& MapName);
	void LoadInventory(class UInventoryComponent* InventoryComponent,const TArray<FItemSave>& SaveInventory);
	void LoadOrbMatrix(UInventoryComponent* InventoryComponent, const FGuid& CoreID, const FOrbMatrixSave& OrbMatrixSave);
	void LoadFragment(UInventoryComponent* InventoryComponent,const TArray<FItemSave>& SaveFragments);
	void LoadCore(UInventoryComponent* InventoryComponent,const FGuid& UniqueID);
	void LoadWeapon(UInventoryComponent* InventoryComponent,const FGuid& UniqueID);
	void LoadEquippedItem(UInventoryComponent* InventoryComponent,TArray<FGuid> EquipUniqueID, TMap<FGuid, int32> EquipSlotIndexMap);
	void LoadAttribute(class UAttributeComponent* AttributeComponent, const TMap<EAttributeType, FAttribute>& AttributesNotIncludeLevelUpPointMap, const
	                   TMap<EAttributeType, FAttribute>& LevelUpPointAttributesMap);
	void RestoreMonsterState(UWorld* World, const TMap<FString, FMonsterRespawnInfos>& DeadMonsters);
	void RestoreFieldItemState(APlayerCharacter* Player, const TMap<FString, FSafeNameSave>& PickuppedItems);
	void RestoreEquipmentEnhancement(UInventoryComponent* InventoryComponent, const TMap<FGuid, FEnhancement>& EquipmentEnhancedMap);
	void RestorePotionEnhancement(UInventoryComponent* InventoryComponent, const TMap<FGuid, FEnhancement>& PotionEnhancementMap);
	void RestoreQuickSlotState(APlayerCharacter* Player,const TMap<int32,FGuid>& ItemQuickSlotMap,const TMap<int32,FGameplayTag>& AbilityQuickSlotMap);
	void CreateSoulTomb(class APlayerCharacter* Player, const FDeadState& DeadState);
	void RestoreBonfire(APlayerCharacter* Player, const TArray<FString>& SavedBonfire);
	void TeleportToBonfire(APlayerCharacter* Player, const FBonfireInformation& BonfireInfo);
	void RestoreTutorial(APlayerCharacter* Player, const TArray<FString>& Tutorials);
};

/**
 * 
 */
UCLASS()
class SOULLIKE_API USoulLikeSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	//로드시 사용할 타입 정보. 노말이 아니면 다른 행동을 합니다.
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
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
	TMap<EAttributeType,FAttribute> AttributesNotIncludeLevelUpPoint;
	UPROPERTY(BlueprintReadOnly)
	TMap<EAttributeType,FAttribute> LevelUpPointAttributes;
	
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
	TMap<FGuid,int32> EquippedWidgetIndexMap;

	//퀵슬롯 상태
	UPROPERTY(BlueprintReadOnly)
	TMap<int32,FGuid> ItemQuickSlotMap;
	UPROPERTY(BlueprintReadOnly)
	TMap<int32,FGameplayTag> AbilityQuickSlotMap;
	
	//장비의 강화수치를 저장하는 변수입니다.
	UPROPERTY(BlueprintReadOnly)
	TMap<FGuid,FEnhancement> EquipEnhancementMap;
	//물약의 강화수치를 저장하는 변수입니다.
	UPROPERTY(BlueprintReadOnly)
	TMap<FGuid,FEnhancement> PotionEnhancementMap;
	
	
	UPROPERTY(BlueprintReadOnly)
	FGuid WeaponID;
	UPROPERTY(BlueprintReadOnly)
	FGuid CoreID;
	
	//신력구슬 상태
	UPROPERTY(BlueprintReadOnly)
	FOrbMatrixSave OrbMatrix;

	/*//레벨에서 사망한 몬스터 ID
	//Key - 레벨이름
	//Value - 사망한 몬스터 ID
	UPROPERTY(BlueprintReadOnly)
	TMap<FString, FMonsterRespawnInfos> DeadMonsters;*/

	//레벨에 배치된 아이템들 중, 획득한 아이템 ID
	UPROPERTY(BlueprintReadOnly)
	TMap<FString,FSafeNameSave> PickUppedItems;

	//사망 정보를 저장하는 구조체
	UPROPERTY(BlueprintReadOnly)
	FDeadState DeadState;

	//활성화된 화톳불을 저장하는 배열
	UPROPERTY(BlueprintReadOnly)
	TArray<FString> ActivateBonfire;

	//다음 로드시 이동해야할 화톳불 정보
	UPROPERTY(VisibleAnywhere)
	FBonfireInformation TeleportBonfireInfo;

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
	void SaveLastSavePoint(const FString& LevelName, const FTransform& LastTransform, UBonfireComponent* BonfireComponent);
	

	//레벨에 배치된 아이템 중, 플레이어가 획득한 아이템임을 저장하기 위해 사용됩니다.
	void SavePlacementItemState(const FString& LevelName, const FString& SafeName);
	//아이템을 획득하면 호출됩니다.
	void SaveAddedItem(const FInventoryItem& ItemInfo);
	//아이템을 사용하면 호출됩니다.
	void SaveUsedItem(const FInventoryItem& ItemInfo);
	//아이템이 지워지면 호출됩니다.
	void SaveRemovedItem(const FGuid& RemoveItemUniqueID);
	//퀵슬롯 상태를 저장하기 위해 호출됩니다.
	void SaveRegisterQuickSlotState(class UInventoryData* Data,int32 Index);
	void SaveUnRegisterQuickSlotState(class UInventoryData* Data, int32 Index);
	void SaveAllQuickSlotState(APlayerCharacter* Player);
	//장비가 강화되면 호출됩니다.
	void SaveUpgradeEquipmentItem(const FGuid& ID, const FEnhancement& Enhancement);
	//포션이 강화되면 호출됩니다.
	void SaveUpgradePotionItem(const FGuid& ID, const FEnhancement& PotionEnhancement);
	//장비 해제시 저장 데이터 갱신을 위해 호출됩니다.
	void SaveUnEquipItem(const FInventoryItem& ItemInfo);
	//플레이어가 아이템을 팔았을 때, 저장된 아이템 목록을 갱신하는 함수입니다.
	void UpdateSoldItemInSaveFile(class UInventoryComponent* InventoryComponent,const FGuid& ItemID);
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


	UPROPERTY()
	TArray<FString> ReadTutorialActors;
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
