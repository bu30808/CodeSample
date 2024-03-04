// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/07_NPC/OrbMatrixSlotOpenWidget.h"

#include "00_Character/04_NPC/99_Component/OrbMatrixSlotOpenComponent.h"
#include "00_Character/BaseCharacter.h"
#include "00_Character/00_Player/OrbBackgroundActor.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "03_Widget/01_Menu/05_Orb/OrbMatrixElementWidget.h"
#include "03_Widget/05_Alert/AlertWidget.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Logging/StructuredLog.h"
#include "00_Character/00_Player/00_Controller/UserController.h"

void UOrbMatrixSlotOpenWidget::ShowUnlockCost(int32 Cost)
{
	FString msg = FString::FormatAsNumber(Cost) + TEXT("비용을 지불하여 슬롯 개방");
	TextBlock_UnLock->SetText(FText::FromString(msg));
}

UOrbMatrix* UOrbMatrixSlotOpenWidget::GetCoreMatrix(const FInventoryItem& OrbCoreItem)
{
	if (const auto orbInformation = static_cast<const FOrbCoreInformation*>(OrbCoreItem.GetItemInformation()))
	{
		UOrbMatrix* matrix = orbInformation->OrbMatrix;
		return matrix;
	}

	return nullptr;
}

void UOrbMatrixSlotOpenWidget::OnClickedOrbMatrixElement(const FOrbMatrixElementInfo& MatrixInfo,
                                                         UOrbMatrixElementWidget* MatrixElementWidget)
{
	if (!MatrixInfo.bLock)
	{
		TextBlock_UnLock->SetText(FText::FromString(TEXT("이미 개방된 슬롯입니다.")));
		Button_UnLock->SetIsEnabled(false);
		UE_LOGFMT(LogTemp, Error, "이미 열린 슬롯입니다.");
		return;
	}

	//이 위젯에 이 메트릭스 요소를 잠금해제하는데 필요한 비용을 표시합니다.
	//잠금해제 비용은 데이터 테이블에서 읽어옵니다.
	if (CoreMatrix.IsValid() && UnlockTable.IsEmpty() == false)
	{
		Button_UnLock->SetIsEnabled(true);
		SelectedMatrixElement = MatrixInfo;
		SelectedMatrixElementWidget = MatrixElementWidget;

		switch (MatrixInfo.OrbMatrixSlotType)
		{
		case EOrbMatrixSlotType::PHYSICAL:
			{
				auto index = CoreMatrix->PhysicalLine.IndexOfByKey(MatrixInfo);
				if (UnlockTable.IsValidIndex(index))
				{
					ShowUnlockCost(UnlockTable[index]->Cost);
				}
				else
				{
					Button_UnLock->SetIsEnabled(false);
				}
				UE_LOGFMT(LogTemp, Log, "{0}코스트에 오픈 가능", UnlockTable[index]->Cost);
			}
			break;
		case EOrbMatrixSlotType::DEFENCE:
			{
				auto index = CoreMatrix->DefenceLine.IndexOfByKey(MatrixInfo);
				if (UnlockTable.IsValidIndex(index))
				{
					ShowUnlockCost(UnlockTable[index]->Cost);
				}
				else
				{
					Button_UnLock->SetIsEnabled(false);
				}
				UE_LOGFMT(LogTemp, Log, "{0}코스트에 오픈 가능", UnlockTable[index]->Cost);
			}
			break;
		case EOrbMatrixSlotType::MAGICAL:
			{
				auto index = CoreMatrix->MagicalLine.IndexOfByKey(MatrixInfo);
				if (UnlockTable.IsValidIndex(index))
				{
					ShowUnlockCost(UnlockTable[index]->Cost);
				}
				else
				{
					Button_UnLock->SetIsEnabled(false);
				}
				UE_LOGFMT(LogTemp, Log, "{0}코스트에 오픈 가능", UnlockTable[index]->Cost);
			}
			break;
		case EOrbMatrixSlotType::FREE:
			{
				auto index = CoreMatrix->FreeLine.IndexOfByKey(MatrixInfo);
				if (UnlockTable.IsValidIndex(index))
				{
					ShowUnlockCost(UnlockTable[index]->Cost);
				}
				else
				{
					Button_UnLock->SetIsEnabled(false);
				}
				UE_LOGFMT(LogTemp, Log, "{0}코스트에 오픈 가능", UnlockTable[index]->Cost);
			}
			break;
		default: ;
		}
	}
}

