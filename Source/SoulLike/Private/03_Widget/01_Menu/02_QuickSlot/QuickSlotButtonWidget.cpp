// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/02_QuickSlot/QuickSlotButtonWidget.h"

#include "00_Character/00_Player/00_Controller/UserController.h"
#include "00_Character/00_Player/00_Controller/00_Component/WidgetManagerComponent.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "02_Ability/AbilityRequirement.h"
#include "03_Widget/MainWidget.h"
#include "03_Widget/QuickSlotWidget.h"
#include "03_Widget/01_Menu/00_Inventory/ItemButtonWidget.h"
#include "03_Widget/07_NPC/00_Bonfire/AbilityButtonWidget.h"
#include "04_Item/ItemActor.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "SoulLike/SoulLike.h"

void UQuickSlotButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if(TextBlock_Key){
		TextBlock_Key->SetText(KeyText);
	}
}

void UQuickSlotButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TextBlock_Count->SetText(FText::GetEmpty());

	if (auto pc = GetOwningPlayer<AUserController>())
	{
		MainQuickSlotWidget = Cast<UMainWidget>(
			pc->GetWidgetManagerComponent()->GetWidget(FGameplayTag::RequestGameplayTag("Widget.Main")))->UMG_QuickSlot;

		InventoryComponent = GetOwningPlayerPawn<APlayerCharacter>()->GetInventoryComponent();
	}
}

void UQuickSlotButtonWidget::ClearSlot()
{
	UE_LOGFMT(LogUMG,Log,"이 슬롯을 클리어 합니다 : {0}",GetName());
		
	InventoryData = nullptr;
	TextBlock_Count->SetText(FText::GetEmpty());
	Image->SetBrushFromSoftTexture(DefaultImage);
	Image->SetColorAndOpacity(FLinearColor(1,1,1,0.75));
	
}











void UItemQuickSlotButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	QuickSlotType = EQuickSlotType::CONSUME;
}

void UItemQuickSlotButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (auto player = GetOwningPlayerPawn<APlayerCharacter>())
	{
		if (QuickSlotType == EQuickSlotType::CONSUME)
		{
			player->GetInventoryComponent()->OnItemQuickSlotUpdate.AddUniqueDynamic(
				this, &UItemQuickSlotButtonWidget::OnRefreshItemSlot);
			player->GetInventoryComponent()->OnRemoveItem.AddUniqueDynamic(
				this, &UItemQuickSlotButtonWidget::OnSlotItemRemoved);
			
		}
	

	}
}

bool UItemQuickSlotButtonWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	if (auto oper = Cast<UDragAndDropOperation>(InOperation))
	{
		if (oper->DraggedWidget->IsA<UItemButtonWidget>())
		{
			UE_LOGFMT(LogUMG, Log, "인벤토리버튼 드롭");
			if (const auto button = Cast<UItemButtonWidget>(oper->DraggedWidget))
			{
				if (auto data = Cast<UItemData>(button->GetInventoryData()))
				{
					//소비아이템만 이 슬롯에 등록할 수 있습니다.
					if (data->InventoryItem.GetItemInformation()->Item_Type ==
						EItemType::CONSUME && QuickSlotType == EQuickSlotType::CONSUME)
					{
						UE_LOGFMT(LogUMG, Log, "인벤토리버튼 정보를 이 퀵슬롯에 등록합니다. : {0}",GetName());
						button->SetEquipped(true);
						//이미 퀵슬롯에 등록된 아이템인지 확인합니다.
						if (IsAlreadyRegistered(data))
						{
							UE_LOGFMT(LogUMG, Log, "이미 다른 퀵슬롯에 등록되어진 아이템을 드롭했습니다.");
							//이미 등록되어있을 경우, 해당 슬롯을 찾아 내용을 지웁니다.
							if(OnRemoveAlreadyRegisteredSlotItem.IsBound())
							{
								const auto& removeIndex = OnRemoveAlreadyRegisteredSlotItem.Execute(data);
								if(removeIndex!=-1){
									InventoryComponent->RemoveQuickSlotItem(data,removeIndex);
								}
							}
						}

						Init(data);
					}
				}
				
			}
		}

		if(oper->DraggedWidget->IsA<UItemQuickSlotButtonWidget>())
		{
			UE_LOGFMT(LogUMG,Log,"다른 아이템 퀵 슬롯 드롭");
			if (const auto button = Cast<UItemQuickSlotButtonWidget>(oper->DraggedWidget))
			{
				if (auto data = button->GetSlotData<UItemData>())
				{
					if(OnRemoveAlreadyRegisteredSlotItem.IsBound())
					{
						const auto& removeIndex = OnRemoveAlreadyRegisteredSlotItem.Execute(data);
						if(removeIndex!=-1){
							InventoryComponent->RemoveQuickSlotItem(data,removeIndex);
						}
					}
					
					//기존 슬롯을 비웁니다.
					button->ClearSlot();
					Init(data);
				}
			}
		}

		oper->DraggedWidget->SetRenderOpacity(oper->OriginalOpacity);
		oper->DraggedWidget->SetVisibility(ESlateVisibility::Visible);

		return true;
	}

	return false;
}

FReply UItemQuickSlotButtonWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	//우클릭시 이 퀵슬롯 초기화.
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		UE_LOGFMT(LogUMG, Warning, "버튼 초기화 : {0}", GetName());
		ClearSlot();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UItemQuickSlotButtonWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UItemQuickSlotButtonWidget::OnRefreshItemSlot(const FGuid& ItemUniqueID, const int32& NewCount)
{
	if(InventoryData!=nullptr)
	{
		if(auto itemData = Cast<UItemData>(InventoryData))
		{
			if(itemData->InventoryItem.UniqueID != ItemUniqueID)
			{
				return;
			}

			TextBlock_Count->SetText(FText::AsNumber(NewCount));
		}
	}
}

void UItemQuickSlotButtonWidget::OnSlotItemRemoved(ABaseCharacter* UsedBy, const FGuid& RemoveItemUniqueID)
{
	if(InventoryData!=nullptr)
	{
		if(auto itemData = Cast<UItemData>(InventoryData))
		{
			if(itemData->InventoryItem.UniqueID != RemoveItemUniqueID)
			{
				return;
			}

			ClearSlot();
		}
	}
}

bool UItemQuickSlotButtonWidget::IsAlreadyRegistered(UItemData* Data) const
{
	if(Data->IsValidLowLevel())
	{
		if(auto player = GetOwningPlayerPawn<APlayerCharacter>())
		{
			return player->GetInventoryComponent()->IsRegistered(Data->InventoryItem.UniqueID);
		}
	}

	return true;
}

void UItemQuickSlotButtonWidget::Init(UInventoryData* Data, bool bIsLoaded)
{
	if (Data->IsValidLowLevel())
	{
		if (Data->IsA<UItemData>())
		{
			UE_LOGFMT(LogUMG,Log,"퀵슬롯 버튼을 초기화 합니다 : {0}",GetName());
			InventoryData = Data;
			auto player = GetOwningPlayerPawn<APlayerCharacter>();
			auto itemData = Cast<UItemData>(InventoryData);
			
			TextBlock_Count->SetText(FText::AsNumber(itemData->InventoryItem.ItemCount));
			Image->SetBrushFromSoftTexture(itemData->InventoryItem.GetItemInformation()->Item_Image);
			Image->SetColorAndOpacity(FLinearColor(1,1,1,1));

			if(!bIsLoaded)
			{
				if (InventoryComponent)
				{
					InventoryComponent->AddQuickSlotItem(Cast<UItemData>(InventoryData), Index);
					OnRegisterItem.Broadcast(player, itemData, Index);
				}
			}
		}
	}else
	{
		ClearSlot();
	}
}

void UItemQuickSlotButtonWidget::UseQuickSlot(UInputAction* InputAction)
{
	UE_LOGFMT(LogUMG, Log, "퀵슬롯 사용 요청 {0}", GetName());
	if (InventoryData != nullptr)
	{
		if (auto player = GetOwningPlayerPawn<APlayerCharacter>())
		{
			player->GetInventoryComponent()->UseItem(Cast<UItemData>(InventoryData)->InventoryItem.UniqueID);
		}
	}
}

void UItemQuickSlotButtonWidget::ClearSlot()
{
	if(InventoryComponent)
	{
		InventoryComponent->RemoveQuickSlotItem(Cast<UItemData>(InventoryData),Index);
	}
	
	Super::ClearSlot();
}

void UItemQuickSlotButtonWidget::RestoreSlotFromUniqueID(const FGuid& ItemID)
{
	if(InventoryComponent && InventoryComponent->IsItemContains(ItemID)){
		
		const auto& item = InventoryComponent->GetInventoryItem(ItemID);
		if (auto newData = NewObject<UItemData>())
		{
			newData->InventoryItem = item;
			Init(newData, true);
		}
	}
}


void UMainItemQuickSlotButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(InventoryComponent == nullptr)
	{
		InventoryComponent = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent();
	}

	if(InventoryComponent)
	{
		InventoryComponent->OnFirstUpdateMainItemQuickSlot.AddUniqueDynamic(this,&UMainItemQuickSlotButtonWidget::OnFirstUpdateMainItemQuickSlotEvent);
		InventoryComponent->OnChangeItemQuickSlot.AddUniqueDynamic(this,&UMainItemQuickSlotButtonWidget::OnChangeItemQuickSlotEvent);
		InventoryComponent->OnItemQuickSlotUpdate.AddUniqueDynamic(this,&UMainItemQuickSlotButtonWidget::OnItemQuickSlotUpdateEvent);
		InventoryComponent->OnRemoveItemQuickSlot.AddUniqueDynamic(this,&UMainItemQuickSlotButtonWidget::OnRemoveItemQuickSlotEvent);
	}

	/*if(auto abComp = GetOwningPlayerPawn<ABaseCharacter>()->GetAbilityComponent())
	{
		abComp->OnChangeAbilityQuickSlot.AddUniqueDynamic(this,&UMainItem)
	}*/
}

