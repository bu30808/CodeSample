// Fill out your copyright notice in the Description page of Project Settings.


#include "97_Interface/00_NPC/MerchantInterface.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "00_Character/04_NPC/99_Component/MerchantComponent.h"
#include "03_Widget/05_Alert/AlertWidget.h"
#include "04_Item/ItemActor.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "Logging/StructuredLog.h"
#include "00_Character/00_Player/00_Controller/UserController.h"
#include "00_Character/04_NPC/NPCBase.h"

// Add default functionality here for any IMerchantInterface functions that are not pure virtual.

void IMerchantInterface::BuyItemFromPlayer(ANPCBase* Merchant, APlayerCharacter* PlayerCharacter, FGuid ItemUniqueID,
                                           int32 BuyCount)
{
	if (auto invenComp = PlayerCharacter->GetInventoryComponent())
	{
		const auto& item = invenComp->GetInventoryItem(ItemUniqueID);
		//아이템이 갯수만큼 있는지 확인
		if (item.ItemCount >= BuyCount)
		{
			//아이템 카운트 제거
			if (invenComp->DecreaseItemCount(ItemUniqueID, BuyCount))
			{
				UE_LOGFMT(LogActor, Log, "플레이어에게 {0}을 {1}개 구매했습니다.", item.GetItemInformation()->Item_Tag.ToString(),
				          BuyCount);
				//비용 지불
				auto attComp = PlayerCharacter->GetAttributeComponent();
				attComp->SetEXP(attComp->GetEXP() + item.GetItemInformation()->Item_Price * BuyCount);
				
				UpdateExpWidget(PlayerCharacter,item.GetItemInformation()->Item_Price * BuyCount);

				attComp->OnCharacterInformationUpdate.Broadcast();
				Execute_GetMerchantComponent(Merchant)->OnBuyItemFromPlayer.Broadcast(PlayerCharacter,ItemUniqueID);

				//재구매 리스트에 추가함.
				Execute_GetMerchantComponent(Merchant)->AddRepurchaseItem(item, BuyCount);
				
			}
		}
	}
}

void IMerchantInterface::SellItemToPlayer(ANPCBase* Merchant, APlayerCharacter* PlayerCharacter, FGuid ItemUniqueID,
                                          int32 TradeCount)
{
	UE_LOGFMT(LogTemp, Log, "NPC가 플레이어에게 아이템을 파려고 합니다 : {0}, {1} 개", ItemUniqueID.ToString(), TradeCount);


	class UMerchantComponent* MerchantComponent = Execute_GetMerchantComponent(Merchant);
	if (MerchantComponent->IsMerchandiseItem(ItemUniqueID) || MerchantComponent->
		IsRepurchaseMerchandiseItem(ItemUniqueID))
	{
		const bool bIsMerchandise = MerchantComponent->IsMerchandiseItem(ItemUniqueID);

		// 판매 또는 재구매하는 아이템 정보를 가져옵니다.
		const auto& merchandise = bIsMerchandise
			                          ? MerchantComponent->GetMerchandiseItem(ItemUniqueID)
			                          : MerchantComponent->GetRepurchaseMerchandiseItem(ItemUniqueID);

		// 구입하려는 갯수가 충분한지 확인합니다.
		if (merchandise.MerchandiseData.bSellInfinite == false)
		{
			if (merchandise.MerchandiseData.Count < TradeCount)
			{
			
				UWidgetHelperLibrary::ShowAlertMsg(PlayerCharacter->GetController<AUserController>(),
				                                   EAlertMsgType::Normal, GlobalMerchantText::buyText,FOnButtonClicked());
				return;
			}
		}

		// 충분한 재화가 있는지 확인합니다.
		const int32 TotalPrice = merchandise.MerchandiseData.Price * TradeCount;
		if (PlayerCharacter->GetAttributeComponent()->GetEXP() < TotalPrice)
		{
			
			UWidgetHelperLibrary::ShowAlertMsg(PlayerCharacter->GetController<AUserController>(), EAlertMsgType::Error,
			                                   GlobalMerchantText::alertText,FOnButtonClicked());
			return;
		}

		// 갯수를 차감하고 비용을 받습니다.
		if (bIsMerchandise)
		{
			if (MerchantComponent->DecreaseMerchandiseItemCount(ItemUniqueID, TradeCount) == false)
			{
				UE_LOGFMT(LogTemp, Error, "상품의 카운트를 줄이는데 실패했습니다.");
				return;
			}
		}
		else
		{
			if (MerchantComponent->DecreaseRepurchaseMerchandiseItemCount(ItemUniqueID, TradeCount) == false)
			{
				UE_LOGFMT(LogTemp, Error, "재구매 상품의 카운트를 줄이는데 실패했습니다.");
				return;
			}
		}

		PlayerCharacter->GetAttributeComponent()->SetEXP(
			PlayerCharacter->GetAttributeComponent()->GetEXP() - TotalPrice);
		UpdateExpWidget(PlayerCharacter,TotalPrice * -1);

		// 아이템을 넘깁니다.
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = PlayerCharacter;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AItemActor* sellItem = PlayerCharacter->GetWorld()->SpawnActor<AItemActor>(
			merchandise.MerchandiseData.ItemActorObject.LoadSynchronous(), spawnParams);
		sellItem->SetItemCount(TradeCount);

		//이 함수 내부에서 자동 저장.
		PlayerCharacter->GetInventoryComponent()->AddItem(sellItem, false);

		PlayerCharacter->GetAttributeComponent()->OnCharacterInformationUpdate.Broadcast();
		MerchantComponent->OnSellItemToPlayer.Broadcast(PlayerCharacter,ItemUniqueID);
	}
}

