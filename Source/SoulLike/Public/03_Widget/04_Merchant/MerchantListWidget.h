// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/04_NPC/99_Component/MerchantComponent.h"
#include "02_Ability/AbilityBase.h"
#include "03_Widget/99_Drag/DropTargetWidget.h"
#include "MerchantListWidget.generated.h"


UENUM()
enum class EShopType :uint8
{
	Purchase,
	Repurchase
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerSellItem, const FInventoryItem&, Item);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerSellAbility, const FAbilityInformation&, Ability);

/**
 * 
 */
UCLASS()
class SOULLIKE_API UMerchantListWidget : public UDropTargetWidget
{
	GENERATED_BODY()

	friend class UMerchantButtonWidget;

protected:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Souls;
	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher* Switcher;
	UPROPERTY(meta=(BindWidget))
	class UListView* ListView_Purchase;
	UPROPERTY(meta=(BindWidget))
	class UListView* ListView_Repurchase;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Repurchase;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_ButtonText;


	UPROPERTY()
	EShopType ShopType = EShopType::Purchase;

	UPROPERTY()
	TArray<class UBaseMerchandiseListData*> MerchantData;

	UPROPERTY()
	TArray<class UBaseMerchandiseListData*> MerchantRepurchaseData;

	UPROPERTY()
	TWeakObjectPtr<class ANPCBase> MerchantNPC;

public:
	FOnPlayerSellItem OnPlayerSellItem;
	FOnPlayerSellAbility OnPlayerSellAbility;

	EShopType GetShopType() { return ShopType; }

	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	                          UDragDropOperation* InOperation) override;
	void CreateMerchandiseList(UMerchantComponent* MerchantComponent, const TMap<FGameplayTag, FMerchandiseItem>& ItemState, const TMap<FGameplayTag, FMerchandiseAbility>& AbilityState);

	UFUNCTION()
	void OnClickedRepurchaseButton();

	void CreateRepurchaseList();

	UFUNCTION()
	void UpdatePlayerExp(APlayerCharacter* Player);

	bool PlayerSellItemOrAbility(class UInventoryData* InventoryData);

	UFUNCTION()
	void UpdateRepurchaseList();

	UFUNCTION()
	void OnBuyItemFromPlayerEvent(APlayerCharacter* Player, const FGuid& ItemID);
	UFUNCTION()
	void OnSellItemToPlayerEvent(APlayerCharacter* Player, ANPCBase* Seller, const FMerchandiseItem& MerchandiseItem);
	UFUNCTION()
	void OnSellAbilityToPlayerEvent(APlayerCharacter* Player, ANPCBase* Seller, const FMerchandiseAbility& MerchandiseAbility);
};
