// Fill out your copyright notice in the Description page of Project Settings.
#include "96_Library/WidgetHelperLibrary.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/00_Player/00_Controller/UserController.h"
#include "00_Character/00_Player/00_Controller/00_Component/InputHandlerComponent.h"
#include "00_Character/00_Player/00_Controller/00_Component/WidgetManagerComponent.h"
#include "03_Widget/01_Menu/00_Inventory/BossItemGetWidget.h"
#include "03_Widget/05_Alert/AlertWidget.h"
#include "03_Widget/11_Tutorial/TutorialWidget.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "99_Subsystem/ToolTipWidgetSubsystem.h"
#include "99_Subsystem/WidgetInteractionSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "SoulLike/SoulLike.h"


void UWidgetHelperLibrary::ShowAlertMsg(AUserController* PC, EAlertMsgType AlertMsgType, FText Msg, const FOnButtonClicked& OnClickedOKButtonEvent)
{
	if (PC)
	{
		if (auto widget = PC->GetWidgetManagerComponent()->AddWidget(FGameplayTag::RequestGameplayTag("Widget.Alert"),1,false))
		{
			Cast<UAlertWidget>(widget)->SetAlertMsg(AlertMsgType, Msg, OnClickedOKButtonEvent);
		}else
		{
			UKismetSystemLibrary::PrintString(PC,TEXT("위젯을 가져올 수 없습니다!!!"));
		}
	}
}

USimpleToolTipWidget* UWidgetHelperLibrary::GetSimpleToolTipWidget(APlayerController* PC, FText Msg)
{
	if (auto system = UGameplayStatics::GetGameInstance(PC)->GetSubsystem<UToolTipWidgetSubsystem>())
	{
		return system->CreateSimpleToolTipWidget(PC, Msg);
	}


	return nullptr;
}

void UWidgetHelperLibrary::OpenWidgetSetting(APlayerController* PC, UUserWidget* OpenWidget)
{
	if (PC)
	{
		if(auto pawn = PC->GetPawn<ABaseCharacter>())
		{
			UE_LOGFMT(LogTemp,Warning,"{0}에 의한 위젯 열림 설정",OpenWidget->GetName());
		
			pawn->SetIgnoreLookInput(true,PC->GetPawn(),FGameplayTag::RequestGameplayTag("Common.Passive.IgnoreMoveInput.Widget"));
			pawn->SetIgnoreMoveInput(true,PC->GetPawn(),FGameplayTag::RequestGameplayTag("Common.Passive.IgnoreMoveInput.Widget"));
			PC->SetShowMouseCursor(true);
			PC->GetLocalPlayer()->GetSubsystem<UWidgetInteractionSubsystem>()->OpenWidgetSetting(
				PC, OpenWidget);
		}
	}
}

void UWidgetHelperLibrary::CloseWidgetSetting(APlayerController* PC, UUserWidget* ClosedWidget, bool RemainMousePoint)
{
	if (PC)
	{
		UE_LOGFMT(LogTemp,Log,"위젯 닫힘 설정");
		if(auto pawn = PC->GetPawn<ABaseCharacter>())
		{
			pawn->SetIgnoreLookInput(false,PC->GetPawn(),FGameplayTag::RequestGameplayTag("Common.Passive.IgnoreMoveInput.Widget"));
			pawn->SetIgnoreMoveInput(false,PC->GetPawn(),FGameplayTag::RequestGameplayTag("Common.Passive.IgnoreMoveInput.Widget"));
			PC->GetLocalPlayer()->GetSubsystem<UWidgetInteractionSubsystem>()->CloseWidgetSetting(PC,RemainMousePoint);
		}
	}
}

void UWidgetHelperLibrary::SetToolTipWidget(UUserWidget* TargetWidget, const FText& Msg)
{
	if (TargetWidget)
	{
		auto tooltipWidget = Cast<USimpleToolTipWidget>(TargetWidget->GetToolTip());

		if (tooltipWidget == nullptr)
		{
			tooltipWidget = GetSimpleToolTipWidget(TargetWidget->GetOwningPlayer<APlayerController>(), FText::GetEmpty());
			TargetWidget->SetToolTip(tooltipWidget);
		}
		tooltipWidget->SetDescriptionText(Msg);
		
	}
}

FString UWidgetHelperLibrary::EnhancementTagToDecoString(const FGameplayTag& Key)
{
	if (Key.ToString().Contains("BreathOfTheForest"))
	{
		return "<img id=\"enhancement.BreathOfTheForest\"/>";
	}
	if (Key.ToString().Contains("WhisperOfTheWild"))
	{
		return "<img id=\"enhancement.WhisperOfTheWild\"/>";
	}
	if (Key.ToString().Contains("FlowerOfTheSpirit"))
	{
		return "<img id=\"enhancement.FlowerOfTheSpirit\"/>";
	}
	if (Key.ToString().Contains("SpiritEssence"))
	{
		return "<img id=\"enhancement.SpiritEssence\"/>";
	}
	if (Key.ToString().Contains("SpiritStone"))
	{
		return "<img id=\"enhancement.SpiritStone\"/>";
	}

	return TEXT("유효하지 않음");
}

void UWidgetHelperLibrary::ShowBossItemGetWidget(AUserController* PC, AItemActor* Item)
{
	if (PC)
	{
		if (auto widget = PC->GetWidgetManagerComponent()->AddWidget(FGameplayTag::RequestGameplayTag("widget.boss.item")))
		{
			Cast<UBossItemGetWidget>(widget)->AddItemElement(Item);
		}
	}
}

void UWidgetHelperLibrary::ShowTutorialWidget(APlayerController* PC, FGameplayTag TutorialTag)
{
	if (PC)
	{
		if(auto instance = Cast<USoulLikeInstance>(PC->GetGameInstance()))
		{
			//세이브 기능을 이용하는 경우, 이미 보고 넘긴 세이브인지 확인합니다.
			if(IsUseSaveGameMode(PC))
			{
				if(instance->IsSkippedTutorial(TutorialTag))
				{
					return;
				}
			}
		
			if (auto widget = Cast<AUserController>(PC)->GetWidgetManagerComponent()->AddWidget(FGameplayTag::RequestGameplayTag("widget.tutorial"),2,false))
			{
				UE_LOGFMT(LogTemp,Warning,"튜토리얼 위젯을 추가합니다.");
				Cast<UTutorialWidget>(widget)->SetTutorial(TutorialTag,nullptr);

				if(IsUseSaveGameMode(PC))
				{
					instance->SaveTutorial(TutorialTag);
				}
			}
		}
	}
}

void UWidgetHelperLibrary::PopUpWidgetProcess(UUserWidget* Widget, bool bIsRemovable)
{
	if(auto pc = Cast<AUserController>(Widget->GetOwningPlayer()))
	{
		if(Widget->IsVisible())
		{
			pc->AddToPopUp(Widget);
		}else
		{
			pc->RemoveFromPopUp(Widget,bIsRemovable);
		}
	}
}

bool UWidgetHelperLibrary::IsUseSaveGameMode(UObject* WorldContext)
{
	if(auto world = WorldContext->GetWorld())
	{
		return world->GetAuthGameMode()->IsA<ASoulLikeGameMode>();
	}
	return false;
}
