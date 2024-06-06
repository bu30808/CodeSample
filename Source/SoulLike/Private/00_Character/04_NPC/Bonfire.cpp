// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/04_NPC/Bonfire.h"

#include "NiagaraComponent.h"
#include "00_Character/04_NPC/99_Component/BonfireComponent.h"
#include "93_SaveGame/SoulLikeSaveGame.h"
#include "96_Library/DataLayerHelperLibrary.h"
#include "96_Library/SaveGameHelperLibrary.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "Components/AudioComponent.h"
#include "Components/BillboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WorldPartition/DataLayer/DataLayerSubsystem.h"

#define PLAYER_COLLISION_CHANNEL ECC_GameTraceChannel5

class UDataLayerSubsystem;

ABonfire::ABonfire()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f;
	
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ActivateParticle"));
	NiagaraComponent->SetupAttachment(RootComponent);
	NiagaraComponent->SetAutoActivate(false);

	CannotActivate_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("CannotActivateParticle"));
	CannotActivate_NiagaraComponent->SetupAttachment(RootComponent);
	CannotActivate_NiagaraComponent->SetAutoActivate(false);


	InfiniteAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
	InfiniteAudioComponent->SetupAttachment(RootComponent);
	InfiniteAudioComponent->Deactivate();

	SpawnPointBillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("SpawnPointBillboardComponent"));
	SpawnPointBillboardComponent->SetupAttachment(RootComponent);
}

void ABonfire::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	NiagaraComponent->SetActive(false);
}

void ABonfire::BeginPlay()
{
	Super::BeginPlay();
	//이 화톳불이 플레이어가 활성환 화톳불인지 확인합니다.
	if (USaveGameHelperLibrary::IsActivatedBonfire(this))
	{
		SetActivate();
	}

	/*if (auto player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		player->OnChangePlayerState.AddUniqueDynamic(this, &ABonfire::OnChangePlayerStateEvent);
	}*/
}

void ABonfire::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	FHitResult hit;
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	objectTypes.Emplace(UEngineTypes::ConvertToObjectType(ECC_Monster));
	//주변에 몬스터가 있는지 확인합시다.
	bCanUseBonfire = !UKismetSystemLibrary::SphereTraceSingleForObjects(this,GetActorLocation(),GetActorLocation(),MonsterCheckRadius,objectTypes,false,TArray<AActor*>(),EDrawDebugTrace::ForDuration,hit,true);
	CannotActivate_NiagaraComponent->SetActive(!bCanUseBonfire);
	if (USaveGameHelperLibrary::IsActivatedBonfire(this))
	{
		NiagaraComponent->SetActive(bCanUseBonfire);
	}
	
}

void ABonfire::AddBonfireInToDataTable()
{
#if WITH_EDITOR
	if (BonfireTable != nullptr)
	{
		const auto& safeName = GetNameSafe(this);
		UE_LOGFMT(LogTemp, Log, "테이블에 이 화톳불의 정보를 추가합니다 : {0}", safeName);
		BonfireInformation.OwnersSafeName = GetNameSafe(this);
		BonfireInformation.LevelName = UGameplayStatics::GetCurrentLevelName(this);
		BonfireInformation.Location = SpawnPointBillboardComponent->GetComponentLocation();
		BonfireInformation.SkyTime = SkyTime;
		
		
		if (safeName.IsEmpty() == false)
		{
			BonfireTable->AddRow(FName(safeName), BonfireInformation);
			BonfireTable->MarkPackageDirty();
		}
	}
	else
	{
		UE_LOGFMT(LogTemp, Error, "화톳불 테이블을 찾을 수 없습니다.");
	}
#endif
}

void ABonfire::SetActivate()
{
	bIsActivate = true;
	InfiniteAudioComponent->Activate();
	InfiniteAudioComponent->Play();
	//필요하다면 여기서 애니메이션을 재생시키세요.
	NiagaraComponent->Activate();
}

void ABonfire::OnChangePlayerStateEvent(EPlayerCharacterState State)
{
	if (bIsActivate)
	{
		switch (State)
		{
		case EPlayerCharacterState::Peaceful:
			NiagaraComponent->SetColorParameter("Color", PeaceColor);
			break;
		case EPlayerCharacterState::Combat:
			NiagaraComponent->SetColorParameter("Color", CombatColor);
			break;
		}
	}
	else
	{
		switch (State)
		{
		case EPlayerCharacterState::Peaceful:
			CannotActivate_NiagaraComponent->Deactivate();
			break;
		case EPlayerCharacterState::Combat:
			CannotActivate_NiagaraComponent->Activate();
			break;
		}
	}
}

void ABonfire::ActivateBonfire(APlayerCharacter* Player)
{
	UGameplayStatics::PlaySoundAtLocation(this, ActivateSound, GetActorLocation());
	SetActivate();

	Player->OnChangePlayerState.AddUniqueDynamic(this, &ABonfire::OnChangePlayerStateEvent);


	//이 화톳불이 활성화 되었다고 저장합니다.
	if (auto instance = Cast<USoulLikeInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		instance->SaveBonfire(this);
		instance->SaveWithLastSavePoint(Player, GetComponentByClass<UBonfireComponent>());
	}
}

TArray<FBonfireInformation*> ABonfire::GetTeleportList() const
{
	if (BonfireTable)
	{
		TArray<FBonfireInformation*> out;
		BonfireTable->GetAllRows<FBonfireInformation>("", out);

		return out;
	}

	return TArray<FBonfireInformation*>();
}

const FBonfireInformation& ABonfire::GetBonfireInformation()
{
	if (BonfireTable)
	{
		
		if(auto info  = BonfireTable->FindRow<FBonfireInformation>(FName(GetNameSafe(this)),""))
		{
			BonfireInformation = *info;
		}
	}

	return BonfireInformation;
}

bool ABonfire::ShowInteractionWidgetDirectly_Implementation()
{
	return bCanUseBonfire;
}
