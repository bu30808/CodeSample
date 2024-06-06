// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RandomSplineMeshActor.generated.h"

UCLASS()
class SOULLIKE_API ARandomSplineMeshActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARandomSplineMeshActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USplineComponent* SplineComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UHierarchicalInstancedStaticMeshComponent* HISMComponent;

	UPROPERTY(EditAnywhere, Category = "Setup")
	TArray<class UStaticMesh*> StaticMeshes;

	UPROPERTY(EditAnywhere, Category = "Setup")
	int32 DistanceBetweenInstances;
	UPROPERTY(EditAnywhere, Category = "Setup")
	FVector InstanceScale = FVector(1,1,1);

	UFUNCTION(BlueprintCallable, Category = "Setup")
	void PopulateInstances();

	virtual void OnConstruction(const FTransform& Transform) override;
};
