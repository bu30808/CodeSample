// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WorldPartition/DataLayer/DataLayerInstance.h"
#include "DataLayerHelperLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UDataLayerHelperLibrary : public UObject
{
	GENERATED_BODY()

public:


	UFUNCTION(BlueprintCallable,BlueprintPure)
	static class AWorldStreamingSourceActor* SpawnWorldStreamingSourceActor(APawn* Owner);
	UFUNCTION(BlueprintCallable,meta=(HidePin = Context, DefaultToSelf = Context))
	static void SetDataLayerRuntimeInstanceState(UObject* Context, class UDataLayerAsset* Asset, EDataLayerRuntimeState NewState);
};
