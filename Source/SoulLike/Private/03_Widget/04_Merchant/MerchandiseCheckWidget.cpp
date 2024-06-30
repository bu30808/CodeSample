// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/04_Merchant/MerchandiseCheckWidget.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "04_Item/ItemActor.h"
#include "97_Interface/00_NPC/MerchantInterface.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Logging/StructuredLog.h"
#include "00_Character/04_NPC/NPCBase.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UMerchandiseCheckWidget::OnVisibilityChangedEvent(ESlateVisibility InVisibility)
{
	Super::OnVisibilityChangedEvent(InVisibility);

	if (IsVisible())
	{
		TradeCount = 1;
	}
}

void UMerchandiseCheckWidget::NativeConstruct()
{
	bUseAsPopUp = true;
	bRemovable = true;
	Super::NativeConstruct();

	Button_Up->OnClicked.AddUniqueDynamic(this, &UMerchandiseCheckWidget::OnClickedUpButton);
	Button_Down->OnClicked.AddUniqueDynamic(this, &UMerchandiseCheckWidget::OnClickedDownButton);

	Button_Ok->OnClicked.AddUniqueDynamic(this, &UMerchandiseCheckWidget::OnClickedOkButton);
	Button_Cancel->OnClicked.AddUniqueDynamic(this, &UMerchandiseCheckWidget::OnClickedCancelButton);


	TextBlock_Count->SetText(FText::AsNumber(1));
}

void UMerchandiseCheckWidget::TradeItem() const
{
	UE_LOGFMT(LogTemp,Log,"{0} {1}",__FUNCTION__,__LINE__);
	if (UKismetSystemLibrary::DoesImplementInterface(MerchantNPC.Get(), UMerchantInterface::StaticClass()))
	{
		UE_LOGFMT(LogTemp,Log,"{0} {1}",__FUNCTION__,__LINE__);
		
		switch (TradeType)
		{
		case EPlayerTradeToNPCType::Purchase:
			UE_LOGFMT(LogTemp,Log,"{0} {1}",__FUNCTION__,__LINE__);
			MerchantNPC->SellItemToPlayer(MerchantNPC.Get(), GetOwningPlayerPawn<APlayerCharacter>(), ItemUniqueID,
										TradeCount);
			break;
		case EPlayerTradeToNPCType::Sell:
			UE_LOGFMT(LogTemp,Log,"{0} {1}",__FUNCTION__,__LINE__);
			MerchantNPC->BuyItemFromPlayer(MerchantNPC.Get(), GetOwningPlayerPawn<APlayerCharacter>(), ItemUniqueID,
										 TradeCount);
			break;
		default: ;
		}
	}
}


void UMerchandiseCheckWidget::TradeAbility() const
{
	if (UKismetSystemLibrary::DoesImplementInterface(MerchantNPC.Get(), UMerchantInterface::StaticClass()))
	{
		auto interface = Cast<IMerchantInterface>(MerchantNPC);
		switch (TradeType)
		{
		case EPlayerTradeToNPCType::Purchase:
			interface->SellAbilityToPlayer(MerchantNPC.Get(), GetOwningPlayerPawn<APlayerCharacter>(),
			                               TradeMerchandiseAbility, TradeCount);
			break;
		case EPlayerTradeToNPCType::Sell:
			interface->BuyAbilityFromPlayer(MerchantNPC.Get(), GetOwningPlayerPawn<APlayerCharacter>(),
			                                TradeMerchandiseAbility, TradeCount);
			break;
		default: ;
		}
	}
}

void UMerchandiseCheckWidget::SetOwnerNPC(ANPCBase* NPC)
{
	MerchantNPC = NPC;
}

void UMerchandiseCheckWidget::OnClickedUpButton()
{
	int32 count = FCString::Atof(*TextBlock_Count->GetText().ToString());
	count = FMath::Clamp(count + 1, 1, MaxCount);
	TradeCount = count;
	TextBlock_Count->SetText(FText::AsNumber(TradeCount));

	UE_LOGFMT(LogTemp, Log, "거래 갯수 갱신 : {0}", TradeCount);
}

void UMerchandiseCheckWidget::OnClickedDownButton()
{
	int32 count = FCString::Atof(*TextBlock_Count->GetText().ToString());
	count = FMath::Clamp(count - 1, 1, MaxCount);
	TradeCount = count;
	TextBlock_Count->SetText(FText::AsNumber(TradeCount));
	UE_LOGFMT(LogTemp, Log, "거래 갯수 갱신 : {0}", TradeCount);
}

void UMerchandiseCheckWidget::OnClickedOkButton()
{
	UE_LOGFMT(LogTemp,Log,"{0} {1}",__FUNCTION__,__LINE__);
	
	SetVisibility(ESlateVisibility::Collapsed);
	if (MerchantNPC.IsValid())
	{
		switch (TradeObjectType)
		{
		case ETradeObjectType::Item:
			TradeItem();
			break;
		case ETradeObjectType::Ability:
			TradeAbility();
			break;
		default: ;
		}
	}
}

void UMerchandiseCheckWidget::OnClickedCancelButton()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UMerchandiseCheckWidget::PlayerSellSetting(ANPCBase* NPC, const FInventoryItem& Item)
{
	MerchantNPC = NPC;

	TextBlock_Count->SetText(FText::AsNumber(1));

	MaxCount = Item.ItemCount;
	ItemUniqueID = Item.UniqueID;

	TextBlock_Name->SetText(Item.GetItemInformation()->Item_Name);

	TextBlock_Action->SetText(GlobalMerchandiseCheckWidgetText::sellText);

	TradeType = EPlayerTradeToNPCType::Sell;
	TradeObjectType = ETradeObjectType::Item;
}

void UMerchandiseCheckWidget::PlayerPurchaseSetting(ANPCBase* NPC,
                                                    const FMerchandiseItem& Merchandise)
{
	MerchantNPC = NPC;

	TextBlock_Count->SetText(FText::AsNumber(1));

	if (Merchandise.MerchandiseData.bSellInfinite)
	{
		MaxCount = INT_MAX;
	}
	else
	{
		MaxCount = Merchandise.MerchandiseData.Count;
	}
	ItemUniqueID = Merchandise.UniqueID;

	TextBlock_Name->SetText(Merchandise.GetItemInformation()->Item_Name);

	TextBlock_Action->SetText(GlobalMerchandiseCheckWidgetText::buyText);

	TradeType = EPlayerTradeToNPCType::Purchase;
	TradeObjectType = ETradeObjectType::Item;
}

void UMerchandiseCheckWidget::PlayerPurchaseSetting(ANPCBase* NPC, const FMerchandiseAbility& MerchandiseAbility)
{
	MerchantNPC = NPC;

	TradeMerchandiseAbility = MerchandiseAbility;

	TextBlock_Count->SetText(FText::AsNumber(1));
	MaxCount = 1;

	TextBlock_Name->SetText(MerchandiseAbility.GetAbilityInformation()->AbilityName);
	TextBlock_Action->SetText(GlobalMerchandiseCheckWidgetText::buyText);

	TradeType = EPlayerTradeToNPCType::Purchase;
	TradeObjectType = ETradeObjectType::Ability;
}