void IMerchantInterface::BuyAbilityFromPlayer(ANPCBase* Merchant, APlayerCharacter* PlayerCharacter,
                                              FMerchandiseAbility MerchandiseAbility, int32 BuyCount)
{
	class UMerchantComponent* MerchantComponent = Execute_GetMerchantComponent(Merchant);

	if (auto abComp = PlayerCharacter->GetAbilityComponent())
	{
		if (abComp->HasAbility(MerchandiseAbility.MerchandiseAbilityData.Tag))
		{
			abComp->RemoveAbility(MerchandiseAbility.MerchandiseAbilityData.Tag);
			UE_LOGFMT(LogActor, Log, "플레이어에게 {0}을 구매했습니다.", MerchandiseAbility.MerchandiseAbilityData.Tag.ToString());
			//비용 지불
			auto attComp = PlayerCharacter->GetAttributeComponent();
			attComp->SetEXP(attComp->GetEXP() + MerchandiseAbility.MerchandiseAbilityData.Price * BuyCount);
			UpdateExpWidget(PlayerCharacter,MerchandiseAbility.MerchandiseAbilityData.Price * BuyCount);

			attComp->OnCharacterInformationUpdate.Broadcast();
			MerchantComponent->OnBuyAbilityFromPlayer.Broadcast(PlayerCharacter);

			//재구매 리스트에 추가함.
			MerchantComponent->AddRepurchaseAbility(MerchandiseAbility);
		}
	}
}

void IMerchantInterface::SellAbilityToPlayer(ANPCBase* Merchant, APlayerCharacter* PlayerCharacter,
                                             FMerchandiseAbility MerchandiseAbility, int32 TradeCount)
{
	class UMerchantComponent* MerchantComponent = Execute_GetMerchantComponent(Merchant);

	auto abilityTag = MerchandiseAbility.MerchandiseAbilityData.Tag;
	UE_LOGFMT(LogTemp, Log, "NPC가 플레이어에게 어빌리티를 파려고 합니다 : {0}, {1} 개", abilityTag.ToString(), TradeCount);
	if (MerchantComponent->IsMerchandiseAbility(abilityTag) || MerchantComponent->
		IsRepurchaseMerchandiseAbility(abilityTag))
	{
		const bool bIsMerchandise = MerchantComponent->IsMerchandiseAbility(abilityTag);

		// 판매 또는 재구매하는 아이템 정보를 가져옵니다.
		const auto& merchandise = bIsMerchandise
			                          ? MerchantComponent->GetMerchandiseAbility(abilityTag)
			                          : MerchantComponent->GetRepurchaseMerchandiseAbility(abilityTag);

		// 구입하려는 갯수가 충분한지 확인합니다.
		if (merchandise.MerchandiseAbilityData.bSellInfinite == false)
		{
			if (merchandise.MerchandiseAbilityData.Count < TradeCount)
			{
			
				UWidgetHelperLibrary::ShowAlertMsg(PlayerCharacter->GetController<AUserController>(),
				                                   EAlertMsgType::Normal, GlobalMerchantText::buyText,FOnButtonClicked());
				return;
			}
		}

		// 충분한 재화가 있는지 확인합니다.
		const int32 TotalPrice = merchandise.MerchandiseAbilityData.Price * TradeCount;
		if (PlayerCharacter->GetAttributeComponent()->GetEXP() < TotalPrice)
		{
		
			UWidgetHelperLibrary::ShowAlertMsg(PlayerCharacter->GetController<AUserController>(), EAlertMsgType::Error,
			                                  GlobalMerchantText::alertText,FOnButtonClicked());
			return;
		}

		// 갯수를 차감하고 비용을 받습니다.
		if (bIsMerchandise)
		{
			if (MerchantComponent->DecreaseMerchandiseAbilityCount(abilityTag, TradeCount) == false)
			{
				UE_LOGFMT(LogTemp, Error, "상품의 카운트를 줄이는데 실패했습니다.");
				return;
			}
		}
		else
		{
			if (MerchantComponent->DecreaseRepurchaseMerchandiseAbilityCount(abilityTag, TradeCount) == false)
			{
				UE_LOGFMT(LogTemp, Error, "재구매 상품의 카운트를 줄이는데 실패했습니다.");
				return;
			}
		}

		PlayerCharacter->GetAttributeComponent()->SetEXP(
			PlayerCharacter->GetAttributeComponent()->GetEXP() - TotalPrice);
		
		UpdateExpWidget(PlayerCharacter,TotalPrice * -1);

		//어빌리티를 넘깁니다.
		PlayerCharacter->GetAbilityComponent()->GiveAbility(
			merchandise.MerchandiseAbilityData.AbilityObject.LoadSynchronous());
		PlayerCharacter->GetAttributeComponent()->OnCharacterInformationUpdate.Broadcast();
		MerchantComponent->OnSellAbilityToPlayer.Broadcast(PlayerCharacter);
	}
}

void IMerchantInterface::UpdateExpWidget(APlayerCharacter* Player, const int32& AddExp)
{
	if(auto attComp = Player->GetAttributeComponent())
	{
		attComp->OnUpdateExp.Broadcast(AddExp);
	}
}
