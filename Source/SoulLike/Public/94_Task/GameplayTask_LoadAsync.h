// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTask.h"
#include "GameplayTask_LoadAsync.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadCompleted, UObject*, LoadedObject);

/**
 * 
 */
UCLASS()
class SOULLIKE_API UGameplayTask_LoadAsync : public UGameplayTask
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TWeakObjectPtr<AActor> Owner;
	UPROPERTY()
	TSoftObjectPtr<UObject> SoftObject;

public:
	UGameplayTask_LoadAsync(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	static UGameplayTask_LoadAsync* LoadSoftObject(AActor* TaskOwner, TSoftObjectPtr<UObject> Object);

	virtual void Activate() override;

	//로드가 끝난 후 브로드캐스트됩니다.
	UPROPERTY(BlueprintAssignable)
	FOnLoadCompleted OnLoadCompleted;

private:
	void LoadSoftObjectFromPath(const TSoftObjectPtr<UObject>& Object);
	//로드가 끝난 후 호출되는 함수입니다.
	UFUNCTION()
	void OnAsyncLoadCompleted();
};
