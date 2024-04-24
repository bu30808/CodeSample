// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldPartition/DataLayer/DataLayerInstance.h"
#include "NexusPortal.generated.h"

DECLARE_DELEGATE_TwoParams(FOnLayerActivated,const UDataLayerInstance* DataLayer, EDataLayerRuntimeState State);

UCLASS()
class SOULLIKE_API ANexusPortal : public AActor
{
	GENERATED_BODY()


protected:
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* BoxComponent;
	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* NiagaraComponent;

	//활성화 상태로 돌릴 레이어
	UPROPERTY(EditAnywhere)
	TArray<UDataLayerAsset*> LayerToActive;
	//로드됨 상태로 돌릴 레이어
	UPROPERTY(EditAnywhere)
	TArray<UDataLayerAsset*> LayerToLoaded;
	
	FOnLayerActivated OnNexusLayerActivated;

public:
	// Sets default values for this actor's properties
	ANexusPortal();

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintNativeEvent)
	void OnChangedLayerState(const UDataLayerInstance* DataLayer, EDataLayerRuntimeState State);

	void LayerActivePresetting(AActor* OtherActor);
	void EndLayerActivate(AActor* OtherActor);
	UFUNCTION()
	void OnActorBeginOverlapEvent(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void OnActorEndOverlapEvent(AActor* OverlappedActor, AActor* OtherActor);
	
	bool IsAlreadySet(AActor* OtherActor);
};
