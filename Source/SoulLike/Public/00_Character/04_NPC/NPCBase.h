// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/BaseCharacter.h"
#include "97_Interface/InteractionInterface.h"
#include "97_Interface/00_NPC/MerchantInterface.h"
#include "NPCBase.generated.h"



//NPC의 상태를 저장하는 구조체
USTRUCT(BlueprintType)
struct FNPCState
{
	GENERATED_BODY()
public:
	//만난적 있나?
	UPROPERTY(BlueprintReadWrite)
	bool bHasMet = false;
	//거점에 합류했나?
	UPROPERTY(BlueprintReadWrite)
	bool bJoinBasement = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsDestroyed = false;

	//필요하다면 사용할 고유 아이디
	//같은 NPC지만, 상황에 따라 다른 장소에 사본이 배치될 수 있습니다.
	//그 때, 고유 이름은 다르기 때문에 두개의 같지만 다른 NPC 객체를 구분하기 위해 사용합니다.
	UPROPERTY(BlueprintReadWrite)
	FString SafeName;
	
	//필요하다면 사용할 위치
	UPROPERTY(BlueprintReadWrite)
	FVector NPCLocation;

	FNPCState(){ }

	FNPCState(class ANPCBase* NPC);
	FNPCState(class ANPCBase* NPC,bool Destroyed);
	void Update(class ANPCBase* NPC);
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadNPCState,const FNPCState&, SavedState);

//이 NPC가 할수있는 행동을 정의합니다. 필요하다면 더 추가하세요.
UENUM(BlueprintType)
enum class ENPCActionType : uint8
{
	Merchant,
	LevelUp,
	Enhancement,
	PotionEnhancement,
	MatrixSlotOpen,
	TeleportBonfire,
	Chest
};

UENUM(BlueprintType)
enum class EEnhancementType :uint8
{
	Equipment,
	Orb
};

/**
 * NPC의 기본 클래스입니다.
 * 상호작용을 위해 인터렉션 인터페이스를 상속받고 있습니다.
 * 필요에 따라 블루프린트로 상속받아 구현하는것을 추천합니다.
 */
UCLASS()
class SOULLIKE_API ANPCBase : public ABaseCharacter, public IInteractionInterface, public IMerchantInterface
{
	GENERATED_BODY()

protected:
	ANPCBase();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UMerchantComponent* MerchantComponent;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class ULevelUpComponent* LevelUpComponent;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UEnhancementComponent* EnhancementComponent;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UPotionEnhancementComponent* PotionEnhancementComponent;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UBonfireComponent* BonfireComponent;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UMatrixSlotOpenComponent* MatrixSlotOpenComponent;

	//이 변수에 담긴 행동들을 할 수 있다고 가정합니다.
	UPROPERTY(EditAnywhere, Category="NPC|Common")
	TArray<ENPCActionType> NPCActions;

	
	//이 NPC가 플레이어와 만난적이 있나요?
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="NPC|Common")
	bool bHasMetNPC = false;
	//이 NPC가 거점에 합류했나요?
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="NPC|Common")
	bool bHasNPCJoined = false;
	
	//지금 이 NPC가 상호작용중인 대상을 저장하는 변수입니다.
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<ABaseCharacter> InteractionActor;


	UPROPERTY(EditAnywhere, Category="NPC|Common")
	FName NPCName;
	UPROPERTY(EditAnywhere, Category="NPC|Common")
	class UDialogue* DialogueAsset;
	UPROPERTY(EditAnywhere, Category="NPC|Common")
	FGameplayTag NPCTag;
	//이 정보가 설정되어있으면, 상점 컴포넌트를 생성해서 사용합니다.
	UPROPERTY(EditAnywhere, Category="NPC|Common|Merchant")
	TSoftObjectPtr<class UDataTable> MerchantItemPath;
	UPROPERTY(EditAnywhere, Category="NPC|Common|Merchant")
	TSoftObjectPtr<class UDataTable> MerchantAbilityPath;

	
