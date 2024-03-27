// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "00_Character/BaseCharacter.h"
#include "97_Interface/InteractionInterface.h"
#include "Components/Button.h"
#include "NPCBase.generated.h"

//이 NPC가 할수있는 행동을 정의합니다. 필요하다면 더 추가하세요.
UENUM(BlueprintType)
enum class ENPCActionType : uint8
{
	Talk,
	Merchant,
	LevelUp,
	Enhancement,
	PotionEnhancement,
	OrbMatrixSlotOpen,
	TeleportBonfire
};

/**
 * NPC의 기본 클래스입니다.
 * 상호작용을 위해 인터렉션 인터페이스를 상속받고 있습니다.
 * 필요에 따라 블루프린트로 상속받아 구현하는것을 추천합니다.
 */
UCLASS()
class SOULLIKE_API ANPCBase : public ABaseCharacter, public IInteractionInterface
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
	class UOrbMatrixSlotOpenComponent* OrbMatrixSlotOpenComponent;

	//이 변수에 담긴 행동들을 할 수 있다고 가정합니다.
	UPROPERTY(EditAnywhere, Category="NPC|Common")
	TArray<ENPCActionType> NPCActions;
	//공용 NPC위젯
	UPROPERTY(EditAnywhere, Category="NPC|Common")
	TSubclassOf<class UNPCWidget> NPCWidgetObject;

	//지금 이 NPC가 상호작용중인 대상을 저장하는 변수입니다.
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<ABaseCharacter> InteractionActor;
	UPROPERTY(Transient)
	TWeakObjectPtr<UNPCWidget> NPCWidget;

	UPROPERTY(EditAnywhere, Category="NPC|Common")
	FName NPCName;
	//이 정보가 설정되어있으면, 상점 컴포넌트를 생성해서 사용합니다.
	UPROPERTY(EditAnywhere, Category="NPC|Common|Merchant")
	TSoftObjectPtr<class UDataTable> MerchantItemPath;
	UPROPERTY(EditAnywhere, Category="NPC|Common|Merchant")
	TSoftObjectPtr<class UDataTable> MerchantAbilityPath;

public:
	ABaseCharacter* GetInteractionActor() { return InteractionActor.Get(); }
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void Interaction_Implementation(ABaseCharacter* Start) override;
	UFUNCTION()
	virtual void FinishInteraction_Implementation() override;

	//플레이어가 이 NPC에게 접근했을 때, 어떠한 상호작용 행동인지 가져옵니다.
	//ex) 상점 / 레벨업  등등
	/*UFUNCTION(BlueprintImplementableEvent)
	FString GetNPCActionName();*/

	const TArray<ENPCActionType>& GetNPCActions() { return NPCActions; }

	UFUNCTION()
	void OnNPCWidgetVisibilityChangedEvent(ESlateVisibility InVisibility);
	
	UFUNCTION(BlueprintCallable)
	void AddNPCWidget();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TalkEvent();
	virtual void TalkEvent_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void MerchantEvent();
	virtual void MerchantEvent_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void LevelUpEvent();
	virtual void LevelUpEvent_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ExitEvent();
	virtual void ExitEvent_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EnhancementEvent();
	virtual void EnhancementEvent_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PotionEnhancementEvent();
	virtual void PotionEnhancementEvent_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OrbMatrixSlotOpenEvent();
	virtual void OrbMatrixSlotOpenEvent_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnTeleportWidgetOpenEvent();
	virtual void OnTeleportWidgetOpenEvent_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BonfireEvent();
	virtual void BonfireEvent_Implementation();

	FOnButtonClickedEvent GetAction(ENPCActionType ActionType);

private:
	UFUNCTION()
	void OnLoadedItemList(UObject* ItemTable);
	UFUNCTION()
	void OnLoadedAbilityList(UObject* AbilityTable);
	

};
