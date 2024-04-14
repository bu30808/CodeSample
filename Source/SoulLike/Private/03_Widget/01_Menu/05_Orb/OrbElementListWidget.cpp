// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/05_Orb/OrbElementListWidget.h"

#include "00_Character/BaseCharacter.h"
#include "03_Widget/01_Menu/05_Orb/OrbElementFilterWidget.h"
#include "03_Widget/01_Menu/05_Orb/OrbListButtonWidget.h"
#include "96_Library/ItemHelperLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/WrapBox.h"
#include "Logging/StructuredLog.h"


void UOrbElementListWidget::OnClickedOpenFilter()
{
	if (VerticalBox_Filter->IsVisible())
	{
		VerticalBox_Filter->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		VerticalBox_Filter->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UOrbElementListWidget::OnClickedInitFilter()
{
	for (auto f : FilterWidgets)
	{
		f->CheckBox->SetIsChecked(false);
	}

	SlotFilter.Empty();

	Refresh();
}

void UOrbElementListWidget::InitFilterFromElement()
{
	for (auto iter : FilteredChild)
	{
		WrapBox->AddChildToWrapBox(iter);
	}
	FilteredChild.Empty();
	SlotFilterFromElement.Empty();
}

void UOrbElementListWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Button_OpenFilter)
	{
		Button_OpenFilter->OnClicked.AddUniqueDynamic(this, &UOrbElementListWidget::OnClickedOpenFilter);
	}

	if (Button_InitFilter)
	{
		Button_InitFilter->OnClicked.AddUniqueDynamic(this, &UOrbElementListWidget::OnClickedInitFilter);
	}

	if (VerticalBox_Filter)
	{
		TArray<UWidget*> widgets;
		WidgetTree->GetAllWidgets(widgets);

		for (auto w : widgets)
		{
			if (w->IsA<UOrbElementFilterWidget>())
			{
				Cast<UOrbElementFilterWidget>(w)->OrbElementListWidget = this;
				FilterWidgets.Add(Cast<UOrbElementFilterWidget>(w));
			}
		}
	}
}

void UOrbElementListWidget::NativeConstruct()
{
	Super::NativeConstruct();
	WrapBox->ClearChildren();
}

void UOrbElementListWidget::AddItem(const FInventoryItem& OrbInfo)
{
	UE_LOGFMT(LogTemp, Warning, "다음 오브 관련 아이템 추가됨 : {0}", OrbInfo.GetItemInformation()->Item_Name.ToString());
	if (auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent())
	{
		//이미 있으면 업데이트만 해줌.
		if (auto find = WrappedChild.FindByPredicate([OrbInfo](class UOrbListButtonWidget* button)
		{
			if (button->GetOrbData() != nullptr)
			{
				return button->GetOrbData()->Data.UniqueID == OrbInfo.UniqueID;
			}
			return false;
		}))
		{
			(*find)->UpdateData(OrbInfo);
		}
		//없으면 새로 추가
		else
		{
			if (auto button = CreateWidget<UOrbListButtonWidget>(GetOwningPlayer(), OrbListButtonWidgetObject))
			{
				if (auto item = NewObject<UOrbData>())
				{
					item->Data = OrbInfo;
					item->bIsEquipped = invenComp->IsEquippedEquipment(OrbInfo.UniqueID);
					UE_LOGFMT(LogTemp, Warning, "다음 오브 관련 아이템 새로 추가 , 장착여부 : {0} {1}",
					          OrbInfo.GetItemInformation()->Item_Name.ToString(), item->bIsEquipped);

					button->SetData(item);
				}

				WrappedChild.Emplace(button);
				WrapBox->AddChildToWrapBox(button);
			}
		}

		if (UItemHelperLibrary::IsOrbFragment(OrbInfo))
		{
			SortElement();
		}
	}
}

void UOrbElementListWidget::SetEquipped(const FGuid& OrbID, bool bEquipped)
{
	if (auto find = WrappedChild.FindByPredicate([OrbID](class UOrbListButtonWidget* button)
	{
		if (button->GetOrbData())
		{
			return button->GetOrbData()->Data.UniqueID == OrbID;
		}
		return false;
	}))
	{
		(*find)->SetEquipped(bEquipped);
	}
}

