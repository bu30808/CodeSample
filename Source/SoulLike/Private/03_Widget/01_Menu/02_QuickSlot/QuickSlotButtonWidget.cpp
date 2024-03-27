// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/02_QuickSlot/QuickSlotButtonWidget.h"

#include "00_Character/00_Player/00_Controller/UserController.h"
#include "00_Character/00_Player/00_Controller/00_Component/WidgetManagerComponent.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "03_Widget/MainWidget.h"
#include "03_Widget/QuickSlotWidget.h"
#include "03_Widget/01_Menu/00_Inventory/ItemButtonWidget.h"
#include "04_Item/ItemActor.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"


void UQuickSlotButtonWidget::LinkToMainWidgetSlot(bool bClear)
{

	if(InventoryData==nullptr)
	{
		return;
	}
	
	//메인 위젯의 퀵슬롯과 연동해야 합니다.
	if(auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent())
	{
		switch(QuickSlotType)
		{
		case EQuickSlotType::CONSUME:
			if(bClear)
			{
				invenComp->RemoveQuickSlotItem(InventoryData,Index);
			}else
			{
				invenComp->AddQuickSlotItem(InventoryData,Index);
			}
			break;
		case EQuickSlotType::ABILITY:
			if(bClear)
			{
				invenComp->RemoveQuickSlotAbility(InventoryData,Index);
			}else
			{
				invenComp->AddQuickSlotAbility(InventoryData,Index);
			}
			break;
		}
	}

}

void UQuickSlotButtonWidget::SetQuickSlotType(EQuickSlotType NewType)
{
	QuickSlotType = NewType;
}

void UQuickSlotButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	TextBlock_Key->SetText(KeyText);
}

void UQuickSlotButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TextBlock_Count->SetText(FText::GetEmpty());

	if (auto pc = GetOwningPlayer<AUserController>())
	{
		MainQuickSlotWidget = Cast<UMainWidget>(
			pc->GetWidgetManagerComponent()->GetWidget(FGameplayTag::RequestGameplayTag("Widget.Main")))->UMG_QuickSlot;
	}

	if (auto player = GetOwningPlayerPawn<APlayerCharacter>())
	{
		if (QuickSlotType == EQuickSlotType::CONSUME)
		{
			player->GetInventoryComponent()->OnItemQuickSlotUpdate.AddUniqueDynamic(
				this, &UQuickSlotButtonWidget::OnRefreshItemSlot);
			player->GetInventoryComponent()->OnRemoveItem.AddUniqueDynamic(
				this, &UQuickSlotButtonWidget::OnSlotItemRemoved);
		}
		else
		{
			player->GetAbilityComponent()->OnRemoveAbility.AddUniqueDynamic(
				this, &UQuickSlotButtonWidget::OnRemoveAbilityEvent);
		}
		
		if(auto instance = Cast<USoulLikeInstance>(UGameplayStatics::GetGameInstance(player)))
		{
			OnRegisterItemOrAbility.AddUniqueDynamic(instance,&USoulLikeInstance::OnRegisterQuickSlotItemOrAbility);
			OnUnRegisterItemOrAbility.AddUniqueDynamic(instance,&USoulLikeInstance::OnUnRegisterQuickSlotItemOrAbility);
		}
	}

}

void UQuickSlotButtonWidget::Init(UInventoryData* Data)
{
	if (Data->IsValidLowLevel())
	{
		InventoryData = Data;
		OnRegisterItemOrAbility.Broadcast(GetOwningPlayerPawn<APlayerCharacter>(),InventoryData,Index);
		
	
		if (Data->IsA<UItemData>())
		{
			auto item = Cast<UItemData>(Data);
			//UKismetSystemLibrary::PrintString(this,TEXT("퀵슬롯 초기화 : ")+item->InventoryItem.GetItemInformation()->Item_Name);
			TextBlock_Count->SetText(FText::AsNumber(item->InventoryItem.ItemCount));
			Image->SetBrushFromSoftTexture(item->InventoryItem.GetItemInformation()->Item_Image);
		}

		if (Data->IsA<UAbilityData>())
		{
			TextBlock_Count->SetText(FText::GetEmpty());
			Image->SetBrushFromSoftTexture(Cast<UAbilityData>(Data)->AbilityInformation.AbilityImage);
		}
		
	}else
	{
		UKismetSystemLibrary::PrintString(this,TEXT("가져온 데이터가 유효하지 않아 이 버튼정보를 초기화 할 수 없습니다."));
	}
}

void UQuickSlotButtonWidget::Clean()
{
	LinkToMainWidgetSlot(true);
	OnUnRegisterItemOrAbility.Broadcast(GetOwningPlayerPawn<APlayerCharacter>(),InventoryData,Index);
	Image->SetBrushFromSoftTexture(DefaultImage);
	TextBlock_Count->SetText(FText::GetEmpty());
	InventoryData = nullptr;
}

