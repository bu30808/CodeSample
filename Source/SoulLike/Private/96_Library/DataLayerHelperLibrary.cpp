// Fill out your copyright notice in the Description page of Project Settings.


#include "96_Library/DataLayerHelperLibrary.h"

#include "92_Tools/WorldStreamingSourceActor.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"




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

void UDataLayerHelperLibrary::SetDataLayerRuntimeInstanceState(UObject* Context, UDataLayerAsset* Asset, EDataLayerRuntimeState NewState)
{
	if(auto subsystem = UDataLayerManager::GetDataLayerManager(Context))
	{
		auto instance = subsystem->GetDataLayerInstanceFromAsset(Asset);
		subsystem->SetDataLayerInstanceRuntimeState(instance,NewState);
	}
}