void UMainItemQuickSlotButtonWidget::OnFirstUpdateMainItemQuickSlotEvent(const UItemData* Data, int32 SlotIndex)
{
	if(Data)
	{
		UE_LOGFMT(LogUMG,Log,"메인 퀵슬롯에 해당 아이템을 처음 등록합니다.");
		MemorisedUniqueID = Data->InventoryItem.UniqueID;
		TextBlock_Count->SetText(FText::AsNumber(Data->InventoryItem.ItemCount));
		Image->SetBrushFromSoftTexture(Data->InventoryItem.GetItemInformation()->Item_Image);
	}
}

void UMainItemQuickSlotButtonWidget::OnChangeItemQuickSlotEvent(const FInventoryItem& Item, int32 SlotIndex)
{
	if(auto itemData = Item.GetItemInformation())
	{
		MemorisedUniqueID = Item.UniqueID;
		TextBlock_Count->SetText(FText::AsNumber(Item.ItemCount));
		Image->SetBrushFromSoftTexture(itemData->Item_Image);
	}
}

void UMainItemQuickSlotButtonWidget::OnItemQuickSlotUpdateEvent(const FGuid& ItemUniqueID, const int32& NewCount)
{
	if(MemorisedUniqueID == ItemUniqueID)
	{
		TextBlock_Count->SetText(FText::AsNumber(NewCount));
	}
}

void UMainItemQuickSlotButtonWidget::OnRemoveItemQuickSlotEvent(const UItemData* Data, int32 SlotIndex)
{
	if(MemorisedUniqueID == Data->InventoryItem.UniqueID)
	{
		ClearSlot();
	}
}

void UMainItemQuickSlotButtonWidget::ClearSlot()
{
	Super::ClearSlot();

	MemorisedUniqueID = FGuid();
}





void UMainAbilityQuickSlotButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(AbilityComponent == nullptr)
	{
		AbilityComponent = GetOwningPlayerPawn<ABaseCharacter>()->GetAbilityComponent();
	}

	if(AbilityComponent)
	{
		AbilityComponent->OnFirstUpdateMainAbilityQuickSlot.AddUniqueDynamic(this,&UMainAbilityQuickSlotButtonWidget::OnFirstUpdateMainAbilityQuickSlotEvent);
		AbilityComponent->OnChangeAbilityQuickSlot.AddUniqueDynamic(this,&UMainAbilityQuickSlotButtonWidget::OnChangeAbilityQuickSlotEvent);
	}
}

void UMainAbilityQuickSlotButtonWidget::OnFirstUpdateMainAbilityQuickSlotEvent(
	const FAbilityInformation& AbilityInformation)
{
	UE_LOGFMT(LogUMG,Log,"메인 퀵슬롯에 해당 아이템을 처음 등록합니다.");
	MemorisedTag = AbilityInformation.AbilityTag;
	Image->SetBrushFromSoftTexture(AbilityInformation.AbilityImage);
}

void UMainAbilityQuickSlotButtonWidget::OnChangeAbilityQuickSlotEvent(const FAbilityInformation& AbilityInformation)
{

	if(!AbilityInformation.AbilityTag.IsValid())
	{
		ClearSlot();
		return;
	}
	
	MemorisedTag = AbilityInformation.AbilityTag;
	Image->SetBrushFromSoftTexture(AbilityInformation.AbilityImage);
}

void UMainAbilityQuickSlotButtonWidget::ClearSlot()
{
	Super::ClearSlot();

	MemorisedTag = FGameplayTag::EmptyTag;
}






void UAbilityQuickSlotButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	QuickSlotType = EQuickSlotType::ABILITY;
}

void UAbilityQuickSlotButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}

bool UAbilityQuickSlotButtonWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	//UAbilityButtonWidget에 대해서 처리해야 합니다.
	if (auto oper = Cast<UDragAndDropOperation>(InOperation))
	{
		if(oper->DraggedWidget->IsA<UAbilityButtonWidget>())
		{
			if (const auto button = Cast<UAbilityButtonWidget>(oper->DraggedWidget))
			{
				if(button->GetAbilityData().IsAttributeEnough(GetOwningPlayerPawn()))
				{
					UE_LOGFMT(LogUMG,Log,"어빌리티 버튼을 드롭했습니다.");
					button->SetEquipped(true);
				
					//이미 등록되어있을 경우, 해당 슬롯을 찾아 내용을 지웁니다.
					if (IsAlreadyRegistered(button->GetAbilityData()))
					{
						UE_LOGFMT(LogUMG, Log, "이미 다른 퀵슬롯에 등록되어진 아이템을 드롭했습니다.");
						OnAlreadyRegisteredAbilityDropped.Broadcast(button->GetAbilityData().AbilityTag);
					}
					//어빌리티 퀵슬롯에서 비어있지 않은 가장 첫 슬롯을 찾아 할당해야 합니다.
					OnDroppedAbility.Broadcast(button->GetAbilityData());
				}else
				{
					FText msg = FText::FromString(TEXT("이 기술을 사용하기 위한 능력치가 모자랍니다."));
					UWidgetHelperLibrary::ShowAlertMsg(GetOwningPlayer<AUserController>(),EAlertMsgType::Warning,msg,FOnButtonClicked());
				}
			}
		}

		oper->DraggedWidget->SetRenderOpacity(oper->OriginalOpacity);
		oper->DraggedWidget->SetVisibility(ESlateVisibility::Visible);

		return true;
	}


	return false;
}

FReply UAbilityQuickSlotButtonWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	//우클릭시 이 퀵슬롯 초기화.
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		UE_LOGFMT(LogUMG, Warning, "버튼 초기화 : {0}", GetName());
		SlotClear(true);
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UAbilityQuickSlotButtonWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UAbilityQuickSlotButtonWidget::OnSlotAbilityRemoved(ABaseCharacter* UsedBy, const FGameplayTag& AbilityTag)
{
}

bool UAbilityQuickSlotButtonWidget::IsAlreadyRegistered(UAbilityData* Data) const
{
	if(Data->IsValidLowLevel())
	{
		if(auto player = GetOwningPlayerPawn<APlayerCharacter>())
		{
			return player->GetAbilityComponent()->IsRegistered(Data->AbilityInformation.AbilityTag);
		}
	}

	return true;
}

bool UAbilityQuickSlotButtonWidget::IsAlreadyRegistered(const FAbilityInformation& Data) const
{
	if(auto player = GetOwningPlayerPawn<APlayerCharacter>())
	{
		return player->GetAbilityComponent()->IsRegistered(Data.AbilityTag);
	}
	return true;
}

void UAbilityQuickSlotButtonWidget::Init(UInventoryData* Data, bool bIsLoaded)
{
	if (Data->IsValidLowLevel())
	{
		if (Data->IsA<UAbilityData>())
		{
			UE_LOGFMT(LogUMG,Log,"퀵슬롯 버튼을 초기화 합니다 : {0}",GetName());
			InventoryData = Data;
			auto player = GetOwningPlayerPawn<APlayerCharacter>();
			auto abilityData = Cast<UAbilityData>(InventoryData);
			
			TextBlock_Count->SetText(FText::GetEmpty());
			Image->SetBrushFromSoftTexture(abilityData->AbilityInformation.AbilityImage);
			Image->SetColorAndOpacity(FLinearColor(1,1,1,1));
			
			if(abilityData->AbilityInformation.AbilityTag.IsValid()){
				UWidgetHelperLibrary::SetToolTipWidget(this,FText::Format(FText::FromString("{0}\n\n{1}\n\n\n{2}"),abilityData->AbilityInformation.AbilityName,abilityData->AbilityInformation.AbilityDescription,abilityData->AbilityInformation.GetAbilityReqDescription()));
			}
			return;
		}
	}

	ClearSlot();
	
}

void UAbilityQuickSlotButtonWidget::SlotClear(bool bCallEvent)
{
	if(bCallEvent)
	{
		if(auto data = GetSlotData<UAbilityData>())
		{
			if(InventoryData)
			{
				if(InventoryData->OwnItemButtonWidget!=nullptr)
				{
					Cast<UAbilityButtonWidget>(InventoryData->OwnItemButtonWidget)->SetEquipped(false);
				}
			}
			
            	
			OnClearSlot.Broadcast(data->AbilityInformation.AbilityTag);
		}
	}else
	{
		ClearSlot();
	}
}

void UAbilityQuickSlotButtonWidget::UseQuickSlot(UInputAction* InputAction)
{
	Super::UseQuickSlot(InputAction);
}

void UAbilityQuickSlotButtonWidget::ClearSlot()
{
	Super::ClearSlot();
}

void UAbilityQuickSlotButtonWidget::OverrideData(UAbilityData* AbilityData)
{
	UE_LOGFMT(LogUMG,Log,"이 슬롯 데이터를 덮어씁니다 : {0}",GetName());
	Init(AbilityData);
}
