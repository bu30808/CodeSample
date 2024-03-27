// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "04_Item/ItemActor.h"
#include "UObject/Object.h"
#include "DataLayerHelperLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UDataLayerHelperLibrary : public UObject
{
	GENERATED_BODY()
public:

	static bool IsInActivatedLayer(const UWorld* World,const TArray<TObjectPtr<const UDataLayerAsset>>& DataLayerAssets);
	static bool IsInActivatedLayer(const UWorld* World,const FString& LayerPath);
	static FString GetLayerFullPath(const UWorld* World, const class UDataLayerAsset* LayerAsset);
	static class UDataLayerAsset* GetAlwaysActivatedDataLayerAsset(const UObject* WorldContext);
	
};
