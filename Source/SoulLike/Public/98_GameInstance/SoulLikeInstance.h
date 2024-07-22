// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "03_Widget/01_Menu/00_Inventory/ItemListWidget.h"
#include "92_Tools/TutorialActor.h"
#include "93_SaveGame/SoulLikeSaveGame.h"
#include "Engine/GameInstance.h"
#include "SoulLikeInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInstance, Log, All);

class AItemActor;
class ABaseMonster;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishLoadGame);
DECLARE_DYNAMIC_DELEGATE(FOnDeletedSaveFile);

USTRUCT(BlueprintType)
struct FDataLayerTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	class UDataLayerAsset* DataLayerAsset;
};

/**
 * 
 */
UCLASS()
class SOULLIKE_API USoulLikeInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	USoulLikeInstance();

private:

	UPROPERTY()
	TSubclassOf<class ASoulTomb> SoulTombClass;
	
	UPROPERTY()
	TSubclassOf<class AWorldStreamingSourceActor> WorldStreamingSourceActorClass;
	UPROPERTY()
	TObjectPtr<APlayerCharacter> CurrentPlayer;
	
	//dataLayerSubsystem->GetDataLayerInstanceFromAssetName가 패키징하면 레이어를 못 찾는 문제가 있어서, 데이터테이블에 우겨넣고 씁니다.
	UPROPERTY()
	class UDataTable* DataLayerTable;

	UPROPERTY(Transient)
	TObjectPtr<class ASoulTomb> CreatedTomb;

protected:
	//볼륨조절을 위한 클래스 포인터
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	class USoundClass* BGMClass;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	class USoundClass* SFXClass;
public:
	UFUNCTION(BlueprintCallable,BlueprintPure)
	float GetBGMVolume();
	UFUNCTION(BlueprintCallable,BlueprintPure)
	float GetSFXVolume();

	UFUNCTION(BlueprintCallable)
	void SetBGMVolume(float vol);
	UFUNCTION(BlueprintCallable)
	void SetSFXVolume(float vol);
