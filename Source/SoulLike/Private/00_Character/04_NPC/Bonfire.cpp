// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/04_NPC/Bonfire.h"

#include "NiagaraComponent.h"
#include "93_SaveGame/SoulLikeSaveGame.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "Components/AudioComponent.h"
#include "Components/BillboardComponent.h"
#include "Kismet/GameplayStatics.h"

ABonfire::ABonfire()
{
	
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ActivateParticle"));
	NiagaraComponent->SetupAttachment(RootComponent);
	NiagaraComponent->SetAutoActivate(false);
	
	InfiniteAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
	InfiniteAudioComponent->SetupAttachment(RootComponent);
	InfiniteAudioComponent->Deactivate();
	
	SpawnPointBillboardComponent =  CreateDefaultSubobject<UBillboardComponent>(TEXT("SpawnPointBillboardComponent"));
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
	
}

void ABonfire::AddBonfireInToDataTable()
{
#ifdef WITH_EDITOR
	if(BonfireTable!=nullptr)
	{
		auto safeName = GetNameSafe(this);
		UE_LOGFMT(LogTemp,Log,"테이블에 이 화톳불의 정보를 추가합니다 : {0}", GetNameSafe(this));
		BonfireInformation.OwnersSafeName = GetNameSafe(this);
		BonfireInformation.LevelName = UGameplayStatics::GetCurrentLevelName(this);
		BonfireInformation.Location = SpawnPointBillboardComponent->GetComponentLocation();
		
		if(safeName.IsEmpty()==false)
		{
			BonfireTable->AddRow(FName(safeName),BonfireInformation);
			BonfireTable->MarkPackageDirty();
		}
	}else
	{
		UE_LOGFMT(LogTemp,Error,"화톳불 테이블을 찾을 수 없습니다.");
	}
#endif
}

void ABonfire::ActivateBonfire(APlayerCharacter* Player)
{
	bIsActivate = true;

	UGameplayStatics::PlaySoundAtLocation(this,ActivateSound,GetActorLocation());
	InfiniteAudioComponent->Activate();
	InfiniteAudioComponent->Play();
	//필요하다면 여기서 애니메이션을 재생시키세요.
	NiagaraComponent->Activate();

	//이 화톳불이 활성화 되었다고 저장합니다.
	if (auto instance = Cast<USoulLikeInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		instance->SaveBonfire(this);
	}
}

TArray<FBonfireInformation*> ABonfire::GetTeleportList() const
{
	if(BonfireTable)
	{
		TArray<FBonfireInformation*> out;
		BonfireTable->GetAllRows<FBonfireInformation>("",out);

		return out;
	}

	return TArray<FBonfireInformation*>();
}

