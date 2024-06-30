// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MerchantListWidget.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "00_Character/04_NPC/99_Component/MerchantComponent.h"
#include "MerchandiseCheckWidget.generated.h"

//파는거니 사는거니?
UENUM()
enum class EPlayerTradeToNPCType : uint8
{
	Purchase,
	Sell
};

//어떤 종류를 거래하니?
UENUM()
enum class ETradeObjectType :uint8
{
	Item,
	Ability
};

/**
 * 
 */
UCLASS()
class SOULLIKE_API UMerchandiseCheckWidget : public UPopUpBasedWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Action;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Name;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Count;

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Up;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Down;


	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Ok;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Cancel;


	UPROPERTY(Transient)
	EPlayerTradeToNPCType TradeType;
	UPROPERTY(Transient)
	ETradeObjectType TradeObjectType;

	UPROPERTY(Transient)
	int32 MaxCount = 0;
	UPROPERTY(Transient)
	int32 TradeCount = 1;
	UPROPERTY()
	TWeakObjectPtr<class ANPCBase> MerchantNPC;
	//거래하는 아이템의 고유 아이디
	UPROPERTY()
	FGuid ItemUniqueID;

	//거래하는 어빌리티의 정보
	UPROPERTY()
	FMerchandiseAbility TradeMerchandiseAbility;


	virtual void OnVisibilityChangedEvent(ESlateVisibility InVisibility) override;

	virtual void NativeConstruct() override;

	void TradeItem() const;
	void TradeAbility() const;

public:

	void SetOwnerNPC(class ANPCBase* NPC);
	
	UFUNCTION()
	void OnClickedUpButton();
	UFUNCTION()
	void OnClickedDownButton();
	UFUNCTION()
	void OnClickedOkButton();
	UFUNCTION()
	void OnClickedCancelButton();

	//플레이어가 아이템을 팔 때 필요한 설정을 합니다.
	void PlayerSellSetting(ANPCBase* NPC, const FInventoryItem& Item);
	//플레이어가 아이템을 구입할 때 필요한 설정을 합니다.
	void PlayerPurchaseSetting(ANPCBase* NPC, const FMerchandiseItem& Merchandise);

	//플레이어가 어빌리티를 구입할 때 필요한 설정을 합니다.
	void PlayerPurchaseSetting(ANPCBase* NPC, const FMerchandiseAbility& MerchandiseAbility);
};

#define LOCTEXT_NAMESPACE "MerchandiseCheckWidget"

namespace GlobalMerchandiseCheckWidgetText
{
	const FText buyText = NSLOCTEXT("MerchandiseCheckWidget", "BuyText", "다음 아이템을 구매합니다");
	const FText sellText = NSLOCTEXT("MerchandiseCheckWidget", "SellText", "다음 아이템을 판매합니다");
	const FText repurchaseText = NSLOCTEXT("MerchandiseCheckWidget", "RepurchaseText", "재구매");
	const FText cannotSellEquippedItemText = NSLOCTEXT("MerchandiseCheckWidget", "CannotSellEquippedItemText",
	                                                   "장착중인 아이템을 팔 수 없습니다.");
	const FText cannotSellText = NSLOCTEXT("MerchandiseCheckWidget", "cannotSellText", "판매 불가능한 아이템입니다.");
}
#undef LOCTEXT_NAMESPACE
