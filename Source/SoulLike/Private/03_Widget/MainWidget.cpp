// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/MainWidget.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "03_Widget/01_Menu/NavigationWidget.h"
#include "03_Widget/01_Menu/00_Inventory/InventoryWidget.h"
#include "03_Widget/01_Menu/03_Equipment/EquipWidget.h"
#include "03_Widget/01_Menu/04_Character/CharacterStatusWidget.h"
#include "03_Widget/01_Menu/04_Character/CharacterTotalInfoWidget.h"
#include "03_Widget/01_Menu/05_Orb/OrbWidget.h"
#include "03_Widget/02_Monster/BossHealthBarWidget.h"
#include "03_Widget/09_Resource/ResourceWidget.h"
#include "03_Widget/98_Debug/AbilityDebugWidget.h"
#include "03_Widget/99_Drag/DragAndDropOperation.h"
#include "03_Widget/99_Drag/DraggableWidget.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Logging/StructuredLog.h"


void UMainWidget::OpenNavigation(bool bForceClose)
{
	if (OpenMenuAnimation)
	{
		if (bForceClose)
		{
			if (bIsNavigationOpen)
			{
				UMG_CharacterInformation->SetVisibility(ESlateVisibility::Collapsed);
				UMG_Inventory->SetVisibility(ESlateVisibility::Collapsed);
				UMG_Equipment->SetVisibility(ESlateVisibility::Collapsed);
				UMG_Orb->SetVisibility(ESlateVisibility::Collapsed);
				PlayAnimationReverse(OpenMenuAnimation);

				bIsNavigationOpen = false;
				UWidgetHelperLibrary::CloseWidgetSetting(GetOwningPlayer(), this);
			}
		}
		else
		{
			if (bIsNavigationOpen)
			{
				//다른 모든 열린 UI를 닫습니다.
				UMG_CharacterInformation->SetVisibility(ESlateVisibility::Collapsed);
				UMG_Inventory->SetVisibility(ESlateVisibility::Collapsed);
				UMG_Equipment->SetVisibility(ESlateVisibility::Collapsed);
				UMG_Orb->SetVisibility(ESlateVisibility::Collapsed);
				PlayAnimationReverse(OpenMenuAnimation);
				UWidgetHelperLibrary::CloseWidgetSetting(GetOwningPlayer(), this);
			}
			else
			{
				PlayAnimationForward(OpenMenuAnimation);
				UWidgetHelperLibrary::OpenWidgetSetting(GetOwningPlayer(), this);
			}

			bIsNavigationOpen = !bIsNavigationOpen;
		}

		//NavigationOpenInputSetting();
	}
}

void UMainWidget::ShowBossWidget(ABaseMonster* Boss)
{
	if (Boss)
	{
		const auto& safeName = GetNameSafe(Boss);

		if (!BossHealthBarWidgetMap.Contains(safeName))
		{
			auto bossWidget = CreateWidget<UBossHealthBarWidget>(GetOwningPlayer(), BossHealthBarWidgetObject);
			bossWidget->SetInfo(Boss);
			VerticalBox_BossWidgets->AddChild(bossWidget);
			BossHealthBarWidgetMap.Emplace(safeName, bossWidget);
		}
	}
}

void UMainWidget::RemoveBossWidget(ABaseMonster* Boss)
{
	if (Boss)
	{
		const auto& safeName = GetNameSafe(Boss);
		if (BossHealthBarWidgetMap.Contains(safeName))
		{
			BossHealthBarWidgetMap[safeName]->RemoveFromParent();
			BossHealthBarWidgetMap.Remove(safeName);
		}
	}
}

void UMainWidget::ForceUpdateSpirit(const FInventoryItem& InventoryItem)
{
	UMG_Equipment->ForceUpdateSpirit(InventoryItem);
}

void UMainWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (UMG_AbilityDebug)
	{
		UMG_AbilityDebug->SetVisibility(ESlateVisibility::Collapsed);
	}

#if WITH_EDITOR
	if (UMG_AbilityDebug)
	{
		UMG_AbilityDebug->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
#endif
}

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Init();
}

void UMainWidget::OnClickedOpenButton()
{
	OpenNavigation();
}


void UMainWidget::Init()
{
	OwnerCharacter = GetOwningPlayerPawn<ABaseCharacter>();

	if (OwnerCharacter.IsValid())
	{
		if (auto attComp = OwnerCharacter->GetAttributeComponent())
		{
			attComp->OnChangeHPValue.AddUniqueDynamic(this, &UMainWidget::OnChangedHPEvent);
			attComp->OnChangeMPValue.AddUniqueDynamic(this, &UMainWidget::OnChangedMPEvent);
			attComp->OnChangeSPValue.AddUniqueDynamic(this, &UMainWidget::OnChangedSPEvent);

			attComp->OnChangeMaxHPValue.AddUniqueDynamic(this, &UMainWidget::OnChangedMaxHPEvent);
			attComp->OnChangeMaxMPValue.AddUniqueDynamic(this, &UMainWidget::OnChangedMaxMPEvent);
			attComp->OnChangeMaxSPValue.AddUniqueDynamic(this, &UMainWidget::OnChangedMaxSPEvent);


			attComp->InitProgressWidget();
		}

		if (UMG_Navigation)
		{
			UMG_Navigation->Init(this);
		}

		Button_NavigationOpen->OnClicked.AddUniqueDynamic(this, &UMainWidget::OnClickedOpenButton);

		if (UMG_CharacterInformation)
		{
			UMG_CharacterInformation->EquipmentWidget = UMG_Equipment;
		}
	}
}

