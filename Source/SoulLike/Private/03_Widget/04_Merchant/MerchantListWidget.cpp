// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/04_Merchant/MerchantListWidget.h"

#include "00_Character/01_Component/AttributeComponent.h"
#include "00_Character/04_NPC/NPCBase.h"
#include "03_Widget/01_Menu/00_Inventory/ItemButtonWidget.h"
#include "03_Widget/04_Merchant/MerchandiseCheckWidget.h"
#include "03_Widget/04_Merchant/MerchantWidget.h"
#include "03_Widget/99_Drag/DragAndDropOperation.h"
#include "03_Widget/99_Drag/DraggableWidget.h"
#include "97_Interface/00_NPC/MerchantInterface.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Logging/StructuredLog.h"


void UMerchantListWidget::CreateMerchandiseList(UMerchantComponent* MerchantComponent)
{
	ListView_Purchase->ClearListItems();

	MerchantNPC = MerchantComponent->GetOwner<ANPCBase>();
	
	const TMap<FGuid, FMerchandiseItem>& merchandiseItem = MerchantComponent->GetMerchandiseItem();
	for (auto iter : merchandiseItem)
	{
		if (auto data = NewObject<UMerchandiseItemListData>())
		{
			data->MerchandiseItem = iter.Value;
			data->MerchantListWidget = this;
			UE_LOGFMT(LogTemp, Log, "상점 아이템 추가 : {0}, {1}", iter.Value.MerchandiseData.Tag.ToString(), data->MerchandiseItem.GetItemInformation()->Item_Image.LoadSynchronous()->GetName());

			MerchantData.Add(data);
			ListView_Purchase->AddItem(data);
		}
	}

	const auto& merchandiseAbility = MerchantComponent->GetMerchandiseAbility();
	for (auto iter : merchandiseAbility)
	{
		if (auto data = NewObject<UMerchandiseAbilityListData>())
		{
			data->MerchandiseAbility = iter.Value;
			data->MerchantListWidget = this;
			UE_LOGFMT(LogTemp, Log, "상점 어빌리티 추가 : {0} {1}", iter.Value.MerchandiseAbilityData.Tag.ToString(),iter.Value.GetAbilityReqDescription().ToString());

			MerchantData.Add(data);
			ListView_Purchase->AddItem(data);
		}
	}
}

void UMerchantListWidget::OnClickedRepurchaseButton()
{
	if (Switcher)
	{
		if (ShopType == EShopType::Purchase)
		{
			ShopType = EShopType::Repurchase;
			Switcher->SetActiveWidgetIndex(1);
			TextBlock_ButtonText->SetText(GlobalMerchandiseCheckWidgetText::buyText);

			CreateRepurchaseList();
		}
		else
		{
			ShopType = EShopType::Purchase;
			Switcher->SetActiveWidgetIndex(0);
			TextBlock_ButtonText->SetText(GlobalMerchandiseCheckWidgetText::repurchaseText);
		}
	}
}

void UMerchantListWidget::CreateRepurchaseList()
{
	UE_LOGFMT(LogTemp, Log, "UMerchantListWidget::CreateRepurchaseList");
	ListView_Repurchase->ClearListItems();
	if (MerchantNPC.IsValid())
	{
		{
			const auto& items = MerchantNPC->GetMerchantComponent()->
				GetRepurchaseItems();
			for (auto iter : items)
			{
				auto data = NewObject<UMerchandiseItemListData>();
				data->MerchandiseItem = iter.Value;

				UE_LOGFMT(LogTemp, Log, "재구매 리스트에 아이템 추가 : {0}", iter.Value.MerchandiseData.Tag.ToString());

				MerchantRepurchaseData.Add(data);
				ListView_Repurchase->AddItem(data);
			}
		}

		{
			const auto& abilities = MerchantNPC->GetMerchantComponent()->
				GetRepurchaseAbility();
			for (auto iter : abilities)
			{
				auto data = NewObject<UMerchandiseAbilityListData>();
				data->MerchandiseAbility = iter.Value;

				UE_LOGFMT(LogTemp, Log, "재구매 리스트에 어빌리티 추가 : {0}", iter.Value.MerchandiseAbilityData.Tag.ToString());

				MerchantRepurchaseData.Add(data);
				ListView_Repurchase->AddItem(data);
			}
		}
	}
}

void UMerchantListWidget::UpdatePlayerExp(APlayerCharacter* Player)
{
	TextBlock_Souls->SetText(FText::AsNumber(Player->GetAttributeComponent()->GetEXP()));
}

bool UMerchantListWidget::PlayerSellItemOrAbility(UInventoryData* InventoryData)
{
	if (InventoryData->IsA<UItemData>())
	{
		UE_LOGFMT(LogTemp, Log, "드롭된 아이템 버튼 정보 : {0}",
		          Cast<UItemData>(InventoryData)->InventoryItem.GetItemInformation()->Item_Tag.ToString());
		OnPlayerSellItem.Broadcast(Cast<UItemData>(InventoryData)->InventoryItem);
		return true;
	}
	
	return false;
}

void UMerchantListWidget::UpdateRepurchaseList()
{
	CreateRepurchaseList();
}

void UMerchantListWidget::OnBuyAbilityFromPlayerEvent(APlayerCharacter* Player)
{
	UpdatePlayerExp(Player);
}

void UMerchantListWidget::OnBuyItemFromPlayerEvent(APlayerCharacter* Player, const FGuid& ItemID)
{
	UpdatePlayerExp(Player);
}

void UMerchantListWidget::OnSellItemToPlayerEvent(APlayerCharacter* Player, const FGuid& ItemID)
{
	UpdatePlayerExp(Player);
	UpdateRepurchaseList();
}

void UMerchantListWidget::OnSellAbilityToPlayerEvent(APlayerCharacter* Player)
{
	UpdatePlayerExp(Player);
}


void UMerchantListWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (Button_Repurchase)
	{
		Button_Repurchase->OnClicked.AddUniqueDynamic(this, &UMerchantListWidget::OnClickedRepurchaseButton);
	}

	UpdatePlayerExp(GetOwningPlayerPawn<APlayerCharacter>());
}

bool UMerchantListWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                       UDragDropOperation* InOperation)
{
	if (ShopType == EShopType::Purchase)
	{
		if (const auto oper = Cast<UDragAndDropOperation>(InOperation))
		{
			oper->DraggedWidget->SetRenderOpacity(oper->OriginalOpacity);
			oper->DraggedWidget->SetVisibility(ESlateVisibility::Visible);
			if (oper->DraggedWidget->IsA<UItemButtonWidget>())
			{
				const auto button = Cast<UItemButtonWidget>(oper->DraggedWidget);
				const auto data = button->GetInventoryData();

				return PlayerSellItemOrAbility(data);

				/*
				if (data->IsA<UItemData>())
				{
					UE_LOGFMT(LogTemp, Log, "드롭된 아이템 버튼 정보 : {0}",
					          Cast<UItemData>(data)->InventoryItem.GetItemInformation()->Item_Tag.ToString());
					OnPlayerSellItem.Broadcast(Cast<UItemData>(data)->InventoryItem);
					return true;
				}

				if (data->IsA<UAbilityData>())
				{
					UE_LOGFMT(LogTemp, Log, "드롭된 어빌리티 버튼 정보 : {0}",
					          Cast<UAbilityData>(data)->AbilityInformation.AbilityTag.ToString());
					OnPlayerSellAbility.Broadcast(Cast<UAbilityData>(data)->AbilityInformation);
					return true;
				}*/
			}
		}
	}

	return false;
}
