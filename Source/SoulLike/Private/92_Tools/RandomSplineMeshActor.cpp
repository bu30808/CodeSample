// Fill out your copyright notice in the Description page of Project Settings.


#include "92_Tools/RandomSplineMeshActor.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Logging/StructuredLog.h"


// Sets default values
ARandomSplineMeshActor::ARandomSplineMeshActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	RootComponent = SplineComponent;

	HISMComponent = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HISMComponent"));
	HISMComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ARandomSplineMeshActor::BeginPlay()
{
	Super::BeginPlay();
	PopulateInstances();
}

void ARandomSplineMeshActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	PopulateInstances();
}

void ARandomSplineMeshActor::PopulateInstances()
{

	if (StaticMeshes.Num() == 0)
	{
		return;
	}

	HISMComponent->ClearInstances();

	int32 loop = UKismetMathLibrary::FTrunc(SplineComponent->GetSplineLength() / DistanceBetweenInstances);

	for(auto i = 0;i<loop;i++)
	{
		float distance = DistanceBetweenInstances * i;
		FVector location = SplineComponent->GetLocationAtDistanceAlongSpline(distance,ESplineCoordinateSpace::Local);

		FRotator rotation = UKismetMathLibrary::Conv_VectorToRotator(
		UKismetMathLibrary::ClampVectorSize(SplineComponent->GetTangentAtDistanceAlongSpline(distance,ESplineCoordinateSpace::Local),0,DistanceBetweenInstances));
		
		int32 MeshIndex = FMath::RandRange(0, StaticMeshes.Num() - 1);
		HISMComponent->SetStaticMesh(StaticMeshes[MeshIndex]);
		HISMComponent->AddInstance(FTransform(rotation,location,InstanceScale));
	}
	
	/*for (int32 i = 0; i < loop; ++i)
	{
		float Distance = (SplineComponent->GetSplineLength() / loop) * i;
		FVector Location = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
		FRotator Rotation = SplineComponent->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);

		// Randomly select a static mesh
		int32 MeshIndex = FMath::RandRange(0, StaticMeshes.Num() - 1);
		HISMComponent->SetStaticMesh(StaticMeshes[MeshIndex]);

		// Create a transform
		FTransform InstanceTransform(Rotation, Location);
		HISMComponent->AddInstance(InstanceTransform);
	}*/
}

void ARandomSplineMeshActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	PopulateInstances();
}

// Called every frame
void ARandomSplineMeshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

