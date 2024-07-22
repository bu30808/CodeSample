// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/07_NPC/00_Bonfire/RegisterAbilityWidget.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/01_Component/AttributeComponent.h"

#include "03_Widget/01_Menu/02_QuickSlot/QuickSlotButtonWidget.h"
#include "03_Widget/07_NPC/00_Bonfire/AbilityButtonWidget.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"


void URegisterAbilityWidget::CreateAbilityList()
{
	WrapBox_AbilityList->ClearChildren();
	if (auto pawn = GetOwningPlayerPawn<ABaseCharacter>())
	{
		if (auto abComp = pawn->GetAbilityComponent())
		{
			const auto& abs = abComp->GetAvailableAbilities();
			for (const auto iter : abs)
			{
				if (iter->GetAbilityInformation().bRegisterInventory)
				{
					WrapBox_AbilityList->AddChildToWrapBox(CreateAbilityButton(iter->GetAbilityInformation()));
				}
			}
		}
	}
}

void URegisterAbilityWidget::CreateMemorySlot()
{
	WrapBox_MemorySlot->ClearChildren();
	if (auto pawn = GetOwningPlayerPawn<ABaseCharacter>())
	{
		if (auto attComp = pawn->GetAttributeComponent())
		{
			const auto& memorySlot = attComp->GetMemorySlot();

			for (auto i = 0; i < memorySlot; i++)
			{
				WrapBox_MemorySlot->AddChildToWrapBox(CreateAbilityQuickSlot(i));
			}
		}
	}

	ReloadRegisterAbility();
}

void URegisterAbilityWidget::ReloadRegisterAbility()
{
	if (auto abComp = GetOwningPlayerPawn<ABaseCharacter>()->GetAbilityComponent())
	{
		const auto& tags = abComp->GetAbilityQuickSlot();

		for (int i = 0; i < WrapBox_MemorySlot->GetChildrenCount(); i++)
		{
			if(tags[i].IsValid() == false)
			{
				break;
			}
			
			if (auto button = Cast<UAbilityQuickSlotButtonWidget>(WrapBox_MemorySlot->GetChildAt(i)))
			{
				if (auto newData = NewObject<UAbilityData>())
				{
					auto findAbButton = GetAbilityButtonFromTag(tags[i]);
					if(findAbButton)
					{
						findAbButton->SetEquipped(true);
					}
					
					newData->OwnItemButtonWidget = findAbButton;
					newData->AbilityInformation = abComp->GetAbilityByTag(tags[i])->GetAbilityInformation();
					button->OverrideData(newData);
				}
			}
		}
	}
}

UAbilityQuickSlotButtonWidget* URegisterAbilityWidget::CreateAbilityQuickSlot(const int32 Index)
{
	if (auto pc = GetOwningPlayer())
	{
		if (auto button = CreateWidget<UAbilityQuickSlotButtonWidget>(pc, AbilityQuickSlotButtonClass))
		{
			button->SetIndex(Index);
			button->OnAlreadyRegisteredAbilityDropped.AddUniqueDynamic(
				this, &URegisterAbilityWidget::OnAlreadyRegisteredAbilityDroppedEvent);
			button->OnDroppedAbility.AddUniqueDynamic(this, &URegisterAbilityWidget::OnDroppedAbilityEvent);
			button->OnClearSlot.AddUniqueDynamic(this, &URegisterAbilityWidget::OnClearAbilityQuickSlotEvent);
			return button;
		}
	}

	return nullptr;
}

UAbilityButtonWidget* URegisterAbilityWidget::CreateAbilityButton(const FAbilityInformation& AbilityInformation)
{
	if (auto pc = GetOwningPlayer())
	{
		if (auto button = CreateWidget<UAbilityButtonWidget>(pc, AbilityButtonClass))
		{
			button->SetInfo(AbilityInformation);
			return button;
		}
	}

	return nullptr;
}

UAbilityButtonWidget* URegisterAbilityWidget::GetAbilityButtonFromTag(const FGameplayTag& AbilityTag)
{
	if(!AbilityTag.IsValid())
	{
		return nullptr;
	}


	for(auto iter : WrapBox_AbilityList->GetAllChildren())
	{
		if(auto widget = Cast<UAbilityButtonWidget>(iter))
		{
			
			if(widget->GetAbilityData().AbilityTag.MatchesTagExact(AbilityTag))
			{
				return widget;
			}
		}
	}
	
	return nullptr;
}

void URegisterAbilityWidget::NativeConstruct()
{
	Button_Close->OnClicked.AddUniqueDynamic(this, &URegisterAbilityWidget::OnClickedCloseButton);
	bUseAsPopUp = true;
	bRemovable = false;
	
	Super::NativeConstruct();
	CreateAbilityList();
	CreateMemorySlot();

	OnVisibilityChanged.AddUniqueDynamic(this, &URegisterAbilityWidget::OnVisibilityChangedEvent);
}

void URegisterAbilityWidget::OnVisibilityChangedEvent(ESlateVisibility InVisibility)
{
	if (IsVisible())
	{
		CreateAbilityList();
		CreateMemorySlot();
	}
	else
	{
		UWidgetHelperLibrary::CloseWidgetSetting(GetOwningPlayer(), this);
	}

	Super::OnVisibilityChangedEvent(InVisibility);
}

