// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/04_NPC/99_Component/MerchantComponent.h"
#include "02_Ability/AbilityBase.h"
#include "03_Widget/97_PopUp/PopUpBasedWidget.h"


#include "MerchantWidget.generated.h"

UCLASS()
class UBaseMerchandiseListData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	TWeakObjectPtr<class UMerchantListWidget> MerchantListWidget;
};

UCLASS()
class UMerchandiseItemListData : public UBaseMerchandiseListData
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	FMerchandiseItem MerchandiseItem;
};

UCLASS()
class UMerchandiseAbilityListData : public UBaseMerchandiseListData
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	FMerchandiseAbility MerchandiseAbility;
};


/**
 * 
 */
UCLASS()
class SOULLIKE_API UMerchantWidget : public UPopUpBasedWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class UMerchantListWidget* UMG_MerchantList;
	UPROPERTY(meta=(BindWidget))
	class UItemListWidget* UMG_Inventory;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMerchandiseCheckWidget> CheckWidgetObject;


	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UPROPERTY()
	TWeakObjectPtr<class ANPCBase> MerchantNPC;

	UPROPERTY()
	TWeakObjectPtr<class UMerchandiseCheckWidget> CheckWidget;

public:
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Close;

	void SetOwnerNPC(class ANPCBase* NPC);
	
	void CreateMerchandiseList(class UMerchantComponent* MerchantComponent);

	//플레이어가 NPC에게 아이템을 구매할때 호출됩니다.
	UFUNCTION()
	void OnPlayerBuyItemEvent(const FMerchandiseItem& Merchandise);
	//플레이어가 NPC에게 아이템을 팔 때 호출되비다.
	UFUNCTION()
	void OnPlayerSellItemEvent(const FInventoryItem& Item);

	//플레이어가 NPC에게 어빌리티를 구매할때 호출됩니다.
	UFUNCTION()
	void OnPlayerBuyAbilityEvent(const FMerchandiseAbility& MerchandiseAbility);
	//플레이어가 NPC에게 어빌리티를 판매할 때 호출됩니다.
	UFUNCTION()
	void OnPlayerSellAbilityEvent(const FAbilityInformation& AbilityInformation);

	UFUNCTION()
	void OnClickedCloseButton();
};
