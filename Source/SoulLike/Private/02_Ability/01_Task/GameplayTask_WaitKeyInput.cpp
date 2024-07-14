// Fill out your copyright notice in the Description page of Project Settings.


#include "02_Ability/01_Task/GameplayTask_WaitKeyInput.h"

#include "EnhancedInputSubsystems.h"
#include "GameplayTasksComponent.h"
#include "InputAction.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "Logging/StructuredLog.h"

UGameplayTask_WaitKeyInput::UGameplayTask_WaitKeyInput(const FObjectInitializer& ObjectInitializer): Super(
	ObjectInitializer)
{
	bTickingTask = true;
}

UGameplayTask_WaitKeyInput* UGameplayTask_WaitKeyInput::WaitKeyInput(APlayerCharacter* Player,
                                                                     const FKeyPressedInfo& ActionInfo,
                                                                     EInputType InputType, bool bAutoTaskEnd)
{
	if (Player == nullptr)
	{
		return nullptr;
	}

	UActorComponent* taskComp = nullptr;
	if (Player->FindComponentByClass(UGameplayTasksComponent::StaticClass()) == nullptr)
	{
		if (UActorComponent* newComp = NewObject<UActorComponent>(Player, UGameplayTasksComponent::StaticClass()))
		{
			newComp->RegisterComponent();
			taskComp = newComp;
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		taskComp = Player->GetComponentByClass(UGameplayTasksComponent::StaticClass());
	}

	UGameplayTask_WaitKeyInput* newTask = NewTask<UGameplayTask_WaitKeyInput>(taskComp);
	newTask->PlayerCharacter = Player;
	newTask->WaitAction = ActionInfo;
	newTask->bWaitCont = !bAutoTaskEnd;
	newTask->WaitInputType = InputType;

	return newTask;
}


void UGameplayTask_WaitKeyInput::Activate()
{
	Super::Activate();
	//UE_LOGFMT(LogTemp,Error,"{0} {1}",__FUNCTION__,__LINE__);
	if (PlayerCharacter.IsValid())
	{
		//UE_LOGFMT(LogTemp,Error,"{0} {1}",__FUNCTION__,__LINE__);
		PC = PlayerCharacter->GetController<APlayerController>();
		if (PC == nullptr)
		{
			//UE_LOGFMT(LogTemp,Error,"{0} {1}",__FUNCTION__,__LINE__);
			EndTask();
		}

		if (auto InputLocalPlayerSubsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			//UE_LOGFMT(LogTemp,Error,"{0} {1}",__FUNCTION__,__LINE__);
			//UE_LOGFMT(LogTemp,Error,"{0} {1}",__FUNCTION__,__LINE__);
			Actionkeys = InputLocalPlayerSubsystem->QueryKeysMappedToAction(WaitAction.InputAction);
		}
	}
	else
	{
		//UE_LOGFMT(LogTemp,Error,"{0} {1}",__FUNCTION__,__LINE__);
		EndTask();
	}
}

void UGameplayTask_WaitKeyInput::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	if (PC.IsValid() && Actionkeys.IsValidIndex(0))
	{
		switch (WaitInputType) {
		case EInputType::PRESS:
			if (PC->WasInputKeyJustPressed(Actionkeys[0]))
			{
				if (OnKeyPressedWithAction.IsBound())
				{
					UE_LOGFMT(LogTemp, Warning, "Get Press Key!! : {0}", Actionkeys[0].ToString());
					OnKeyPressedWithAction.Broadcast(WaitAction);
				}
				else
				{
					UE_LOGFMT(LogTemp, Error, "눌림 키 이벤트가 바인드되지 않았습니다 : {0}", Actionkeys[0].ToString());
				}

				//계속 기다리는것이 아니라면 종료시킵니다.
				if (!bWaitCont)
				{
					EndTask();
				}
			}
			break;
		case EInputType::HELD_DOWN:
			if (PC->IsInputKeyDown(Actionkeys[0]))
			{
				if (OnKeyPressedWithAction.IsBound())
				{
					UE_LOGFMT(LogTemp, Warning, "Get held down  Key!! : {0}", Actionkeys[0].ToString());
					OnKeyPressedWithAction.Broadcast(WaitAction);
				}
				else
				{
					UE_LOGFMT(LogTemp, Error, "눌림 키 이벤트가 바인드되지 않았습니다 : {0}", Actionkeys[0].ToString());
				}

				//계속 기다리는것이 아니라면 종료시킵니다.
				if (!bWaitCont)
				{
					EndTask();
				}
			}
			break;
		case EInputType::RELEASE:
			if(PC->WasInputKeyJustReleased(Actionkeys[0]))
			{

				if (OnKeyReleasedWithAction.IsBound())
				{
					UE_LOGFMT(LogTemp, Warning, "Get Release Key!! : {0}", Actionkeys[0].ToString());
					OnKeyReleasedWithAction.Broadcast(WaitAction);
				}
				else
				{
					UE_LOGFMT(LogTemp, Error, "땜 키 이벤트가 바인드되지 않았습니다  : {0}", Actionkeys[0].ToString());
				}

				//계속 기다리는것이 아니라면 종료시킵니다.
				if (!bWaitCont)
				{
					EndTask();
				}
			}
			break;
		}
		
	}
}