void URegisterAbilityWidget::CreateRegisterAbilityList()
{
	CreateAbilityList();
	CreateMemorySlot();
}

void URegisterAbilityWidget::OnClickedCloseButton()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void URegisterAbilityWidget::OnAlreadyRegisteredAbilityDroppedEvent(const FGameplayTag& AbilityTag)
{
	bool bRemoved = false;
	int32 removedIndex = -1;
	for (auto i = 0; i < WrapBox_MemorySlot->GetChildrenCount(); i++)
	{
		if (auto button = Cast<UAbilityQuickSlotButtonWidget>(WrapBox_MemorySlot->GetChildAt(i)))
		{
			if (auto data = button->GetSlotData<UAbilityData>())
			{
				if (data->AbilityInformation.AbilityTag.MatchesTagExact(AbilityTag))
				{
					UE_LOGFMT(LogTemp,Log,"{0} {1} : 이 태그를 가진 슬롯을 제거합니다 {2}",__FUNCTION__,__LINE__,AbilityTag.ToString());
					bRemoved = true;
					removedIndex = i;
					break;
				};
			}
		}
	}

	if (bRemoved)
	{
		//다음 슬롯정보가 없는 경우
		if(WrapBox_MemorySlot->GetChildAt(removedIndex + 1) == nullptr)
		{
			Cast<UAbilityQuickSlotButtonWidget>(WrapBox_MemorySlot->GetChildAt(removedIndex))->SlotClear();
			return;
		}
		
		UAbilityQuickSlotButtonWidget* lastSlot = nullptr;
		for (int32 i = removedIndex; i < WrapBox_MemorySlot->GetChildrenCount() - 1; ++i)
		{
			if (auto currentSlot = Cast<UAbilityQuickSlotButtonWidget>(WrapBox_MemorySlot->GetChildAt(i)))
			{
				if (auto nextSlot = Cast<UAbilityQuickSlotButtonWidget>(WrapBox_MemorySlot->GetChildAt(i + 1)))
				{
					lastSlot = nextSlot;
					UE_LOGFMT(LogTemp,Log,"{0} {1} : {2}를 다음 슬롯인 {3} 정보로 덮어씁니다",__FUNCTION__,__LINE__,currentSlot->GetName(),nextSlot->GetName());
					currentSlot->OverrideData(nextSlot->GetSlotData<UAbilityData>());
				}
			}
		}

		if(lastSlot != nullptr)
		{
			UE_LOGFMT(LogTemp,Log,"마지막 슬롯 비움 : {0}",lastSlot->GetName());
			lastSlot->SlotClear();
		}
	}
	
	
	if (auto abComp = GetOwningPlayerPawn<ABaseCharacter>()->GetAbilityComponent())
	{
		bool bIsAllSlotEmpty = true;
		TArray<FGameplayTag> tagArray;
		for (auto i = 0; i < WrapBox_MemorySlot->GetChildrenCount(); i++)
		{
			if (auto button = Cast<UAbilityQuickSlotButtonWidget>(WrapBox_MemorySlot->GetChildAt(i)))
			{
				if (auto data = button->GetSlotData<UAbilityData>())
				{
					if(data->AbilityInformation.AbilityTag.IsValid())
					{
						bIsAllSlotEmpty = false;
					}
					tagArray.Add(data->AbilityInformation.AbilityTag);
					continue;
				}

				tagArray.Add(FGameplayTag::EmptyTag);
			}
		}
		abComp->OverrideAbilityQuickSlotArray(tagArray);

		if(bIsAllSlotEmpty)
		{
			abComp->ClearMainAbiltiyQuickSlot();
		}
	}
}

void URegisterAbilityWidget::OnDroppedAbilityEvent(const FAbilityInformation& AbilityInformation)
{
	//어빌리티 퀵슬롯에서 비어있지 않은 가장 첫 슬롯을 찾아 할당해야 합니다.
	if (auto abComp = GetOwningPlayerPawn<ABaseCharacter>()->GetAbilityComponent())
	{
		const auto& slotIndex = abComp->AddQuickSlotAbility(AbilityInformation.AbilityTag);

		if(auto slot = Cast<UAbilityQuickSlotButtonWidget>(WrapBox_MemorySlot->GetChildAt(slotIndex)))
		{
			if(slot->GetIndex() == slotIndex)
			{
				UE_LOGFMT(LogTemp,Log,"이 퀵슬롯을 초기화 합니다.");
				auto newData = NewObject<UAbilityData>();
				newData->OwnItemButtonWidget = slot;
				newData->AbilityInformation = AbilityInformation;
				slot->Init(newData);
			}
		}
	}
}

void URegisterAbilityWidget::OnClearAbilityQuickSlotEvent(const FGameplayTag& AbilityTag)
{
	//이 태그를 퀵슬롯 배열에서 제거하고, 한칸씩 당겨야 합니다.
	OnAlreadyRegisteredAbilityDroppedEvent(AbilityTag);
}
