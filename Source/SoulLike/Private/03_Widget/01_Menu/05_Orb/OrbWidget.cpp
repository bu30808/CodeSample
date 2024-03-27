// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/05_Orb/OrbWidget.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/00_Player/OrbBackgroundActor.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "03_Widget/01_Menu/05_Orb/OrbElementListWidget.h"
#include "03_Widget/01_Menu/05_Orb/OrbMatrixElementWidget.h"
#include "96_Library/ItemHelperLibrary.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/WidgetSwitcher.h"
#include "Logging/StructuredLog.h"

void UOrbWidget::OnAddOrbItem(class ABaseCharacter* UsedBy, const FInventoryItem& ItemInfo, AItemActor* ItemActor)
{
	if (UItemHelperLibrary::IsOrbCore(ItemInfo))
	{
		UE_LOGFMT(LogTemp, Log, "코어 추가");
		UMG_OrbElementListCore->AddItem(ItemInfo);
		return;
	}

	if (UItemHelperLibrary::IsOrbFragment(ItemInfo))
	{
		UE_LOGFMT(LogTemp, Log, "파편 추가 : {0}", ItemInfo.GetItemInformation()->Item_Name.ToString());
		UMG_OrbElementListFragment->AddItem(ItemInfo);
	}
}

void UOrbWidget::OnClickedCoreButton()
{
	if (WidgetSwitcher)
	{
		WidgetSwitcher->SetActiveWidgetIndex(0);
	}
}

void UOrbWidget::OnClickedFragmentButton()
{
	if (WidgetSwitcher)
	{
		WidgetSwitcher->SetActiveWidgetIndex(1);
	}
}

void UOrbWidget::OnEquipCoreEvent(const FInventoryItem& OrbCoreItem)
{
	if (OrbMatrixElementWidgetObject)
	{
		//오브 메트릭스를 만듭니다.
		CreateOrbMatrix(OrbCoreItem);
	}
}

/*
void UOrbWidget::OnEquipFragmentEvent(const FInventoryItem& OrbFragmentItem)
{
	//파편에 할당된 능력치 및 어빌리티를 부여합니다.
	EquipOrbFragment(OrbFragmentItem);
}
*/

void UOrbWidget::EquipOrbCore(const FInventoryItem& OrbCoreItem)
{
	if (auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent())
	{
		invenComp->UseItem(OrbCoreItem.UniqueID);
	}
}

void UOrbWidget::EquipOrbFragment(const FInventoryItem& OrbFragmentItem)
{
	if (auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent())
	{
		invenComp->UseItem(OrbFragmentItem.UniqueID);
	}
}

void UOrbWidget::CreateOrbMatrix(const FInventoryItem& OrbCoreItem)
{
	UKismetSystemLibrary::PrintString(this,TEXT("오브메트릭스 생성"));
	if (MatrixElements.Num() == 0)
	{
		UKismetSystemLibrary::PrintString(this,TEXT("오브메트릭스 첫 생성"));
		CreateOrbMatrixFirst(OrbCoreItem);
	}
	else
	{
		UKismetSystemLibrary::PrintString(this,TEXT("오브메트릭스 덮어쓰기"));
		OverrideOrbMatrix(OrbCoreItem);
	}
}

void UOrbWidget::CreateOrbMatrixFirst(const FInventoryItem& OrbCoreItem)
{
	const auto orbInformation = static_cast<const FOrbCoreInformation*>(OrbCoreItem.GetItemInformation());
	if (orbInformation == nullptr)
	{
		return;
	}

	UniformGridPanel_Matrix->ClearChildren();
	if (const auto orbMatrix = orbInformation->OrbMatrix)
	{
		//처음 생성할 때,
		auto& matrix = orbMatrix->OrbMatrix;
		for (auto i = 0; i < matrix.Num(); i++)
		{
			if (matrix.IsValidIndex(i))
			{
				for (auto j = 0; j < matrix[i].Num(); j++)
				{
					if (auto e = CreateWidget<UOrbMatrixElementWidget>(this, OrbMatrixElementWidgetObject))
					{
						if (matrix[i][j].OrbMatrixSlotType == EOrbMatrixSlotType::CORE)
						{
							e->CoreColor = orbInformation->CoreColor;
							e->OnEquipCore.AddUniqueDynamic(this, &UOrbWidget::OnEquipCoreEvent);
							SetEquippedInOrbListWidget(OrbCoreItem.UniqueID, EOrbType::CORE, true);
							CoreMatrixElementWidget = e;
						}
						
						UniformGridPanel_Matrix->AddChildToUniformGrid(e, i, j);
						if (auto slot = Cast<UUniformGridSlot>(e->Slot))
						{
							slot->SetHorizontalAlignment(HAlign_Center);
							slot->SetVerticalAlignment(VAlign_Center);
						}

						e->SetOwnerWidget(this);
						e->SetInfo(matrix[i][j]);
												
						MatrixElements.Add(e);
					}
				}
			}
		}
	}
}

