// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/06_Enhancement/EnhancementWidget.h"

#include "03_Widget/01_Menu/00_Inventory/ItemListWidget.h"
#include "03_Widget/01_Menu/05_Orb/OrbElementListWidget.h"
#include "03_Widget/06_Enhancement/EnhancementAttributeWidget.h"
#include "03_Widget/06_Enhancement/EnhancementButtonWidget.h"
#include "00_Character/04_NPC/99_Component/EnhancementComponent.h"
#include "03_Widget/05_Alert/AlertWidget.h"
#include "96_Library/ItemHelperLibrary.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "99_Subsystem/ItemManagerSubsystem.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/RichTextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "Components/WrapBox.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "00_Character/00_Player/00_Controller/UserController.h"
#include "03_Widget/01_Menu/00_Inventory/ItemInfoWidget.h"
#include "Components/Border.h"

void UEnhancementWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (UMG_Inventory_Spirit)
	{
		UMG_Inventory_Spirit->HideFilter();
		UMG_Inventory_Spirit->HideCloseButton();
	}

	if (UMG_Inventory_Armor)
	{
		UMG_Inventory_Armor->HideFilter();
		UMG_Inventory_Armor->HideCloseButton();
	}
}

void UEnhancementWidget::NativeConstruct()
{
	bUseAsPopUp = true;
	bRemovable = true;
	Super::NativeConstruct();

	if (UMG_EnhancementButton)
	{
		UMG_EnhancementButton->SetOwnerWidget(this);
		UMG_EnhancementButton->Clear();
	}

	Button_Close->OnClicked.AddUniqueDynamic(this,&UEnhancementWidget::OnClickedCloseButton);
	
	Button_Armor->OnClicked.AddUniqueDynamic(this, &UEnhancementWidget::OnClickedArmor);
	Button_Spirit->OnClicked.AddUniqueDynamic(this, &UEnhancementWidget::OnClickedSpirit);
	Button_Fragment->OnClicked.AddUniqueDynamic(this, &UEnhancementWidget::OnClickedFragment);
	Button_Core->OnClicked.AddUniqueDynamic(this, &UEnhancementWidget::OnClickedCore);
	Button_Upgrade->OnClicked.AddUniqueDynamic(this, &UEnhancementWidget::OnClickedUpgrade);
	Button_Upgrade->SetIsEnabled(false);

	if (VerticalBox_Materials)
	{
		VerticalBox_Materials->SetVisibility(ESlateVisibility::Hidden);
	}

	if (Border_UpgradeMax)
	{
		Border_UpgradeMax->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	if (WidgetSwitcher)
	{
		for(auto iter : WidgetSwitcher->GetAllChildren())
		{

			{
				if(auto widget  = Cast<UOrbElementListWidget>(iter))
				{
					widget->EnhancementSetting(this);
					continue;
				}
			}
			{
				if(auto widget  = Cast<UItemListWidget>(iter))
				{
					widget->EnhancementSetting(this);
					continue;
				}
			}
		}

		if (UMG_OrbElementListCore)
		{
			UMG_OrbElementListCore->CreateCoreListFromInventory();
		}

		if (UMG_OrbElementListFragment)
		{
			UMG_OrbElementListFragment->CreateFragmentListFromInventory();
		}

		if (UMG_Inventory_Spirit)
		{
			UMG_Inventory_Spirit->CreateInventoryItemList();
		}

		if (UMG_Inventory_Armor)
		{
			UMG_Inventory_Armor->CreateInventoryItemList();
		}
		
		WidgetSwitcher->SetActiveWidgetIndex(0);
	}

	

	UGameplayStatics::PlaySound2D(this, OpenSound);
	UWidgetHelperLibrary::ShowTutorialWidget(GetOwningPlayer(),FGameplayTag::RequestGameplayTag("Tutorial.Enhancement"));
}


void UEnhancementWidget::OnClickedArmor()
{
	if (WidgetSwitcher)
	{
		WidgetSwitcher->SetActiveWidgetIndex(3);

		UMG_Inventory_Armor->OnAddListViewWidget.Unbind();
		UMG_Inventory_Armor->OnAddListViewWidget.BindLambda([this](UUserWidget* Widget)
		{
			UMG_Inventory_Armor->HideWidgetIfNotArmor(Widget);
		});
		UMG_Inventory_Armor->CreateItemList_Armor();
	}
}

void UEnhancementWidget::OnClickedSpirit()
{
	if (WidgetSwitcher)
	{
		WidgetSwitcher->SetActiveWidgetIndex(2);

		UMG_Inventory_Spirit->OnAddListViewWidget.Unbind();
		UMG_Inventory_Spirit->OnAddListViewWidget.BindLambda([this](UUserWidget* Widget)
		{
			UMG_Inventory_Spirit->HideWidgetIfNotSpirit(Widget);
		});
		UMG_Inventory_Spirit->CreateItemList_Sprite();
	}
}

void UEnhancementWidget::OnClickedFragment()
{
	if (WidgetSwitcher)
	{
		WidgetSwitcher->SetActiveWidgetIndex(1);
	}
}

void UEnhancementWidget::OnClickedCore()
{
	if (WidgetSwitcher)
	{
		WidgetSwitcher->SetActiveWidgetIndex(0);
	}
}

void UEnhancementWidget::OnClickedUpgrade()
{
	if (EnhancementComponent.IsValid())
	{
		//올릴 능력치 타입을 가져옵니다.
		EAttributeType attributeToIncrease = EAttributeType::NONE;
		for (auto c : CheckBoxes)
		{
			if (c->CheckBox->IsChecked())
			{
				attributeToIncrease = c->SelectedAttribute;
				break;
			}
		}

		if (attributeToIncrease != EAttributeType::NONE)
		{
			//이미 장착중인 아이템에 대한 강화 처리
			auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent();
			bool bReEquipAfterEnhance = false;
			if (invenComp->IsEquipped(SetItem.UniqueID))
			{
				bReEquipAfterEnhance = true;
				//강제 장착 해제
				invenComp->UnEquip(SetItem.UniqueID);
			}


			//강화합니다.
			if (EnhancementComponent->Upgrade(GetOwningPlayerPawn<ABaseCharacter>(), SetItem.UniqueID,
			                                  attributeToIncrease))
			{
				//저장된 아이템 정보 갱신
				//프래그먼트인 경우
				if (invenComp->GetFragments().Contains(SetItem.UniqueID))
				{
					SetItem = invenComp->GetFragments()[SetItem.UniqueID];
				}
				else
				{
					//그 외의 경우
					SetItem = invenComp->GetInventoryItem(SetItem.UniqueID);
				}

				RefreshItemList(SetItem);
				//새로고침 합니다.
				UMG_EnhancementButton->OverrideToolTip(SetItem);
				SetEnhancementInfo(SetItem);

				if (bReEquipAfterEnhance)
				{
					invenComp->UseItem(SetItem.UniqueID);
				}
			}
		}
	}
}

void UEnhancementWidget::OnClickedCloseButton()
{
	UWidgetHelperLibrary::CloseWidgetSetting(GetOwningPlayer(),this);
}

void UEnhancementWidget::CreateEnhancementAttributes(const FInventoryItem& Item)
{
	CheckBoxes.Empty();
	WrapBox_Attribute->ClearChildren();

	if (WrapBox_Attribute && EnhancementComponent.IsValid())
	{
		if (EnhancementAttributeObject)
		{
			auto allowEnhancement = static_cast<const FEquipmentInformation*>(Item.GetItemInformation())->Enhancement.
				GetAllowEnhancement();

			for (auto att : allowEnhancement)
			{
				const auto widget = CreateWidget<UEnhancementAttributeWidget>(
					GetOwningPlayer(), EnhancementAttributeObject);
				widget->SetAttributeType(att);
				widget->SetParent(this);

				CheckBoxes.Add(widget);
				WrapBox_Attribute->AddChildToWrapBox(widget);
			}
		}
	}
}

void UEnhancementWidget::RefreshItemList(const FInventoryItem& Item)
{
	if (auto widget = WidgetSwitcher->GetActiveWidget())
	{
		//0번 코어
		//1번 파편
		if (widget->IsA<UOrbElementListWidget>())
		{
			Cast<UOrbElementListWidget>(widget)->RefreshFromInventoryItem(Item);
			return;
		}
		//2번 무기
		//3번 방어구
		if (widget->IsA<UItemListWidget>())
		{
			Cast<UItemListWidget>(widget)->RefreshFromInventoryItem(Item);
		}
	}
}

void UEnhancementWidget::ForceUnCheckOthers(UEnhancementAttributeWidget* CheckedWidget)
{
	for (auto c : CheckBoxes)
	{
		if (c != CheckedWidget)
		{
			c->CheckBox->SetIsChecked(false);
		}
	}
}

void UEnhancementWidget::SetEnhancementComponent(UEnhancementComponent* E_Component)
{
	EnhancementComponent = E_Component;
	EnhancementComponent->OnUpgradeEquipment.AddUniqueDynamic(this,&UEnhancementWidget::OnUpgradeEquipmentEvent);
}

void UEnhancementWidget::SetEnhancementInfo(const FInventoryItem& Item)
{
	if (EnhancementComponent.IsValid() && UItemHelperLibrary::IsEquipment(Item))
	{
		const auto equip = static_cast<const FEquipmentInformation*>(Item.GetItemInformation());
		const auto& enhancementInfo = Cast<AEquipmentItemActor>(Item.GetSpawndItemActor())->GetEnhancementInfo();
		if (enhancementInfo.CanEnhance())
		{
			Button_Upgrade->SetIsEnabled(true);
			Border_UpgradeMax->SetVisibility(ESlateVisibility::Collapsed);

			SetItem = Item;

			RichTextBlock_Cur->SetText(FText::AsNumber(enhancementInfo.CurEnhancement));
			RichTextBlock_Next->SetText(FText::FromString(
				"<enhancement.next>" + FString::FormatAsNumber(enhancementInfo.CurEnhancement + 1) + "</>"));

			const auto& nextInfo = EnhancementComponent->GetNextEnhancementInfo(enhancementInfo.CurEnhancement);

			if (auto system = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UItemManagerSubsystem>())
			{
				FString msg;
				for (auto iter : nextInfo.NeedItems)
				{
					if (const auto enhancementMaterial = system->GetEnhancementItemInformation(iter.Key))
					{
						msg += UWidgetHelperLibrary::EnhancementTagToDecoString(iter.Key) + enhancementMaterial->
							Item_Name.ToString() + " " + FString::FormatAsNumber(iter.Value) + TEXT("개\n");
					}
				}
				RichTextBlock_Material->SetText(FText::FromString(msg));
			}


			RichTextBlock_NeedSoul->SetText(FText::AsNumber(nextInfo.NeedSouls));
			VerticalBox_Materials->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			CreateEnhancementAttributes(Item);

			if (EnhancementComponent->CanEnhance(GetOwningPlayerPawn<ABaseCharacter>(), Item))
			{
				Button_Upgrade->SetIsEnabled(true);
			}
			else
			{
				Button_Upgrade->SetIsEnabled(false);
				UE_LOGFMT(LogTemp, Error, "{0} {1} : 강화 불가능한 상태입니다.", __FUNCTION__, __LINE__);
			}
		}
		else
		{
			//UWidgetHelperLibrary::ShowAlertMsg(GetOwningPlayer<AUserController>(),EAlertMsgType::Error,TEXT("더 이상 강화할 수 없는 아이템입니다."));
			UE_LOGFMT(LogTemp, Error, "{0} {1} : 더 이상 아이템을 강화할 수 없습니다.", __FUNCTION__, __LINE__);
			Button_Upgrade->SetIsEnabled(false);
			Border_UpgradeMax->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UEnhancementWidget::ShowItemInformation(UInventoryData* Data)
{
	if(UMG_ItemInfo)
	{
		if(auto itemData = Cast<UItemData>(Data))
		{
			UMG_ItemInfo->SetInfo(itemData->InventoryItem);
		}
	}
}

void UEnhancementWidget::ShowItemInformation(UOrbData* Data)
{
	if(UMG_ItemInfo)
	{
		UMG_ItemInfo->SetInfo(Data->Data);
	}
}

void UEnhancementWidget::OnUpgradeEquipmentEvent(const FGuid& ID, AEquipmentItemActor* EquipmentItemActor)
{
	if(auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent())
	{
		if(invenComp->K2_HasItemByID(ID))
		{
			const auto& item = invenComp->GetInventoryItem(ID);

			if(UItemHelperLibrary::IsOrbItem(item))
			{
				auto data = NewObject<UOrbData>();
				data->Data = item;
				data->bIsEquipped = invenComp->IsEquipped(ID);
				ShowItemInformation(data);
				return;
			}

			auto data = NewObject<UItemData>();
			data-> InventoryItem= item;
			ShowItemInformation(data);
		}else
		{
			UKismetSystemLibrary::PrintString(this,TEXT("강화하려는 장비 아이템을 인벤토리에서 찾을 수 없습니다!!"));
		}
	}
}