void UOrbElementListWidget::ForceReFiltering()
{
	for (auto f : FilterWidgets)
	{
		if (f->CheckBox->IsChecked())
		{
			f->CheckBox->OnCheckStateChanged.Broadcast(true);
		}
	}
}

TArray<UOrbData*> UOrbElementListWidget::FindSameSlotType(const TArray<UOrbData*>& FilteredByAttribute)
{
	if (OrbElementListType == EOrbElementListType::CORE)
	{
		return FilteredByAttribute;
	}

	if (SlotFilter.Num() == 0 && SlotFilterFromElement.Num() == 0)
	{
		return FilteredByAttribute;
	}

	TSet<UOrbData*> findData;
	for (auto data : FilteredByAttribute)
	{
		for (auto s : SlotFilter)
		{
			if (auto frag = static_cast<const FOrbFragmentInformation*>(data->Data.GetItemInformation()))
			{
				if (frag->SlotType == s)
				{
					findData.Emplace(data);
				}
			}
		}

		for (auto s : SlotFilterFromElement)
		{
			if (auto frag = static_cast<const FOrbFragmentInformation*>(data->Data.GetItemInformation()))
			{
				if (frag->SlotType == s)
				{
					findData.Emplace(data);
				}
			}
		}
	}

	TArray<UOrbData*> returnData;
	for (auto iter : findData)
	{
		if (iter != nullptr)
		{
			returnData.Emplace(iter);
		}
	}

	return returnData;
}

TArray<UOrbData*> UOrbElementListWidget::FindSameAttribute(const TArray<UOrbData*>& Items, EAttributeType FindAttribute)
{
	TArray<UOrbData*> findData;

	//이 어트리뷰트 타입이 있는 대상을 추가합니다.
	for (auto item : Items)
	{
		{
			//기본적으로 달린 능력치 체크
			if (const auto frag = static_cast<const FOrbFragmentInformation*>(item->Data.GetItemInformation()))
			{
				//있는 경우
				if (frag->EquipmentAttribute.ContainsByPredicate([FindAttribute](const FEquipmentAttribute& Att)
				{
					return Att.AttributeType == FindAttribute;
				}))
				{
					findData.Emplace(item);
				}
			}
			else
			{
				UKismetSystemLibrary::PrintString(this,TEXT("프레그먼트 데이터 정보를 가져올 수 없습니다!!!"));
			}
		}


		//강화로 달린 능력치 체크
		{
			for (const auto iter : Cast<AEquipmentItemActor>(item->Data.GetSpawndItemActor())->GetEnhancementInfo().
			     EnhancementMap)
			{
				if (iter.Key == FindAttribute && iter.Value > 0)
				{
					findData.Emplace(item);
				}
			}
		}
	}


	return findData;
}

bool UOrbElementListWidget::IsAllFilterEmpty()
{
	for (auto f : FilterWidgets)
	{
		if (f->CheckBox->IsChecked())
		{
			return false;
		}
	}

	if (SlotFilter.Num() > 0)
	{
		return false;
	}

	if (SlotFilterFromElement.Num() > 0)
	{
		return false;
	}

	return true;
}

void UOrbElementListWidget::OnUpdateFilterEvent()
{
	if (IsAllFilterEmpty())
	{
		Refresh();
		return;
	}

	TArray<UOrbData*> filteredData;
	//파편 슬롯 타입에 관계없이 어트리뷰트만 검사함.
	for (auto iter : WrappedChild)
	{
		filteredData.Emplace(iter->GetOrbData());
	}

	for (const auto attType : AttributeFilter)
	{
		filteredData = FindSameAttribute(filteredData, attType);
	}

	//슬롯 타입을 검사함
	auto result = FindSameSlotType(filteredData);

	//결과에 해당하는 애들만 필터 렙 박스에 추가해서 보여줍니다.
	WrapBox->SetVisibility(ESlateVisibility::Collapsed);
	for (auto iter : WrappedChild)
	{
		if (const auto data = iter->GetOrbData())
		{
			if (auto find = result.FindByPredicate([data](const UOrbData* item)
			{
				return item->Data.UniqueID == data->Data.UniqueID;
			}))
			{
				FilteredChild.Emplace(iter);
				WrapBox_Filter->AddChildToWrapBox(iter);
			}
		}
	}
	WrapBox_Filter->SetVisibility(ESlateVisibility::Visible);
}

