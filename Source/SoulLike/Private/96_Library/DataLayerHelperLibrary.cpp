// Fill out your copyright notice in the Description page of Project Settings.


#include "96_Library/DataLayerHelperLibrary.h"

#include "92_Tools/WorldStreamingSourceActor.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "WorldPartition/DataLayer/DataLayerSubsystem.h"

bool UDataLayerHelperLibrary::IsInActivatedLayer(const UWorld* World,
                                                 const TArray<TObjectPtr<const UDataLayerAsset>>& DataLayerAssets)
{
	if (const UDataLayerSubsystem* dataLayerSubsystem = UWorld::GetSubsystem<UDataLayerSubsystem>(World))
	{
		for (auto iter : DataLayerAssets)
		{
			if (dataLayerSubsystem->GetDataLayerInstanceRuntimeState(iter) != EDataLayerRuntimeState::Activated)
			{
				return false;
			}
		}
		return true;
	}

	return false;
}

bool UDataLayerHelperLibrary::IsInActivatedLayer(const UWorld* World, const FString& LayerPath)
{
	if (const UDataLayerSubsystem* dataLayerSubsystem = UWorld::GetSubsystem<UDataLayerSubsystem>(World))
	{
		if (const auto instance = dataLayerSubsystem->GetDataLayerInstanceFromAssetName(FName(LayerPath)))
		{
			if (dataLayerSubsystem->GetDataLayerRuntimeState(instance) != EDataLayerRuntimeState::Activated)
			{
				return false;
			}
		}
	}
	return false;
}

FString UDataLayerHelperLibrary::GetLayerFullPath(const UWorld* World, const UDataLayerAsset* LayerAsset)
{
	if (const UDataLayerSubsystem* dataLayerSubsystem = UWorld::GetSubsystem<UDataLayerSubsystem>(World))
	{
		return dataLayerSubsystem->GetDataLayerInstanceFromAsset(LayerAsset)->GetDataLayerFullName();
	}

	return FString();
}

AWorldStreamingSourceActor* UDataLayerHelperLibrary::SpawnWorldStreamingSourceActor(APawn* Owner)
{
	if (Owner)
	{
		FActorSpawnParameters spawnParam;
		spawnParam.Owner = Owner;
		spawnParam.Instigator = Owner;


		//일단 플레이어의 주변이 스트리밍이 끝난 후에, 로드에 필요한 행동을 합니다.
		if (const auto streamingSource = Owner->GetWorld()->SpawnActor<AWorldStreamingSourceActor>(
			AWorldStreamingSourceActor::StaticClass(), spawnParam))
		{
			streamingSource->bShouldDestroy = true;
			return streamingSource;
		}
	}

	return nullptr;
}

UDataLayerSubsystem* UDataLayerHelperLibrary::GetDataLayerSubsystem(const UObject* Context)
{
	if (UWorld* world = Context->GetWorld())
	{
		return UWorld::GetSubsystem<UDataLayerSubsystem>(world);
	}

	return nullptr;
}
