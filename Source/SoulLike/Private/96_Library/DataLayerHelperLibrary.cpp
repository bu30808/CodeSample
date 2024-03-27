// Fill out your copyright notice in the Description page of Project Settings.


#include "96_Library/DataLayerHelperLibrary.h"

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
		if(const auto instance = dataLayerSubsystem->GetDataLayerInstanceFromAssetName(FName(LayerPath)))
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

UDataLayerAsset* UDataLayerHelperLibrary::GetAlwaysActivatedDataLayerAsset(const UObject* WorldContext)
{
	if(WorldContext)
	{
		if(auto instance = Cast<USoulLikeInstance>(WorldContext->GetWorld()->GetGameInstance()))
		{
			return instance->GetAlwaysActivatedLayer();
			
		}
		
	}

	return nullptr;
}
