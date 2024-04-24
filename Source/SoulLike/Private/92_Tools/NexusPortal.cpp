// Fill out your copyright notice in the Description page of Project Settings.


#include "92_Tools/NexusPortal.h"

#include "NiagaraComponent.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "96_Library/DataLayerHelperLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "WorldPartition/DataLayer/DataLayerAsset.h"
#include "WorldPartition/DataLayer/DataLayerSubsystem.h"


// Sets default values
ANexusPortal::ANexusPortal()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetBoxExtent(FVector(32, 700, 700));
	RootComponent = BoxComponent;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ANexusPortal::BeginPlay()
{
	Super::BeginPlay();
}

void ANexusPortal::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OnActorBeginOverlap.AddUniqueDynamic(this, &ANexusPortal::OnActorBeginOverlapEvent);
	OnActorEndOverlap.AddUniqueDynamic(this, &ANexusPortal::OnActorEndOverlapEvent);
}

void ANexusPortal::OnChangedLayerState_Implementation(const UDataLayerInstance* DataLayer, EDataLayerRuntimeState State)
{
	if(!OnNexusLayerActivated.ExecuteIfBound(DataLayer, State))
	{
		UE_LOGFMT(LogTemp, Error, "ANexusPortal : 레이어 설정 델리게이트 호출에 실패했습니다.");
	}
}

void ANexusPortal::LayerActivePresetting(AActor* OtherActor)
{
	if (auto player = Cast<APlayerCharacter>(OtherActor))
	{
		player->DisableInput(Cast<APlayerController>(player->GetController()));
		player->GetCapsuleComponent()->SetEnableGravity(false);

		auto manager = UGameplayStatics::GetPlayerCameraManager(player, 0);
		manager->StartCameraFade(0, 1.f, 1.f, FColor::Black, true, true);
	}
}

void ANexusPortal::EndLayerActivate(AActor* OtherActor)
{
	UE_LOGFMT(LogTemp, Log, "ANexusPortal : 레이어 설정이 끝났습니다.");
	OnNexusLayerActivated.Unbind();
	if (auto player = Cast<APlayerCharacter>(OtherActor))
	{
		auto manager = UGameplayStatics::GetPlayerCameraManager(OtherActor, 0);
		manager->StopCameraFade();
		player->GetCapsuleComponent()->SetEnableGravity(true);
		player->EnableInput(Cast<APlayerController>(player->GetController()));
		if (auto subsystem = UDataLayerHelperLibrary::GetDataLayerSubsystem(this))
		{
			subsystem->OnDataLayerRuntimeStateChanged.RemoveDynamic(
				this, &ANexusPortal::OnChangedLayerState);
		}
	}
}

void ANexusPortal::OnActorBeginOverlapEvent(AActor* OverlappedActor, AActor* OtherActor)
{
	if (auto subsystem = UDataLayerHelperLibrary::GetDataLayerSubsystem(this))
	{
		if(IsAlreadySet(OtherActor))
		{
			return;
		}
		UE_LOGFMT(LogTemp, Log, "ANexusPortal : 레이어 설정을 시작합니다.");
		OnNexusLayerActivated.BindLambda(
			[ this, OtherActor](const UDataLayerInstance* DataLayer, EDataLayerRuntimeState State)
			{
				if (State == EDataLayerRuntimeState::Activated)
				{
					for (auto iter : LayerToActive)
					{
						if (DataLayer->GetDataLayerFullName().Equals(iter->GetPathName()))
						{
							EndLayerActivate(OtherActor);
							break;
						}
					}
				}
			});


		subsystem->OnDataLayerRuntimeStateChanged.AddUniqueDynamic(
			this, &ANexusPortal::OnChangedLayerState);

		LayerActivePresetting(OtherActor);

		for (const auto iter : LayerToActive)
		{
			subsystem->SetDataLayerInstanceRuntimeState(iter, EDataLayerRuntimeState::Activated);
		}

		for (const auto iter : LayerToLoaded)
		{
			subsystem->SetDataLayerInstanceRuntimeState(iter, EDataLayerRuntimeState::Loaded);
		}
	}
}

void ANexusPortal::OnActorEndOverlapEvent(AActor* OverlappedActor, AActor* OtherActor)
{
}

bool ANexusPortal::IsAlreadySet(AActor* OtherActor)
{
	if (UDataLayerSubsystem* dataLayerSubsystem = UDataLayerHelperLibrary::GetDataLayerSubsystem(OtherActor))
	{
		//해당 레이어가 전부 활성화 상태인가요?
		for(auto iter : LayerToActive)
		{
			if(dataLayerSubsystem->GetDataLayerInstanceRuntimeState(iter) != EDataLayerRuntimeState::Activated)
			{
				return false;	
			}
		}
		//해당 레이어가 전부 로드됨 상태인가요?
		for(auto iter : LayerToLoaded)
		{
			if(dataLayerSubsystem->GetDataLayerInstanceRuntimeState(iter) != EDataLayerRuntimeState::Loaded)
			{
				return false;	
			}
		}

	}
	return true;
}
