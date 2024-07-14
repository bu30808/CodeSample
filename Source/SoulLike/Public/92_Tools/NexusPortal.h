// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldPartition/DataLayer/DataLayerInstance.h"
#include "NexusPortal.generated.h"

DECLARE_DELEGATE_TwoParams(FOnLayerActivated,const UDataLayerInstance* DataLayer, EDataLayerRuntimeState State);
/*
 *	BIC버전에선 비활성화
 */
UCLASS()
class SOULLIKE_API ANexusPortal : public AActor
{
	GENERATED_BODY()


protected:
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* DefaultSceneRoot;
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* PortalMeshComponent;
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* BoxComponent;
	
	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* NiagaraComponent;

	UPROPERTY(EditAnywhere)
	class UMaterialInterface* PortalMaterial;
	UPROPERTY(EditAnywhere)
	class UTexture* LocationRenderTargetImage;


	UPROPERTY(EditAnywhere,Category="Teleport")
	class APlayerStart* TeleportLocation;
	UPROPERTY(EditAnywhere,Category="Teleport")
	TArray<class UDataLayerAsset*> LayerToActivate;
	UPROPERTY(EditAnywhere,Category="Teleport")
	TArray<class UDataLayerAsset*> LayerToLoaded;
	UPROPERTY(EditAnywhere,Category="Teleport")
	const UDataLayerAsset* NexusLayerAsset;


	UPROPERTY()
	TWeakObjectPtr<class APlayerCharacter> Player;

public:
	// Sets default values for this actor's properties
	ANexusPortal();

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	
	UFUNCTION(BlueprintCallable)
	void SetPortalMaterial();
	UFUNCTION(BlueprintCallable)
	void StartTeleport(AActor* OtherActor);
	UFUNCTION()
	void OnStreamingCompleteEvent();
	UFUNCTION()
	void OnAfterStreamingCompleteEvent();
	
};