void UOrbMatrixSlotOpenWidget::SetOrbMatrixSlotOpenComponent(UOrbMatrixSlotOpenComponent* Component)
{
	OrbMatrixSlotOpenComponent = Component;
}

void UOrbMatrixSlotOpenWidget::OnClickedUnLock()
{
	if (CoreMatrix.IsValid())
	{
		OrbMatrixSlotOpenComponent->UnLock(GetOwningPlayer(), CoreMatrix.Get(), SelectedMatrixElement,
		                                   SelectedMatrixElementWidget.Get(), UnlockTable);
		//잠금해제 시도 이후, 이미 슬롯이 열렸다면, 버튼을 비활성화 합니다.
		if (SelectedMatrixElementWidget->GetCurMatrixElementInfo().bLock == false)
		{
			TextBlock_UnLock->SetText(FText::FromString(TEXT("이미 개방된 슬롯입니다.")));
			Button_UnLock->SetIsEnabled(false);
		}
	}
}

void UOrbMatrixSlotOpenWidget::OnClickedCloseButton()
{
	UWidgetHelperLibrary::CloseWidgetSetting(GetOwningPlayer(),this);
	RemoveFromParent();
}

void UOrbMatrixSlotOpenWidget::NativeConstruct()
{
	bUseAsPopUp = false;
	bRemovable = true;
	Super::NativeConstruct();
	Init();

	Button_UnLock->OnClicked.AddUniqueDynamic(this, &UOrbMatrixSlotOpenWidget::OnClickedUnLock);
	Button_UnLock->SetIsEnabled(false);
	Button_Close->OnClicked.AddUniqueDynamic(this, &UOrbMatrixSlotOpenWidget::OnClickedCloseButton);


	if(auto pawn = GetOwningPlayerPawn<APlayerCharacter>())
	{
		pawn->OrbBackgroundActor->ShowRender(true);
	}
	
	UWidgetHelperLibrary::ShowTutorialWidget(GetOwningPlayer(),FGameplayTag::RequestGameplayTag("Tutorial.Orb.SlotOpen"));
}

void UOrbMatrixSlotOpenWidget::Init()
{
	if (auto invenComp = GetOwningPlayerPawn<ABaseCharacter>()->GetInventoryComponent())
	{
		if (invenComp->GetInventory().Contains(invenComp->GetEquippedCoreID()))
		{
			const auto& core = invenComp->GetEquippedCore();
			if (OrbMatrixElementWidgetObject)
			{
				//오브 메트릭스를 만듭니다.
				CreateOrbMatrix(core);
			}
			//OnEquipCoreEvent(core);
		}
	}

	if (FragmentUnlockTable)
	{
		FragmentUnlockTable->GetAllRows("", UnlockTable);
	}
}

void UOrbMatrixSlotOpenWidget::CreateOrbMatrixFirst(const FInventoryItem& OrbCoreItem)
{
	Super::CreateOrbMatrixFirst(OrbCoreItem);
	for (auto e : MatrixElements)
	{
		e->ElementType = EOrbMatrixElementType::SlotOpen;
		e->OnClickedMatrixElement.AddUniqueDynamic(this, &UOrbMatrixSlotOpenWidget::OnClickedOrbMatrixElement);
	}

	CoreMatrix = GetCoreMatrix(OrbCoreItem);
}

void UOrbMatrixSlotOpenWidget::OverrideOrbMatrix(const FInventoryItem& OrbCoreItem)
{
	Super::OverrideOrbMatrix(OrbCoreItem);
	for (auto e : MatrixElements)
	{
		e->ElementType = EOrbMatrixElementType::SlotOpen;
		e->OnClickedMatrixElement.AddUniqueDynamic(this, &UOrbMatrixSlotOpenWidget::OnClickedOrbMatrixElement);
	}
	CoreMatrix = GetCoreMatrix(OrbCoreItem);
}

void UOrbMatrixSlotOpenWidget::OnVisibilityChangedEvent(ESlateVisibility InVisibility)
{
	if(auto pawn = GetOwningPlayerPawn<APlayerCharacter>())
	{
		pawn->OrbBackgroundActor->ShowRender(IsVisible());
	}
	
	UWidgetHelperLibrary::PopUpWidgetProcess(this,bUseAsPopUp);
}
