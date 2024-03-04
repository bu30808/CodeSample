// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/00_Player/00_Controller/00_Component/InputHandlerComponent.h"

#include "00_Character/00_Player/00_Controller/UserController.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "02_Ability/01_Task/GameplayTask_WaitKeyInput.h"
#include "Logging/StructuredLog.h"

// Sets default values for this component's properties
UInputHandlerComponent::UInputHandlerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UInputHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	PC = GetOwner<AUserController>();

	if (PC.IsValid())
	{
		InputLocalPlayerSubsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	}
}


// Called every frame
void UInputHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInputHandlerComponent::StartWaitAction(const TArray<FKeyPressedInfo>& WaitAction, bool bTriggerImmediately)
{
	PressedActionMap.Empty();
	
	if (PC.IsValid())
	{
		bObserveTrigger = bTriggerImmediately;
		//UKismetSystemLibrary::PrintString(this,TEXT("키 대기 시작"),true,true,FColor::Cyan,100.f);
		for (auto& info : WaitAction)
		{
	

			if (WaitActionTasks.Contains(info))
			{
				if (WaitActionTasks[info]->IsValidLowLevel())
				{
					WaitActionTasks[info]->EndTask();
				}
				WaitActionTasks.Remove(info);
			}

			if (auto task = UGameplayTask_WaitKeyInput::WaitKeyInput(PC->GetPawn<APlayerCharacter>(), info, true))
			{
				WaitActionTasks.Add(info, task);
				task->OnKeyPressedWithAction.AddUniqueDynamic(this, &UInputHandlerComponent::OnPressedKeyDown);
				task->ReadyForActivation();
			}
		}

	}
}

void UInputHandlerComponent::EndWaitAction(const TArray<FKeyPressedInfo>& WaitAction)
{
	bObserveTrigger = false;
	
	UE_LOGFMT(LogTemp, Log, "{0} {1} : 키 대기 구간을 종료하고, 입력이 있었다면 실행합니다.", __FUNCTION__, __LINE__);
	//UKismetSystemLibrary::PrintString(this,TEXT("키 대기 종료"),true,true,FColor::Cyan,100.f);

	//입력이 있었습니까?
	bool bIsPressed = false;
	for (auto action : WaitAction)
	{
		if (PressedActionMap.Contains(action))
		{
			if (PressedActionMap[action] == true)
			{
				UKismetSystemLibrary::PrintString(this,TEXT("키 입력 액션 실행 : ")+action.AbilityTag.ToString(),true,true,FColor::Cyan,100.f);
				BroadcastPressedEvent(action);
				bIsPressed = true;
				break;
			}
		}
	}
	

	for (auto action : WaitAction)
	{
		
		if (WaitActionTasks.Contains(action))
		{
			if (WaitActionTasks[action]->IsValidLowLevel())
			{
				WaitActionTasks[action]->EndTask();
			}
			else
			{
				WaitActionTasks.Remove(action);
			}
		}
	}

	if(!bIsPressed)
	{
		//UKismetSystemLibrary::PrintString(this,TEXT("키 입력이 없었습니다! : ") + WaitAction[0].AbilityTag.ToString(),true,true,FColor::Cyan,100.f);

		if(WaitAction.Num()>0){
			BroadcastNotPressedEvent(WaitAction[0]);
		}
	}

	/*
	for (auto action : notPressedAction)
	{
		UE_LOGFMT(LogTemp, Error, "{0} {1}", __FUNCTION__, __LINE__);
		BroadcastNotPressedEvent(action);
	}
	*/


}

void UInputHandlerComponent::BindEvent(const FKeyPressedInfo& ActionInfo, const FOnKeyPressedWithAction& OnKeyPressed,
                                       const FOnKeyNotPressed& OnKeyNotPressed)
{

	
	const auto& key = InputLocalPlayerSubsystem->QueryKeysMappedToAction(ActionInfo.InputAction);
	UE_LOGFMT(LogInput, Error, "{0} {1} : {2} 키 대기 이벤트 바인드", __FUNCTION__, __LINE__, key[0].ToString());
	InputPressedActionMap.Add(ActionInfo, OnKeyPressed);
	InputNotPressedMap.Add(ActionInfo, OnKeyNotPressed);
	
}


void UInputHandlerComponent::UnBindEvent(const FKeyPressedInfo& ActionInfo)
{
	if (ActionInfo.InputAction != nullptr)
	{
		const auto& key = InputLocalPlayerSubsystem->QueryKeysMappedToAction(ActionInfo.InputAction);
		if (key.Num() > 0)
		{
			UE_LOGFMT(LogInput, Error, "{0} {1} {2} : {3} 키 대기 이벤트 언바인드", __FUNCTION__, __LINE__,
			          ActionInfo.AbilityTag.ToString(), key[0].ToString());
			if (InputPressedActionMap.Contains(ActionInfo))
			{
				InputPressedActionMap.Remove(ActionInfo);
			}

			if (InputNotPressedMap.Contains(ActionInfo))
			{
				InputNotPressedMap.Remove(ActionInfo);
			}
		}
	}
}

void UInputHandlerComponent::BroadcastPressedEvent(const FKeyPressedInfo& Action)
{
	if (InputPressedActionMap.Contains(Action))
	{
		UE_LOGFMT(LogInput, Warning, "{0} {1}", __FUNCTION__, __LINE__);
		InputPressedActionMap[Action].Broadcast(Action);
	}
	else
	{
		auto key = InputLocalPlayerSubsystem->QueryKeysMappedToAction(Action.InputAction);
		UE_LOGFMT(LogInput, Error, "{0} {1} {2} : {3}키가 눌렸을 때, 행동할 맵에 저장된 함수가 없습니다.", __FUNCTION__, __LINE__,
		          Action.AbilityTag.ToString(), key[0].ToString());
	}
}

void UInputHandlerComponent::BroadcastNotPressedEvent(const FKeyPressedInfo& WaitAction)
{
	if (InputNotPressedMap.Contains(WaitAction))
	{
		InputNotPressedMap[WaitAction].Broadcast();
		InputNotPressedMap.Empty();
	}
	
}

void UInputHandlerComponent::OnPressedKeyDown(const FKeyPressedInfo& Action)
{

	auto key = InputLocalPlayerSubsystem->QueryKeysMappedToAction(Action.InputAction);
	UKismetSystemLibrary::PrintString(this,TEXT("키 입력 감지됨 : ") + Action.InputAction->GetName());
	UE_LOGFMT(LogTemp, Error, "{0} {1} : {2} 키 입력 확인됨.", __FUNCTION__, __LINE__, key[0].ToString());
	if(bObserveTrigger)
	{
		BroadcastPressedEvent(Action);
		return;
	}
	PressedActionMap.Add(Action, true);
}
