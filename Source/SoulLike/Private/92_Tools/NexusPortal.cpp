// Fill out your copyright notice in the Description page of Project Settings.


#include "92_Tools/NexusPortal.h"

#include "NiagaraComponent.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "92_Tools/WorldStreamingSourceActor.h"
#include "96_Library/DataLayerHelperLibrary.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerStart.h"
#include "WorldPartition/DataLayer/DataLayerAsset.h"
#include "WorldPartition/DataLayer/DataLayerSubsystem.h"


// Sets default values
ANexusPortal::ANexusPortal()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = DefaultSceneRoot;

	PortalMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	PortalMeshComponent->SetupAttachment(RootComponent);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(PortalMeshComponent);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(PortalMeshComponent);
}

// Called when the game starts or when spawned
void ANexusPortal::BeginPlay()
{
	Super::BeginPlay();
}

void ANexusPortal::SetPortalMaterial()
{
	if (auto instance = PortalMeshComponent->CreateDynamicMaterialInstance(0, PortalMaterial))
	{
		instance->SetTextureParameterValue("Image", LocationRenderTargetImage);
	}
}

void ANexusPortal::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//OnActorBeginOverlap.AddUniqueDynamic(this, &ANexusPortal::OnActorBeginOverlapEvent);

	SetPortalMaterial();
}

void ANexusPortal::StartTeleport(AActor* OtherActor)
{
	if (OtherActor->IsA<APlayerCharacter>())
	{
		if (TeleportLocation != nullptr && !LayerToActivate.IsEmpty())
		{
			if (Player == nullptr)
			{
				Player = Cast<APlayerCharacter>(OtherActor);
			}
			
			Player->ShowLoadingScreen(true);

			if(auto subsystem = UDataLayerManager::GetDataLayerManager(this))
			{

				for(auto iter : LayerToLoaded)
				{
					subsystem->SetDataLayerInstanceRuntimeState(subsystem->GetDataLayerInstance(iter), EDataLayerRuntimeState::Loaded);
				}

				
				for (auto iter : LayerToActivate)
				{
					subsystem->SetDataLayerInstanceRuntimeState(subsystem->GetDataLayerInstance(iter), EDataLayerRuntimeState::Activated);
				}
				
				if (auto streamingActor = UDataLayerHelperLibrary::SpawnWorldStreamingSourceActor(Player.Get()))
				{
					streamingActor->OnStreamingComplete.AddUniqueDynamic(this, &ANexusPortal::OnStreamingCompleteEvent);
					streamingActor->OnAfterStreamingComplete.AddUniqueDynamic(
						this, &ANexusPortal::OnAfterStreamingCompleteEvent);

					streamingActor->StreamingStart(TeleportLocation->GetActorLocation());
				}
			}

			
		}
	}
}

void ANexusPortal::OnStreamingCompleteEvent()
{
	if (auto subsystem =  UDataLayerManager::GetDataLayerManager(this))
	{
		subsystem->SetDataLayerInstanceRuntimeState(subsystem->GetDataLayerInstanceFromAsset(NexusLayerAsset), EDataLayerRuntimeState::Unloaded);
	}
}

void ANexusPortal::OnAfterStreamingCompleteEvent()
{
	if (Player.IsValid())
	{
		Player->SetActorRotation(TeleportLocation->GetActorRotation());
		Player->ResetCameraBoom();
		Player->ShowLoadingScreen(false);
		Player->GetController<APlayerController>()->SetViewTargetWithBlend(Player.Get());
	}
}