void UOrbElementListWidget::AddFilter(EOrbMatrixSlotType SlotType)
{
	SlotFilter.Emplace(SlotType);
}

void UOrbElementListWidget::AddFilterFromElement(EOrbMatrixSlotType SlotType)
{
	SlotFilterFromElement.Emplace(SlotType);
}

void UOrbElementListWidget::AddFilter(EAttributeType AttributeType)
{
	AttributeFilter.AddUnique(AttributeType);
}

void UOrbElementListWidget::RemoveFilter(EOrbMatrixSlotType SlotType)
{
	SlotFilter.Remove(SlotType);
}

void UOrbElementListWidget::RemoveFilter(EAttributeType AttributeType)
{
	AttributeFilter.Remove(AttributeType);
}

void UOrbElementListWidget::RemoveFilterFromElement(EOrbMatrixSlotType SlotType)
{
	SlotFilterFromElement.Remove(SlotType);
}

void UOrbElementListWidget::CreateCoreListFromInventory()
{
	if (auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent())
	{
		const auto& inventory = invenComp->GetInventory();
		for (const auto& iter : inventory)
		{
			if (iter.Value.GetItemInformation()->Item_Tag.MatchesTag(
				FGameplayTag::RequestGameplayTag("Item.Equipment.Orb.Core")))
			{
				AddItem(iter.Value);
			}
		}
	}
}

void UOrbElementListWidget::CreateFragmentListFromInventory()
{
	WrapBox->ClearChildren();
	if (auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent())
	{
		const auto& inventory = invenComp->GetFragments();
		for (const auto& iter : inventory)
		{
			AddItem(iter.Value);
		}
	}
}

void UOrbElementListWidget::RefreshFromInventoryItem(const FInventoryItem& Item)
{
	AddItem(Item);
}

void UOrbElementListWidget::Refresh()
{
	WrapBox_Filter->SetVisibility(ESlateVisibility::Collapsed);
	FilteredChild.Empty();
	SortElement();
	WrapBox->SetVisibility(ESlateVisibility::Visible);
}

void UOrbElementListWidget::EnhancementSetting(UUserWidget* ParentsWidget)
{
	ParentsUMG = ParentsWidget;
}

void UOrbElementListWidget::OnItemButtonHovered(UOrbData* Data)
{
	if (ParentsUMG.IsValid())
	{
		Cast<UEnhancementWidget>(ParentsUMG)->ShowItemInformation(Data);
	}
}

void UOrbElementListWidget::GetWidgetBySlotType(EOrbMatrixSlotType Type)
{
	for (auto iter : WrappedChild)
	{
		if (auto orbData = iter->GetOrbData())
		{
			if (auto info = orbData->Data.GetItemInformation())
			{
				if (const auto frag = static_cast<const FOrbFragmentInformation*>(info))
				{
					if (frag->SlotType == Type)
					{
						WrapBox->AddChildToWrapBox(iter);
					}
				}
			}
		}
		else
		{
			UE_LOGFMT(LogTemp, Error, "버튼에 저장된 오브 데이터를 가져올 수 없어요");
		}
	}
}

void UOrbElementListWidget::RemoveFromParent()
{
	//UE_LOGFMT(LogTemp, Warning, "파편 리스트 내부 버튼이 제거되려 합니다. :{0}", GetName());
	Super::RemoveFromParent();
}

void UOrbElementListWidget::SortElement()
{
	WrapBox->ClearChildren();
	for (auto i = 1; i < static_cast<int>(EOrbMatrixSlotType::MAX); i++)
	{
		GetWidgetBySlotType(static_cast<EOrbMatrixSlotType>(i));
	}


	/*GridChild.Sort([](class UOrbListButtonWidget* a,UOrbListButtonWidget* b)
	{
		const auto fragA = static_cast<const FOrbFragmentInformation*>(a->GetOrbData()->Data.GetItemInformation());
		const auto fragB = static_cast<const FOrbFragmentInformation*>(b->GetOrbData()->Data.GetItemInformation());

		if(fragA !=nullptr && fragB!=nullptr)
		{
			return fragA->SlotType < fragB->SlotType;
		}

		return false;
	});*/
}