void UOrbWidget::OverrideOrbMatrix(const FInventoryItem& OrbCoreItem)
{
	const auto orbInformation = static_cast<const FOrbCoreInformation*>(OrbCoreItem.GetItemInformation());
	if (orbInformation == nullptr)
	{
		return;
	}

	if (const auto orbMatrix = orbInformation->OrbMatrix)
	{
		int32 matrixElementsIndex = 0;
		//처음 생성할 때,
		auto& matrix = orbMatrix->OrbMatrix;
		for (auto i = 0; i < matrix.Num(); i++)
		{
			if (matrix.IsValidIndex(i))
			{
				for (auto j = 0; j < matrix[i].Num(); j++)
				{
					if (MatrixElements.IsValidIndex(matrixElementsIndex))
					{
						auto e = MatrixElements[matrixElementsIndex++];
						e->ClearSlot();

						if (matrix[i][j].OrbMatrixSlotType == EOrbMatrixSlotType::CORE)
						{
							e->CoreColor = orbInformation->CoreColor;
							e->OnEquipCore.AddUniqueDynamic(this, &UOrbWidget::OnEquipCoreEvent);
							//e->MatrixElementInfo.UniqueID = OrbCoreItem.UniqueID;
							//e->UpdateToolTip(OrbCoreItem);
							CoreMatrixElementWidget = e;
							SetEquippedInOrbListWidget(OrbCoreItem.UniqueID, EOrbType::CORE, true);
						}
						
						e->SetInfo(matrix[i][j]);
						
					}
				}
			}
		}
	}
}

void UOrbWidget::RefreshMatrix(const FInventoryItem& Core)
{
	const auto orbInformation = static_cast<const FOrbCoreInformation*>(Core.GetItemInformation());
	if (orbInformation == nullptr)
	{
		return;
	}

	if (const auto orbMatrix = orbInformation->OrbMatrix)
	{
		int32 matrixElementsIndex = 0;
		//처음 생성할 때,
		auto& matrix = orbMatrix->OrbMatrix;
		for (auto i = 0; i < matrix.Num(); i++)
		{
			if (matrix.IsValidIndex(i))
			{
				for (auto j = 0; j < matrix[i].Num(); j++)
				{
					if (MatrixElements.IsValidIndex(matrixElementsIndex))
					{
						const auto e = MatrixElements[matrixElementsIndex++];
						e->SetInfo(matrix[i][j]);
					}
				}
			}
		}
	}
}

UOrbMatrixElementWidget* UOrbWidget::IsOrbEquipped(const FGuid& OrbID)
{
	if (OrbID == FGuid())
	{
		return nullptr;
	}

	for (auto e : MatrixElements)
	{
		if (e->MatrixElementInfo.UniqueID == OrbID)
		{
			return e;
		}
	}

	return nullptr;
}

void UOrbWidget::OnVisibilityChangedEvent(ESlateVisibility InVisibility)
{
	Super::OnVisibilityChangedEvent(InVisibility);
	
	if(auto pawn = GetOwningPlayerPawn<APlayerCharacter>())
	{
		pawn->OrbBackgroundActor->ShowRender(IsVisible());
		if (IsVisible())
		{	
			UWidgetHelperLibrary::ShowTutorialWidget(GetOwningPlayer(),FGameplayTag::RequestGameplayTag("Tutorial.Orb"));
			UMG_OrbElementListCore->Refresh();
			UMG_OrbElementListFragment->Refresh();

			if (auto invenComp = pawn->GetInventoryComponent())
			{
				if (invenComp->GetEquippedCoreID() != FGuid())
				{
					const auto& core = invenComp->GetEquippedCore();
					RefreshMatrix(core);
				}
			}
		}
	}
}

void UOrbWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (auto player = GetOwningPlayerPawn<ABaseCharacter>())
	{
		player->GetInventoryComponent()->OnAddItem.AddUniqueDynamic(this, &UOrbWidget::OnAddOrbItem);
	}

	if(Image_OrbRender)
	{
		Image_OrbRender->SetIsEnabled(false);
	}

	if (Button_Core)
	{
		Button_Core->OnClicked.AddUniqueDynamic(this, &UOrbWidget::OnClickedCoreButton);
	}

	if (Button_Fragment)
	{
		Button_Fragment->OnClicked.AddUniqueDynamic(this, &UOrbWidget::OnClickedFragmentButton);
	}

	OnVisibilityChanged.AddUniqueDynamic(this, &UOrbWidget::OnVisibilityChangedEvent);
}

void UOrbWidget::NativeConstruct()
{
	bUseAsPopUp = true;
	bRemovable =false;
	Super::NativeConstruct();

	if (OrbMatrixElementWidgetObject)
	{
		if (auto c = UniformGridPanel_Matrix->GetChildAt(0))
		{
			Cast<UOrbMatrixElementWidget>(c)->OnEquipCore.AddUniqueDynamic(this, &UOrbWidget::OnEquipCoreEvent);
		}
	}

	OnClickedFragmentButton();

}

void UOrbWidget::SetEquippedInOrbListWidget(const FGuid& OrbID, EOrbType OrbType, bool bEquipped)
{
	switch (OrbType)
	{
	case EOrbType::CORE:
		UMG_OrbElementListCore->SetEquipped(OrbID, bEquipped);
		break;
	case EOrbType::FRAGMENT:
		UMG_OrbElementListFragment->SetEquipped(OrbID, bEquipped);
		break;
	default: ;
	}
}
