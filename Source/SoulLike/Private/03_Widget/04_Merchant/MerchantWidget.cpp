// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/04_Merchant/MerchantWidget.h"

#include "00_Character/04_NPC/99_Component/MerchantComponent.h"
#include "03_Widget/01_Menu/00_Inventory/ItemListWidget.h"
#include "03_Widget/04_Merchant/MerchantListWidget.h"
#include "03_Widget/04_Merchant/MerchandiseCheckWidget.h"
#include "03_Widget/05_Alert/AlertWidget.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "00_Character/00_Player/00_Controller/UserController.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "00_Character/04_NPC/NPCBase.h"
#include "97_Interface/00_NPC/MerchantInterface.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Logging/StructuredLog.h"

void UMerchantWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (UMG_Inventory)
	{
		UMG_Inventory->OnPlayerBuyItemFromNPC.AddUniqueDynamic(this, &UMerchantWidget::OnPlayerBuyItemEvent);
		UMG_Inventory->OnPlayerBuyAbilityFromNPC.AddUniqueDynamic(this, &UMerchantWidget::OnPlayerBuyAbilityEvent);
		UMG_Inventory->MerchantSetting();
	}

	if (UMG_MerchantList)
	{
		UMG_MerchantList->OnPlayerSellItem.AddUniqueDynamic(this, &UMerchantWidget::OnPlayerSellItemEvent);
		UMG_MerchantList->OnPlayerSellAbility.AddUniqueDynamic(this, &UMerchantWidget::OnPlayerSellAbilityEvent);
	}
}

void UMerchantWidget::NativeConstruct()
{
	bUseAsPopUp = true;
	bRemovable = true;

	Super::NativeConstruct();

	if (UMG_Inventory)
	{
		UMG_Inventory->CreateInventoryItemList();
	}

	if (Button_Close)
	{
		Button_Close->OnClicked.AddUniqueDynamic(this, &UMerchantWidget::OnClickedCloseButton);
	}

	if (CheckWidgetObject)
	{
		CheckWidget = CreateWidget<UMerchandiseCheckWidget>(GetOwningPlayer(), CheckWidgetObject);
	}
	

	if(auto tuto  = UWidgetHelperLibrary::ShowTutorialWidget(GetOwningPlayer(), FGameplayTag::RequestGameplayTag("Tutorial.Merchant")))
	{
		tuto->OnClosedTutorialWidget.BindDynamic(this,&UPopUpBasedWidget::SetFocusOnThisWidget);
	}
}

void UMerchantWidget::SetOwnerNPC(ANPCBase* NPC)
{
	MerchantNPC = NPC;
	CheckWidget->SetOwnerNPC(MerchantNPC.Get());
}

void UMerchantWidget::CreateMerchandiseList(UMerchantComponent* MerchantComponent)
{
	if (MerchantNPC.IsValid() && UKismetSystemLibrary::DoesImplementInterface(
		MerchantNPC.Get(), UMerchantInterface::StaticClass()))
	{
		MerchantComponent->OnBuyItemFromPlayer.
		                   AddUniqueDynamic(UMG_MerchantList, &UMerchantListWidget::OnBuyItemFromPlayerEvent);
		MerchantComponent->OnSellItemToPlayer.AddUniqueDynamic(UMG_MerchantList,
		                                                       &UMerchantListWidget::OnSellItemToPlayerEvent);
		
		MerchantComponent->OnSellAbilityToPlayer.AddUniqueDynamic(UMG_MerchantList,
		                                                          &UMerchantListWidget::OnSellAbilityToPlayerEvent);

		if (UMG_MerchantList)
		{
			UMG_MerchantList->CreateMerchandiseList(MerchantComponent);
		}
	}
}

void UMerchantWidget::OnPlayerBuyItemEvent(const FMerchandiseItem& Merchandise)
{
	if (CheckWidget.IsValid())
	{
		if (CheckWidget->IsInViewport() == false)
		{
			CheckWidget->AddToViewport();
		}

		if (CheckWidget->IsVisible() == false)
		{
			CheckWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		CheckWidget->PlayerPurchaseSetting(MerchantNPC.Get(), Merchandise);
	}
}

void UMerchantWidget::OnPlayerSellItemEvent(const FInventoryItem& Item)
{
	//혹시나 장비에 장착중인 아이템을 파려고 하는지 확인합니다.
	if (GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent()->IsEquippedEquipment(Item.UniqueID))
	{
		UWidgetHelperLibrary::ShowAlertMsg(GetOwningPlayer<AUserController>(), EAlertMsgType::Warning,
		                                   GlobalMerchandiseCheckWidgetText::cannotSellEquippedItemText,
		                                   FOnButtonClicked());
		return;
	}

	//판매 가능한 상태의 아이템인지 확인합니다.
	if (Item.GetItemInformation()->bSellable == false)
	{
		UWidgetHelperLibrary::ShowAlertMsg(GetOwningPlayer<AUserController>(), EAlertMsgType::Normal,
		                                   GlobalMerchandiseCheckWidgetText::cannotSellText, FOnButtonClicked());
		return;
	}


	if (CheckWidget.IsValid())
	{
		if (CheckWidget->IsInViewport() == false)
		{
			CheckWidget->AddToViewport();
		}

		if (CheckWidget->IsVisible() == false)
		{
			CheckWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		CheckWidget->PlayerSellSetting(MerchantNPC.Get(), Item);
	}
}

void UMerchantWidget::OnPlayerBuyAbilityEvent(const FMerchandiseAbility& MerchandiseAbility)
{
	if (CheckWidget.IsValid())
	{
		if (CheckWidget->IsInViewport() == false)
		{
			CheckWidget->AddToViewport();
		}

		if (CheckWidget->IsVisible() == false)
		{
			CheckWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		CheckWidget->PlayerPurchaseSetting(MerchantNPC.Get(), MerchandiseAbility);
	}
}

void UMerchantWidget::OnPlayerSellAbilityEvent(const FAbilityInformation& AbilityInformation)
{
	//퀵슬롯에 올라간 어빌리티를 판매하려는지 확인합니다.
}

void UMerchantWidget::OnClickedCloseButton()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