public:
	//NPC의 상태가 로드되었을 때 할 행동을 정의합니다.
	//각 NPC마다 상황이 다르기 때문에 따로 정의하세요.
	UPROPERTY(BlueprintAssignable)
	FOnLoadNPCState OnLoadNPCState;


	UFUNCTION(BlueprintCallable,BlueprintPure)
	const FGameplayTag& GetNPCTag() const {return NPCTag;}
	
	/**
	 * 만남 상태를 저장합니다. 
	 * @param bSave 참이라면, 세이브파일에 저장합니다.
	 */
	UFUNCTION(BlueprintCallable)
	void SetMetNPC(bool bSave = true);
	UFUNCTION(BlueprintCallable,BlueprintPure)
	bool HasMet() const {return bHasMetNPC;}

	
	/**
	 * 거점에 합류했는지 저장합니다.
	 * @param bSave 참이라면, 세이브파일에 저장합니다.
	 * @param bSave = true
	 */
	UFUNCTION(BlueprintCallable)
	void SetJoinNPCAtBasement(bool bSave = true);
	UFUNCTION(BlueprintCallable,BlueprintPure)
	bool HasJoinNPCAtBasement() const {return bHasNPCJoined;}

protected:
	UFUNCTION(BlueprintCallable)
	void SaveMet();
	UFUNCTION(BlueprintCallable)
	void SaveJoin();
	UFUNCTION(BlueprintCallable)
	void SaveDestroyed();
public:

	ABaseCharacter* GetInteractionActor() const { return InteractionActor.Get(); }
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void Interaction_Implementation(ABaseCharacter* Start) override;
	UFUNCTION()
	virtual void FinishInteraction_Implementation() override;
	void LoadNPCState();
	
	const TArray<ENPCActionType>& GetNPCActions() { return NPCActions; }
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void MerchantEvent();
	virtual void MerchantEvent_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void LevelUpEvent();
	virtual void LevelUpEvent_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EnhancementEvent(EEnhancementType EnhancementType);
	virtual void EnhancementEvent_Implementation(EEnhancementType EnhancementType);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PotionEnhancementEvent();
	virtual void PotionEnhancementEvent_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void MatrixSlotOpenEvent();
	virtual void MatrixSlotOpenEvent_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnTeleportWidgetOpenEvent();
	virtual void OnTeleportWidgetOpenEvent_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BonfireEvent();
	virtual void BonfireEvent_Implementation();
	
	UFUNCTION(BlueprintCallable,BlueprintPure)
	FString GetSafeName() const {return GetNameSafe(this);}
private:
	UFUNCTION()
	void OnLoadedItemList(UObject* ItemTable);
	UFUNCTION()
	void OnLoadedAbilityList(UObject* AbilityTable);

	virtual void OnRemoveAttributeEffectAdditionalInformationEvent_Implementation(const FAttributeEffect& Effect, UAbilityEffectAdditionalInformation* AdditionalInformation, float DeltaTime) override;


public:
	//인터페이스
	virtual void SellItemToPlayer(ANPCBase* Merchant, APlayerCharacter* PlayerCharacter, FGuid ItemUniqueID, int32 TradeCount) override;
	virtual void SellAbilityToPlayer(ANPCBase* Merchant, APlayerCharacter* PlayerCharacter, FMerchandiseAbility MerchandiseAbility, int32 TradeCount) override;
	virtual void BuyAbilityFromPlayer(ANPCBase* Merchant, APlayerCharacter* PlayerCharacter, FMerchandiseAbility MerchandiseAbility, int32 BuyCount) override;
	virtual void BuyItemFromPlayer(ANPCBase* Merchant, APlayerCharacter* PlayerCharacter, FGuid ItemUniqueID, int32 BuyCount) override;
	virtual void UpdateExpWidget(APlayerCharacter* Player, const int32& AddExp) override;
	virtual UMerchantComponent* GetMerchantComponent() override;

	virtual void SetActorHiddenInGame(bool bNewHidden) override;

	
};
