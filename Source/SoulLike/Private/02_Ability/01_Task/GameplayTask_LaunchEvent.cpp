// Fill out your copyright notice in the Description page of Project Settings.


#include "02_Ability/01_Task/GameplayTask_LaunchEvent.h"

#include "GameplayTasksComponent.h"
#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Logging/StructuredLog.h"

UGameplayTask_LaunchEvent::UGameplayTask_LaunchEvent(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer)
{
	bTickingTask = true;
}

UGameplayTask_LaunchEvent* UGameplayTask_LaunchEvent::LaunchEvent(AActor* TaskOwner, UObject* AdditionalObj,
                                                                  float TickRateValue)
{
	if (TaskOwner->IsValidLowLevel() == false)
	{
		UE_LOGFMT(LogTemp, Error, "테스크 오너가 유효하지 않습니다 : {0} {1}", __FUNCTION__, __LINE__);
		return nullptr;
	}

	UActorComponent* taskComp = nullptr;
	if (TaskOwner->FindComponentByClass(UGameplayTasksComponent::StaticClass()) == nullptr)
	{
		if (UActorComponent* newComp = NewObject<UActorComponent>(TaskOwner, UGameplayTasksComponent::StaticClass()))
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
		taskComp = TaskOwner->GetComponentByClass(UGameplayTasksComponent::StaticClass());
	}

	UGameplayTask_LaunchEvent* newTask = NewTask<UGameplayTask_LaunchEvent>(taskComp);
	newTask->TickRate = TickRateValue;
	newTask->AdditionalObject = AdditionalObj;

	return newTask;
}

void UGameplayTask_LaunchEvent::Activate()
{
	Super::Activate();
	OnTaskActivate.Broadcast();
}

void UGameplayTask_LaunchEvent::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	if (IsFinished())
	{
		OnTaskTick.Clear();
		OnTaskTickWithDelta.Clear();

		bTickingTask = false;
		return;
	}


	DT = DeltaTime;

	if (TickRate == -1)
	{
		OnTaskTick.Broadcast();
		OnTaskTickWithDelta.Broadcast(DeltaTime);
	}
	else
	{
		CurrentTime += DeltaTime;

		if (CurrentTime >= TickRate)
		{
			// 여기서 원하는 작업을 수행
			// ...
			OnTaskTick.Broadcast();
			OnTaskTickWithDelta.Broadcast(CurrentTime);
			CurrentTime = 0.0f; // 시간 초기화
		}
	}
}


UGameplayTask_AddImpulse::UGameplayTask_AddImpulse(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer)
{
	bTickingTask = true;
}

UGameplayTask_AddImpulse* UGameplayTask_AddImpulse::Task_AddImpulse(ABaseCharacter* TaskOwner, float Time, float Power,
                                                                    FVector DirectionVector, bool bChangeVelocity)
{
	if (TaskOwner->IsValidLowLevel() == false)
	{
		UE_LOGFMT(LogTemp, Error, "테스크 오너가 유효하지 않습니다 : {0} {1}", __FUNCTION__, __LINE__);
		return nullptr;
	}

	UActorComponent* taskComp = nullptr;
	if (TaskOwner->FindComponentByClass(UGameplayTasksComponent::StaticClass()) == nullptr)
	{
		if (UActorComponent* newComp = NewObject<UActorComponent>(TaskOwner, UGameplayTasksComponent::StaticClass()))
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
		taskComp = TaskOwner->GetComponentByClass(UGameplayTasksComponent::StaticClass());
	}

	UGameplayTask_AddImpulse* newTask = NewTask<UGameplayTask_AddImpulse>(taskComp);
	newTask->DirVector = DirectionVector;
	newTask->FinishTime = Time;
	newTask->ForcePower = Power;
	newTask->CharacterMovementComponent = TaskOwner->GetCharacterMovement();
	newTask->bVelocityChange = bChangeVelocity;

	return newTask;
}

void UGameplayTask_AddImpulse::Activate()
{
	Super::Activate();
	TotalTime = 0;
}

void UGameplayTask_AddImpulse::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	// ElapsedTime 갱신
	TotalTime += DeltaTime;

	// N초 동안의 시간이 지나지 않았다면
	if (TotalTime < FinishTime)
	{
		// 플레이어 캐릭터에게 특정 방향으로 힘을 가함 (AddImpulse)
		// 방향 벡터와 힘의 세기를 곱하여 힘 벡터 생성
		FVector Impulse = DirVector * ForcePower * DeltaTime;
		// 플레이어 캐릭터에게 힘을 가함
		CharacterMovementComponent->AddImpulse(Impulse, bVelocityChange);
	}
	else
	{
		OnTaskFinished.Broadcast();
		EndTask();
	}
}
