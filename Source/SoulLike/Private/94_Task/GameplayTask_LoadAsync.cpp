// Fill out your copyright notice in the Description page of Project Settings.


#include "94_Task/GameplayTask_LoadAsync.h"

#include "GameplayTasksComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

UGameplayTask_LoadAsync::UGameplayTask_LoadAsync(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bTickingTask = false;
}

UGameplayTask_LoadAsync* UGameplayTask_LoadAsync::LoadSoftObject(AActor* TaskOwner, TSoftObjectPtr<UObject> Object)
{
	if (TaskOwner != nullptr)
	{
		UActorComponent* taskComp = nullptr;
		if (TaskOwner->FindComponentByClass(UGameplayTasksComponent::StaticClass()) == nullptr)
		{
			taskComp = NewObject<UGameplayTasksComponent>(TaskOwner, UGameplayTasksComponent::StaticClass());
			taskComp->RegisterComponent();
		}
		else
		{
			taskComp = TaskOwner->GetComponentByClass(UGameplayTasksComponent::StaticClass());
		}

		UGameplayTask_LoadAsync* newTask = NewTask<UGameplayTask_LoadAsync>(taskComp);

		newTask->Owner = TaskOwner;
		newTask->SoftObject = Object;

		return newTask;
	}

	return nullptr;
}

void UGameplayTask_LoadAsync::Activate()
{
	Super::Activate();
	LoadSoftObjectFromPath(SoftObject);
}

void UGameplayTask_LoadAsync::LoadSoftObjectFromPath(const TSoftObjectPtr<UObject>& Object)
{
	const FSoftObjectPath path = Object.ToSoftObjectPath();

	FStreamableManager& streamable = UAssetManager::GetStreamableManager();

	const FStreamableDelegate loadCompletedDelegate = FStreamableDelegate::CreateUObject(
		this, &UGameplayTask_LoadAsync::OnAsyncLoadCompleted);

	streamable.RequestAsyncLoad(path, loadCompletedDelegate);
}

void UGameplayTask_LoadAsync::OnAsyncLoadCompleted()
{
	OnLoadCompleted.Broadcast(SoftObject.Get());
	EndTask();
}