void UMainWidget::OnChangedHPEvent(float HP, float MaxHP)
{
	if (UMG_HP)
	{
		UMG_HP->UpdateProgress(HP, MaxHP);
	}
}

void UMainWidget::OnChangedMPEvent(float MP, float MaxMP)
{
	if (UMG_MP)
	{
		UMG_MP->UpdateProgress(MP, MaxMP);
	}
}

void UMainWidget::OnChangedSPEvent(float SP, float MaxSP)
{
	if (UMG_SP)
	{
		UMG_SP->UpdateProgress(SP, MaxSP);
	}
}

void UMainWidget::OnChangedMaxHPEvent(float MaxValue)
{
	if (UMG_HP)
	{
		UMG_HP->UpdateProgressLength(MaxValue);
	}
}

void UMainWidget::OnChangedMaxMPEvent(float MaxValue)
{
	if (UMG_MP)
	{
		UMG_MP->UpdateProgressLength(MaxValue);
	}
}

void UMainWidget::OnChangedMaxSPEvent(float MaxValue)
{
	if (UMG_SP)
	{
		UMG_SP->UpdateProgressLength(MaxValue);
	}
}

bool UMainWidget::WidgetMoveToDraggedPoint(UDragAndDropOperation* Oper, const FGeometry& InGeometry,
                                           const FDragDropEvent& InDragDropEvent)
{
	if (Oper)
	{
		Oper->DraggedWidget->SetRenderOpacity(Oper->OriginalOpacity);

		if (!IsDropIgnoreWidget(Oper->DraggedWidget))
		{
			//오퍼레이션으로 가져온 드래그중인 위젯을 잠깐 제거합니다.
			Oper->DraggedWidget->RemoveFromParent();
			//다시 보여줍니다.
			CanvasPanel->AddChildToCanvas(Oper->DraggedWidget.Get());
			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Oper->DraggedWidget->Slot))
			{
				CanvasSlot->SetSize(Oper->DesiredSize);
				CanvasSlot->SetPosition(UWidgetLayoutLibrary::GetMousePositionOnViewport(this) - Oper->DragOffset);
			}
			//Oper->DraggedWidget->SetPositionInViewport(
			//	UWidgetLayoutLibrary::GetMousePositionOnViewport(this) - Oper->DragOffset, false);
		}
	}


	return true;
}

void UMainWidget::NavigationOpenInputSetting()
{
	UEnhancedInputLocalPlayerSubsystem* subSystem = nullptr;
	if (auto pc = GetOwningPlayer())
	{
		if (pc->PlayerInput->IsA<UEnhancedPlayerInput>())
		{
			subSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		}
	}

	if (subSystem == nullptr)
	{
		UE_LOGFMT(LogTemp, Error, "인풋 시스템을 가져올 수 없습니다. {0} {1}", __FUNCTION__, __LINE__);
		return;
	}

	//사용할 수 있는 입력설정을 교체합니다.
	if (bIsNavigationOpen)
	{
		GetOwningPlayer()->bShowMouseCursor = true;
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(GetOwningPlayer(), this);

		subSystem->ClearAllMappings();
		subSystem->AddMappingContext(NavigationMenuInputContext, 0);
	}
	//일반적인 입력 설정으로 되돌립니다.
	else
	{
		GetOwningPlayer()->bShowMouseCursor = false;
		subSystem->ClearAllMappings();
		GetOwningPlayerPawn<APlayerCharacter>()->SetDefaultMappingContext();
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetOwningPlayer());
	}
}

void UMainWidget::OpenCharacterInformation()
{
	if (UMG_CharacterInformation)
	{
		if (!UMG_CharacterInformation->IsVisible())
		{
			UMG_CharacterInformation->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			UMG_CharacterInformation->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UMainWidget::OpenInventory()
{
	if (UMG_Inventory)
	{
		if (!UMG_Inventory->IsVisible())
		{
			UMG_Inventory->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			UMG_Inventory->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UMainWidget::OpenEquipment()
{
	if (UMG_Equipment)
	{
		if (!UMG_Equipment->IsVisible())
		{
			UMG_Equipment->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			UMG_Equipment->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UMainWidget::OpenOrb()
{
	if (UMG_Orb)
	{
		if (!UMG_Orb->IsVisible())
		{
			UMG_Orb->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UMainWidget::OpenOption()
{
	if (UMG_Option)
	{
		if (!UMG_Option->IsVisible())
		{
			UMG_Option->SetVisibility(ESlateVisibility::Visible);
		}
	}
}