public:

	void SetPlayer(APlayerCharacter* PlayerCharacter){CurrentPlayer = PlayerCharacter;}

	UPROPERTY()
	int32 SaveIndex = 0;

	//이 변수를 직접 사용하지 마세요.
	//GetSaveGameInstance함수를 통해서 불러오세요.
	UPROPERTY()
	class USoulLikeSaveGame* SaveGameInstance;
	//이 변수를 직접 사용하지 마세요.
	//GetSaveGameInstance함수를 통해서 불러오세요.
	UPROPERTY()
	class USoulLikeOptionSaveGame* OptionSaveGameInstance;

	//이 값이 참인동안에는 저장이 되지 않습니다. 로드중에 참이 됩니다.
	UPROPERTY(Transient)
	bool bBlockSave = false;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> LoadingWidgetClass;

	//지금 세이브 파일을 로드중인 상태인지 확인합니다.
	UFUNCTION(BlueprintCallable,BlueprintPure)
	bool IsNowLoading(){return bBlockSave;}
	
	virtual void Init() override;
	//로딩스크린 생성
	void PreLoadMapEvent(const FString& LevelName);
	//월드 로드 이후 호출됨.
	void OnPostLoadMapWithWorldEvent(UWorld* World);

	bool IsUseGameSave() const;
	bool IsRespawn();

	UFUNCTION(BlueprintCallable)
	void DeleteSaveFile(const FOnDeletedSaveFile& OnDeletedSaveFile,FString SlotName = "SaveFile");

	UPROPERTY(BlueprintCallable)
	FOnFinishLoadGame OnFinishLoadGame;

	UFUNCTION(BlueprintCallable)
	void LoadGame();
	
	//필드 아이템 상황을 복구합니다.
	UFUNCTION()
	void RestoreFieldItemState();
	//공통적으로 로드되는 부분
	UFUNCTION()
	void LoadCommon();

	//저장된 레벨과 현 레벨에 다르면 레벨을 새로 엽니다.
	void LoadLevel(const FString& LevelName);

	//EGameLoadType::NORMAL일때 사용됩니다.
	void NormalLoad();
	void RestoreBeforeDead();

	//EGameLoadType::RESPAWN 이거나 EGameLoadType::TELEPORT_TO_LAST_SAVEPOINT일때 사용됩니다.
	UFUNCTION()
	void DeadRespawn();
	//게임을 종료했다 다시 시작하면 호출됩니다.
	//EGameLoadType::RESTART
	UFUNCTION()
	void Restart();

	
	//EGameLoadType::TELEPORT_TO_OTHER_BONFIRE일때 사용됩니다.
	void TeleportToOtherBonfireLoad();

	void LoadBonfireSetting(const FBonfireInformation& BonfireInformation);
	
	//화톳불이 속한 레이어를 복구합니다.
	void LoadBonfireLayer(const FBonfireInformation& BonfireInformation);

	//레이어를 복구합니다.
	void LoadLayer();
	void LoadLayers(const TArray<TSoftObjectPtr<class UDataLayerAsset>>& Layers) const;
	
	//화톳불에 저장된 시간으로 되돌립니다.
	UFUNCTION()
	void LoadSkyFromBonfire(const FBonfireInformation& BonfireInformation) const;

	void LoadSky();
	void LoadTransform();
	void LoadLastMonsterState();

	/*void LoadPlayerLocation(class APlayerCharacter* Player, AWorldStreamingSourceActor* StreamingActor);*/

	//스트리밍 액터를 스폰하여 가져옵니다.
	class AWorldStreamingSourceActor* CreateWorldStreamingActor();

	void LoadInventory();
	void LoadAttribute(bool bIsRespawn);
	void LoadQuickSlotState();
	void CreateSoulTomb(class APlayerCharacter* Player);

	UPROPERTY()
	FTimerHandle LoadFinishTimerHandle;

	UFUNCTION()
	void OnTryRestartLoadFinish();
	UFUNCTION()
	void TryLoadFinish();
	UFUNCTION()
	void LoadFinish();


	UFUNCTION()
	void OnAddItemQuickSlotEvent(const class UItemData* Data, int32 SlotIndex);
	UFUNCTION()
	void OnRemoveItemQuickSlotEvent(const class UItemData* Data, int32 SlotIndex);

	UFUNCTION()
	void OnAddAbilityQuickSlotEvent(const TArray<FGameplayTag>& AbilityQuickSlotTags);

	UFUNCTION(BlueprintCallable)
	void ClearSave();
	UFUNCTION(BlueprintCallable)
	void TeleportToLastSavePoint(class APlayerCharacter* Player);
	//게임을 종료했을 때, 상태를 저장합니다.
	void SaveWhenEndPlay(APlayerCharacter* PlayerCharacter);

	//저장후 호출되는 콜백
	void OnSaved(const FString& SlotName, const int32 UserIndex, bool bSucc);

	class USoulLikeSaveGame* GetSaveGameInstance(const FString& SlotName = "SaveFile");
	void SaveGameInstanceToSlot(class USoulLikeSaveGame* SaveInstance, const FString& SlotName = "SaveFile");
	class USoulLikeOptionSaveGame* GetOptionSaveGameInstance(const FString& SlotName = "OptionSaveFile");
	void SaveGameInstanceToSlot(class USoulLikeOptionSaveGame* SaveInstance, const FString& SlotName = "OptionSaveFile");

	//필드에 배치된 아이템을 획득하면 상태를 저장하기 위해 호출됩니다.
	UFUNCTION()
	void OnSaveLevelItemPlacementStateEvent(ABaseCharacter* GetBy, AItemActor* GotItemActor);
	//아이템을 획득하면 저장
	UFUNCTION()
	void OnAddItemEvent(ABaseCharacter* GetBy, const FInventoryItem& ItemInfo, class AItemActor* GotItemActor);
	UFUNCTION()
	void OnAddAbilityItemEvent(ABaseCharacter* GetBy, const FAbilityInformation& AbilityItemInfo, AItemActor* GotItemActor);
	//아이템을 사용하면 저장
	UFUNCTION()
	void OnUseItemEvent(ABaseCharacter* UsedBy, const FInventoryItem& ItemInfo);
	//아이템 강화에 성공하면 호출됩니다.
	UFUNCTION()
	void OnUpgradeEquipmentEvent(const FGuid& ID, class AEquipmentItemActor* EquipmentItemActor);
	//포션 강화에 성공하면 호출됩니다.
	UFUNCTION()
	void OnUpgradePotionEvent(const FGuid& ID, class APotionItemActor* PotionItemActor);
	//메트릭스 슬롯을 열면 호출합니다.
	UFUNCTION()
	void OnSlotOpenEvent(APlayerCharacter* Player, class UOrbMatrix* Matrix);
	//장비 해제하면 호출됨.
	UFUNCTION()
	void OnUnEquipItemEvent(ABaseCharacter* UnEquippedBy, const FInventoryItem& ItemInfo);
	//플레이어가 NPC에게 아이템을 팔면 호출됩니다.
	UFUNCTION()
	void OnNPCBuyItemFromPlayerEvent(class APlayerCharacter* InteractPlayer, const FGuid& ItemID);
	//체크포인트에서 체크포인트 정보를 저장하는 기능을 합니다.
	UFUNCTION()
	void SaveLastCheckpoint(APlayerCharacter* Player, UBonfireComponent* BonfireComponent);
	//아이템이 인벤토리에서 삭제될 떄 호출됩니다.
	UFUNCTION()
	void OnRemoveItemEvent(class ABaseCharacter* UsedBy, const FGuid& RemoveItemUniqueID);


	//사망시 호출됩니다.
	UFUNCTION()
	void OnDeadPlayer(APlayerCharacter* Player);


	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool ExistSaveFile(FString SlotName = "SaveFile");

	//활성화된 화톳불을 저장합니다.
	UFUNCTION()
	void SaveBonfire(class ABonfire* Bonfire);
	//활성화된 화톳불인지 리턴합니다.
	UFUNCTION()
	bool IsActivatedBonfire(const FString& LevelName, const FString& SafeName);
	UFUNCTION()
	void SaveTeleportBonfire(const FBonfireInformation& BonfireInfo);


	//읽은 튜토리얼 액터를 저장합니다.
	void SaveTutorial(class ATutorialActor* TutorialActor);
	void SaveTutorial(FGameplayTag TutorialTag);
	//이미 읽고 넘긴 튜토리얼 위젯인가요? (튜토리얼 액터가 아님)
	UFUNCTION()
	bool IsSkippedTutorial(FGameplayTag TutorialTag);

	//어트리뷰트만 저장할 일이 있을때 호출하세요.
	UFUNCTION()
	void SaveAttribute(APlayerCharacter* PlayerCharacter);

	//경험치를 획득했을 때 저장하기 위해 호출됩니다.
	void SaveAttributeExp(const FAttribute& ExpAttribute);

	//클리어한 우두머리를 저장합니다. 
	void SaveKilledBoss(ABaseMonster* BaseMonster);

	//하늘 상태를 저장합니다.
	void SaveSky(APlayerCharacter* Player);
	void SaveDataLayer(APlayerCharacter* Player);
	
	/**
	 * 상자를 열거나 상자 아이템을 획득하면 호출됩니다.
	 * @param Chest 열거나 아이템을 획득한 상자
	 * @param bEaredChestItem 상자 내부 아이템을 획득하면 참을 주세요.
	 */
	void SaveChest(class AChest* Chest, bool bEaredChestItem);


	/**
	 * NPC를 만난적이 있는가 저장합니다.
	 * @param NPC 
	 */
	void SaveNPCMet(class ANPCBase* NPC);
	/**
	 * NPC가 거점에 합류했는지 저장합니다.
	 * @param NPC 
	 */
	void SaveNPCJoin(class ANPCBase* NPC);

	
	void SaveNPCDestoryed(class ANPCBase* NPC);

	UFUNCTION()
	void OnSellItemToPlayerEvent(APlayerCharacter* InteractPlayer, ANPCBase* Seller, const FMerchandiseItem& MerchandiseItem);
	UFUNCTION()
	void OnSellAbilityToPlayerEvent(APlayerCharacter* InteractPlayer, ANPCBase* Seller, const FMerchandiseAbility& MerchandiseAbility);
	
	
	/**
	 * 저장된 NPC정보를 읽어와 복구합니다.
	 * @param NPC
	 */
	void LoadNPCState(class ANPCBase* NPC);

	TMap<FGameplayTag, FMerchandiseItem> LoadNPCSellItemState(const FGameplayTag& NPCTag);
	TMap<FGameplayTag,FMerchandiseAbility> LoadNPCSellAbilityState(const FGameplayTag& NPCTag);

	friend class USaveGameHelperLibrary;