void UQuickSlotButtonWidget::CleanMainQuickSlotButton()
{
	Image->SetBrushFromSoftTexture(DefaultImage);
	TextBlock_Count->SetText(FText::GetEmpty());
	InventoryData = nullptr;
}

bool UQuickSlotButtonWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                          UDragDropOperation* InOperation)
{
	if (auto oper = Cast<UDragAndDropOperation>(InOperation))
	{
		
		if (oper->DraggedWidget->IsA<UQuickSlotButtonWidget>())
		{
			if(oper->DraggedWidget != this)
			{
				UE_LOGFMT(LogTemp, Log, "퀵 드롭");
				const auto quick = Cast<UQuickSlotButtonWidget>(oper->DraggedWidget);
				OnDropOtherQuickSlot(quick);
			}else
			{
				UKismetSystemLibrary::PrintString(this,TEXT("같은 슬롯 드롭됨."));
			}
		}

		if (oper->DraggedWidget->IsA<UItemButtonWidget>())
		{
			UE_LOGFMT(LogTemp, Log, "인벤토리버튼 드롭");
			if (const auto button = Cast<UItemButtonWidget>(oper->DraggedWidget))
			{
				button->SetEquipped(true);
				OnDropConsumeItem(button);
				OnDropAbility(button);
			}
		}

		oper->DraggedWidget->SetRenderOpacity(oper->OriginalOpacity);
		oper->DraggedWidget->SetVisibility(ESlateVisibility::Visible);
		
		return true;
	}

	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

FReply UQuickSlotButtonWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//우클릭시 이 퀵슬롯 초기화.
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		UE_LOGFMT(LogTemp, Warning, "버튼 초기화 : {0}", Index);
		Clean();
	}
	
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UQuickSlotButtonWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UQuickSlotButtonWidget::SetIndex(int32 NewIndex)
{
	Index = NewIndex;
}

void UQuickSlotButtonWidget::OnDropOtherQuickSlot(UQuickSlotButtonWidget* OtherQuickSlotButtonWidget)
{

	if (OtherQuickSlotButtonWidget && QuickSlotType == OtherQuickSlotButtonWidget->QuickSlotType)
	{
		if(auto data = OtherQuickSlotButtonWidget->InventoryData.Get())
		{
		
			//새 정보로 이 슬롯을 덮어씁니다.
			Init(data);
			//이전 슬롯을 비웁니다.
			OtherQuickSlotButtonWidget->Clean();
			
			LinkToMainWidgetSlot();
		}
	}
}

void UQuickSlotButtonWidget::OnDropConsumeItem(UItemButtonWidget* InventoryButtonWidget)
{
	if (InventoryButtonWidget)
	{
		if (InventoryButtonWidget->GetInventoryData()->IsA<UItemData>())
		{
			if(auto data =Cast<UItemData>( InventoryButtonWidget->GetInventoryData()))
			{
				//소비아이템만 이 슬롯에 등록할 수 있습니다.
				if (data->InventoryItem.GetItemInformation()->Item_Type ==
					EItemType::CONSUME && QuickSlotType == EQuickSlotType::CONSUME)
				{
					//이미 퀵슬롯에 등록된 아이템인지 확인합니다.
					if(IsAlreadyRegistered(data))
					{
						//이미 등록되어있을 경우, 해당 슬롯을 찾아 내용을 지웁니다.
						OnRemoveAlreadyRegisteredSlot.Broadcast(data);
					}
					
					Init(data);
					LinkToMainWidgetSlot();
				}
			}
		}
	}
}

void UQuickSlotButtonWidget::OnDropAbility(UItemButtonWidget* InventoryButtonWidget)
{
	if (InventoryButtonWidget)
	{
		if (QuickSlotType == EQuickSlotType::ABILITY && InventoryButtonWidget->GetInventoryData()->IsA<UAbilityData>())
		{
			if(auto data = Cast<UAbilityData>(InventoryButtonWidget->GetInventoryData()))
			{
				//이미 퀵슬롯에 등록된 아이템인지 확인합니다.
				if(IsAlreadyRegistered(data))
				{
					//이미 등록되어있을 경우, 해당 슬롯을 찾아 내용을 지웁니다.
					OnRemoveAlreadyRegisteredSlot.Broadcast(data);
				}
				
				Init(data);
				LinkToMainWidgetSlot();
			}
		}
	}
}

bool UQuickSlotButtonWidget::IsAlreadyRegistered(UItemData* Data) const
{
	if(auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent())
	{
		return invenComp->IsRegistered(Data->InventoryItem.UniqueID);
	}
	return false;
}

bool UQuickSlotButtonWidget::IsAlreadyRegistered(UAbilityData* Data) const
{
	if(auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent())
	{
		return invenComp->IsRegistered(Data->AbilityInformation.AbilityTag);
	}
	return false;
}

