// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldPartition/DataLayer/DataLayerInstance.h"
#include "PortalActor.generated.h"

UCLASS()
class SOULLIKE_API APortalActor : public AActor
{
	GENERATED_BODY()
	friend class UTransitionPortalSubsystem;
	friend class ATransition_PortalActor;
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class USceneComponent* SceneComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UStaticMeshComponent* PortalMeshComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UBoxComponent* BoxComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class USceneCaptureComponent2D* PortalCameraComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UWorldPartitionStreamingSourceComponent* StreamingSourceComponent;

	UPROPERTY(EditAnywhere)
	class UMaterialInterface* PortalMaterial;
	UPROPERTY(Transient,BlueprintReadWrite)
	class ADynamicSkyActor* Sky;
	UPROPERTY(Transient,BlueprintReadOnly)
	class UMaterialInstanceDynamic* PortalMaterialInstance;
	UPROPERTY(Transient)
	class UTextureRenderTarget2D* PortalRenderTarget;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float SkyTime;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	class UDataAsset* Weather;
	UPROPERTY(EditAnywhere,Transient)
	bool bUseDayNightCycle = false;
public:	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class APortalActor* LinkedPortal;


	// Sets default values for this actor's properties
	APortalActor();


	void SetTextureTarget(APortalActor* PortalActor);
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void SetPortalRenderTarget();
	void MaterialSetting();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostInitializeComponents() override;



	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void UpdateSceneCapture(APortalActor* Target) const;
	void CheckViewportSize();

	UFUNCTION(BlueprintCallable)
	void TeleportPlayer(AActor* OtherActor, APortalActor* Target);

	UFUNCTION(BlueprintCallable)
	void SetLinkedPortal(APortalActor* OtherPortal);

	UFUNCTION(BlueprintCallable)
	void SetSkyTimeAndWeather();
};
/*
 * 트렌지션 존으로 입장하는 포탈
 */
UENUM(BlueprintType)
enum class ETransitionPortalType:uint8
{
	ENTRANCE_TO_TRANSITION_ZONE,//원래 장소 → 트렌지션 존
	EXIT_TO_ORIGINAL_ZONE,//트렌지션 존 → 원래 장소
	EXIT_TO_ANOTHER_ZONE,//트렌지션 존 → 다른 새로운 존
	ENTRANCE_FROM_ANOTHER_ZONE//다른 새로운 존 → 트렌지션 존
};
UCLASS()
class SOULLIKE_API ATransition_PortalActor : public APortalActor
{
	GENERATED_BODY()

	friend class UTransitionPortalSubsystem;
public:
	ATransition_PortalActor();
protected:

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* PlayerCheckComponent;
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* PostProcessOverlapComponent;
	UPROPERTY(VisibleAnywhere)
	class UPostProcessComponent* PostProcessComponent;
	
	
	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> Player;
	UPROPERTY(EditAnywhere)
	ETransitionPortalType TransitionPortalType;

	//트렌지션 레벨에서 활성화 할 레이어
	UPROPERTY(EditAnywhere)
	class UDataLayerAsset* TransitionDataLayerAssetToActivate;
	//그 외 활성화 해야 할 레이어
	UPROPERTY(EditAnywhere)
	class UDataLayerAsset* DataLayerAssetToActive;
	//비활성화해야 할 나머지 레이어
	UPROPERTY(EditAnywhere)
	TArray<class UDataLayerAsset*> TransitionDataLayerAssetsToUnloaded;
	//그 외 비활성화해야 할 나머지 레이어
	UPROPERTY(EditAnywhere)
	TArray<class UDataLayerAsset*> DataLayerAssetsToUnloaded;
	UPROPERTY(EditAnywhere)
	bool bShouldActivateStreaming = false;
	//목적지
	UPROPERTY(EditAnywhere)
	TObjectPtr<class ATransition_PortalActor> DestinationPortalActor;
	
	UPROPERTY(EditAnywhere)
	FLinearColor OriginalPortalColor;

	UPROPERTY()
	FTimerHandle EnableCameraLagTimer;
	UPROPERTY()
	FTimerHandle ReactivateTimerHandle;

	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void TeleportEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnPlayerCheckedEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable)
	void TeleportToZone(AActor* OtherActor);
	UFUNCTION()
	void OnDatalayerInstanceRuntimeStateChangedEvent(const UDataLayerInstance* DataLayer, EDataLayerRuntimeState State);

	void ReActivatePortal();
	void DeactivatePortal();
	void EnableCameraLag();
	void DisableCameraLag();
	void DataLayerSetting();
	void SetPlayerCameraLag(bool bEnabled);
	void OverrideSkyAndWeatherCurValue();
	void ActivateDataLayerAssetToActive();
	void OverrideUnloadedLayers(const TArray<UDataLayerAsset*>&	UnloadLayers){DataLayerAssetsToUnloaded = UnloadLayers;}
};
