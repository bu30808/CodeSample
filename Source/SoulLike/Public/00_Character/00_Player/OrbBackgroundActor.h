// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrbBackgroundActor.generated.h"

UCLASS()
class SOULLIKE_API AOrbBackgroundActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrbBackgroundActor();

protected:
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* SceneComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class USceneCaptureComponent2D* SceneCaptureComponent2D;
	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* NiagaraComponent;

	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> TextureTarget;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void ShowRender(bool bIsVisible);
};