void UQuickSlotButtonWidget::UseQuickSlot(UInputAction* InputAction)
{
	UE_LOGFMT(LogTemp, Log, "Call Use Quick Slot {0}", Index+1);
	if (InventoryData != nullptr)
	{
		if (auto player = GetOwningPlayerPawn<APlayerCharacter>())
		{
			switch (QuickSlotType)
			{
			case EQuickSlotType::CONSUME:
				player->GetInventoryComponent()->UseItem(Cast<UItemData>(InventoryData)->InventoryItem.UniqueID);
				break;
			case EQuickSlotType::ABILITY:
				player->GetAbilityComponent()->ActivateAbility(
					Cast<UAbilityData>(InventoryData)->AbilityInformation.AbilityTag, player, InputAction);
				break;
			default: ;
			}
		}
	}
}


void UQuickSlotButtonWidget::OnRefreshItemSlot(const FGuid& ItemUniqueID, const int32& NewCount)
{
	//UE_LOGFMT(LogTemp,Log,"퀵슬롯을 새로 고칩니다");
	if (InventoryData != nullptr)
	{
		if(Cast<UItemData>(InventoryData)->InventoryItem.UniqueID == ItemUniqueID)
		{
			if(NewCount <= 0)
			{
				UE_LOGFMT(LogTemp,Log,"퀵슬롯에 업데이트 되는 대상의 카운트가 0 이하입니다.");
				//아이템 갯수가 0이하인 경우
				//업데이트 된 아이템 정보가 인벤토리 내부에 더 이상 남아있지 않은지 확인합니다.
				if (auto invenComp = GetOwningPlayerPawn<APlayerCharacter>()->GetInventoryComponent())
				{
					if(!invenComp->K2_HasItemByID(ItemUniqueID))
					{
						UE_LOGFMT(LogTemp,Log,"아이템 정보가 인벤토리에서 제거됐으니 슬롯 정보를 초기화 합니다.");
						Clean();
						return;
					}
					
				}
			}
			UE_LOGFMT(LogTemp,Log,"퀵슬롯을 새로 고칩니다 : 업데이트된 갯수 : {0}",NewCount);
				Cast<UItemData>(InventoryData)->InventoryItem.ItemCount = NewCount;
				Init(InventoryData.Get());
			
		}
	}else
	{
		//UE_LOGFMT(LogTemp,Log,"퀵슬롯을 새로 고치려 했으나, 해당 슬롯의 데이터가 비었습니다.");
	}
}


void UQuickSlotButtonWidget::OnSlotItemRemoved(ABaseCharacter* Player, const FGuid& ItemUniqueID)
{
	if (InventoryData != nullptr)
	{
		if (Cast<UItemData>(InventoryData)->InventoryItem.UniqueID == ItemUniqueID)
		{
			//갯수를 0개로 표시합니다.
			TextBlock_Count->SetText(FText::AsNumber(0));
		}
	}
}

void UQuickSlotButtonWidget::OnRemoveAbilityEvent(const FGameplayTag& AbilityTag)
{
	if (InventoryData !=nullptr)
	{
		if (Cast<UAbilityData>(InventoryData)->AbilityInformation.AbilityTag.MatchesTagExact(AbilityTag))
		{
			Clean();
		}
	}
}

FGuid UQuickSlotButtonWidget::GetSlotItemUniqueID()
{
	if(InventoryData !=nullptr)
	{
		if(InventoryData->IsA<UItemData>())
		{
			return Cast<UItemData>(InventoryData)->InventoryItem.UniqueID;
		}
	}
	return FGuid();
}

FGameplayTag UQuickSlotButtonWidget::GetSlotAbilityTag()
{
	if(InventoryData != nullptr)
	{
		if(InventoryData->IsA<UAbilityData>())
		{
			return Cast<UAbilityData>(InventoryData)->AbilityInformation.AbilityTag;
		}
	}
	return FGameplayTag::EmptyTag;
}

void UQuickSlotButtonWidget::RestoreSlotFromItemGUID(const FGuid& ItemID)
{
	if(auto invenComp = GetOwningPlayerPawn<APlayerCharacter>()->GetInventoryComponent())
	{
		if(invenComp->IsItemContains(ItemID))
		{
			const auto item = invenComp->GetInventoryItem(ItemID);
			if(auto newData = NewObject<UItemData>())
			{
				newData->InventoryItem = item;
				Init(newData);
				LinkToMainWidgetSlot();
			}

		}
	}
}

void UQuickSlotButtonWidget::RestoreSlotFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if(auto abComp = GetOwningPlayerPawn<APlayerCharacter>()->GetAbilityComponent())
	{
		if(abComp->HasAbility(AbilityTag))
		{
			if(auto newData = NewObject<UAbilityData>())
			{
				newData->AbilityInformation = abComp->GetAbilityByTag(AbilityTag)->GetAbilityInformation();
				Init(newData);
				LinkToMainWidgetSlot();
			}
		}		
	}
}