protected:
	//이미 활성화된 화톳불인지 확인하여 리턴하는 함수입니다. 헬퍼 라이브러리를 통해서 호출하세요.
	bool IsActivatedBonfire(const class ABonfire* Bonfire);
	//이미 플레이어가 획득한 아이템인지 확인하는 함수입니다. 헬퍼 라이브러리를 통해서 호출하세요.
	bool IsAlreadyPickUppedItem(const class AItemActor* Item);
	//이미 읽은 튜토리얼인지 저장합니다.
	bool IsAlreadyReadTutorial(const ATutorialActor* TutorialActor);
	//해당 태그를 가진 우두머리가 처치된적이 있는지 확인합니다.
	bool IsBossKilled(const FGameplayTag& BossMonsterTag);
	//이미 열린 상자인지 확인합니다. 헬퍼 라이브러리를 통해서 호출하세요.
	bool IsOpenedChest(const class AChest* Chest);
	//이미 이 상자의 아이템을 획득했는지 확인합니다.
	bool IsAlreadyGetChestItem(class AChest* Chest);

public:
	//저장된 게임옵션을 불러옵니다.
	FGameOption LoadGameOption(bool& bSucc);
	
	void SaveBGMVolume(float Vol);
	void SaveSFXVolume(float Vol);
	void SaveTextureStreamingPool(int32 TextureStreamingPool);
};