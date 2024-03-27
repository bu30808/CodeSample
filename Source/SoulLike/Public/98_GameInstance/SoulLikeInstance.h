// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/04_NPC/Bonfire.h"
#include "03_Widget/01_Menu/00_Inventory/ItemListWidget.h"
#include "92_Tools/TutorialActor.h"
#include "Engine/GameInstance.h"
#include "WorldPartition/DataLayer/DataLayerInstance.h"
#include "SoulLikeInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInstance, Log, All);

class AItemActor;
class ABaseMonster;


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
	//항상 로드되어있는 레이어 정보입니다.
	//저장된 아이템 스폰이나 영혼무덤 생성에 사용합니다.
	//절대 언로드되면 안 되는 대상들을 이 레이어에 추가하면 됩니다.
	UPROPERTY()
	class UDataLayerAsset* AlwaysActivatedLayer;

	UPROPERTY()
	TSubclassOf<class ASoulTomb> SoulTombClass;
public:
	class UDataLayerAsset* GetAlwaysActivatedLayer() const {return AlwaysActivatedLayer;};

	UPROPERTY()
	int32 SaveIndex=0;

	//이 변수를 직접 사용하지 마세요.
	//GetSaveGameInstance함수를 통해서 불러오세요.
	UPROPERTY()
	class USoulLikeSaveGame* SaveGameInstance;

	//이 값이 참인동안에는 저장이 되지 않습니다. 로드중에 참이 됩니다.
	UPROPERTY(Transient)
	bool bBlockSave = false;
	UPROPERTY()
	TSubclassOf<class UUserWidget> LoadingWidgetClass;

	/*UPROPERTY()
	class USoulLikeSaveGame* SaveGameInstance;*/
public:
	virtual void Init() override;
	//로딩스크린 생성
	void PreLoadMapEvent(const FString& LevelName);
	//월드 로드 이후 호출됨.
	void OnPostLoadMapWithWorldEvent(UWorld* World);
	
	bool IsUseGameSave() const;


	
	UFUNCTION(BlueprintCallable)
	void LoadGame(class APlayerCharacter* Player);

	void LoadLayer(class APlayerCharacter* Player);
	void LoadSky(class APlayerCharacter* Player);
	bool LoadPlayerLocation(class APlayerCharacter* Player);
	void LoadInventory(class APlayerCharacter* Player);
	void LoadAttribute(class APlayerCharacter* Player, bool bIsRespawn);
	void LoadQuickSlotState(class APlayerCharacter* Player);
	void CreateSoulTomb(class APlayerCharacter* Player);
	











	
	UFUNCTION(BlueprintCallable)
	void ClearSave();
	UFUNCTION(BlueprintCallable)
	void TeleportToLastSavePoint(class APlayerCharacter* Player);

	
	//저장후 호출되는 콜백
	void OnSaved(const FString& SlotName, const int32 UserIndex, bool bSucc);
	
	class USoulLikeSaveGame* GetSaveGameInstance(const FString& SlotName = "SaveFile");
	void SaveGameInstanceToSlot(class USoulLikeSaveGame* SaveInstance, const FString& SlotName = "SaveFile");
	


	//필드에 배치된 아이템을 획득하면 상태를 저장하기 위해 호출됩니다.
	UFUNCTION()
	void OnSaveLevelItemPlacementStateEvent(ABaseCharacter* GetBy,  AItemActor* GotItemActor);
	//아이템을 획득하면 저장
	UFUNCTION()
	void OnAddItemEvent(ABaseCharacter* UsedBy, const FInventoryItem& ItemInfo, class AItemActor* GotItemActor);
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
	void OnNPCBuyItemFromPlayerEvent(class APlayerCharacter* InteractPlayer,const FGuid& ItemID);
	//체크포인트에서 체크포인트 정보를 저장하는 기능을 합니다.
	UFUNCTION()
	void SaveWithLastSavePoint(APlayerCharacter* Player, UBonfireComponent* BonfireComponent);
	//아이템이 인벤토리에서 삭제될 떄 호출됩니다.
	UFUNCTION()
	void OnRemoveItemEvent(class ABaseCharacter* UsedBy, const FGuid& RemoveItemUniqueID);
	//퀵슬롯에 아이템이 등록되면 호출됩니다.
	UFUNCTION()
	void OnRegisterQuickSlotItemOrAbility(APlayerCharacter* Player, class UInventoryData* Data,int32 Index);
	//퀵슬롯에 아이템이 해제되면 호출됩니다.
	UFUNCTION()
	void OnUnRegisterQuickSlotItemOrAbility(APlayerCharacter* Player, UInventoryData* Data, int32 Index);

	//사망시 호출됩니다.
	UFUNCTION()
	void OnDeadPlayer(APlayerCharacter* PlayerCharacter);


	UFUNCTION(BlueprintCallable,BlueprintPure)
	bool ExistSaveFile();

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


	//클리어한 우두머리를 저장합니다. 
	void SaveKilledBoss(ABaseMonster* BaseMonster);

	//하늘 상태를 저장합니다.
	void SaveSky(APlayerCharacter* Player);

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
};


