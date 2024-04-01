// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/00_Player/00_Controller/UserController.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/00_Player/00_Controller/00_Component/InputHandlerComponent.h"
#include "00_Character/00_Player/00_Controller/00_Component/WidgetManagerComponent.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/PlayerInput.h"
#include "Logging/StructuredLog.h"

AUserController::AUserController()
{
	WidgetManagerComponent = CreateDefaultSubobject<UWidgetManagerComponent>(TEXT("WidgetManagerComponent"));
	InputHandlerComponent = CreateDefaultSubobject<UInputHandlerComponent>(TEXT("InputHandler"));

	TeamId = static_cast<uint8>(ETeam::Player);
}

void AUserController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AUserController::AddToPopUp(UUserWidget* Widget)
{
	UE_LOGFMT(LogSlate, Log, "팝업위젯에 추가됨 : {0}", Widget->GetName());
	UWidgetHelperLibrary::OpenWidgetSetting(this, Widget);
	PopUpStack.Push(Widget);
}

void AUserController::RemoveFromPopUp(UUserWidget* Widget, bool bIsRemovable)
{
	if (PopUpStack.Num() <= 0)
	{
		return;
	}

	if (PopUpStack.Top() == Widget)
	{
		auto pop = PopUpStack.Pop();
		UWidgetHelperLibrary::CloseWidgetSetting(this, pop, false);
		if (bIsRemovable)
		{
			UE_LOGFMT(LogSlate, Log, "팝업위젯 영구 제거 1 : {0}", pop->GetName());
			pop->RemoveFromParent();
		}
	}
	else
	{
		const auto index = PopUpStack.Find(Widget);
		if (index != INDEX_NONE)
		{
			UWidgetHelperLibrary::CloseWidgetSetting(this, Widget, false);
			if (bIsRemovable)
			{
				UE_LOGFMT(LogSlate, Log, "팝업위젯 영구 제거 2 : {0}", Widget->GetName());
				Widget->RemoveFromParent();
			}
			PopUpStack.RemoveAt(index);
		}
	}
}

void AUserController::ClosePopUp()
{
	if (PopUpStack.Num() <= 0)
	{
		return;
	}
	auto top = PopUpStack.Top();
	UE_LOGFMT(LogSlate, Log, "팝업위젯 닫기 요청 : {0}", top->GetName());
	top->SetVisibility(ESlateVisibility::Collapsed);
}

bool AUserController::IsThereAnyPopUp()
{
	return !PopUpStack.IsEmpty();
}

void AUserController::SetMouseSensitivity(float Sensitivity)
{
	if (PlayerInput)
	{
		PlayerInput->SetMouseSensitivity(Sensitivity);
	}
}

float AUserController::GetMouseSensitivity()
{
	if (PlayerInput)
	{
		return PlayerInput->GetMouseSensitivityX();
	}

	return 0.5f;
}
