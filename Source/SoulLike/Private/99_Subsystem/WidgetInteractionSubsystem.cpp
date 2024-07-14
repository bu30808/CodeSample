// Fill out your copyright notice in the Description page of Project Settings.


#include "99_Subsystem/WidgetInteractionSubsystem.h"

#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedPlayerInput.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Logging/StructuredLog.h"

UWidgetInteractionSubsystem::UWidgetInteractionSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> context(TEXT(
		"/Script/EnhancedInput.InputMappingContext'/Game/Blueprints/00_Character/00_Input/IMC_MenuInput.IMC_MenuInput'"));
	if (context.Succeeded())
	{
		WidgetInputContext = context.Object;
	}
}

void UWidgetInteractionSubsystem::OpenWidgetSetting(APlayerController* PC, UUserWidget* FocusWidget)
{
	//MenuInputContext추가
	if (auto system = GetLocalInputSystem(PC))
	{
		PC->bShowMouseCursor = true;
		WidgetOpenStack.Push(false);
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("위젯 열림 스택 : %d"), WidgetOpenStack.Num()));
		UE_LOGFMT(LogCharacter, Log, "위젯 입력 매핑을 적용합니다.");
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PC, FocusWidget, EMouseLockMode::LockAlways, true);
		system->ClearAllMappings();
		system->AddMappingContext(WidgetInputContext, 0);
	}
}

void UWidgetInteractionSubsystem::CloseWidgetSetting(APlayerController* PC, bool RemainMousePoint)
{
	UE_LOGFMT(LogCharacter, Log, "입력메핑 스택1");
	if (auto system = GetLocalInputSystem(PC))
	{
		UE_LOGFMT(LogCharacter, Log, "입력메핑 스택2");
		if (WidgetOpenStack.Num() > 0)
		{
			WidgetOpenStack.Pop();
		}

		if (WidgetOpenStack.IsEmpty())
		{
			UE_LOGFMT(LogCharacter, Log, "입력메핑 스택3");

			system->ClearAllMappings();
			PC->GetPawn<APlayerCharacter>()->SetDefaultMappingContext();

			if (RemainMousePoint)
			{
				UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PC);
			}
			else
			{
				UE_LOGFMT(LogCharacter, Log, "마우스 포인터를 제거하고, 게임에 인풋을 고정합니다.");
				UWidgetBlueprintLibrary::SetInputMode_GameOnly(PC);
				PC->bShowMouseCursor = false;
			}
		}
	}
}

void UWidgetInteractionSubsystem::InitSubsystem()
{
	WidgetOpenStack.Empty();
}

UEnhancedInputLocalPlayerSubsystem* UWidgetInteractionSubsystem::GetLocalInputSystem(APlayerController* PC) const
{
	if (PC)
	{
		if (PC->PlayerInput->IsA<UEnhancedPlayerInput>())
		{
			return ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		}
	}

	return nullptr;
}
