// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/00_Player/SoulTomb.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "93_SaveGame/SoulLikeSaveGame.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASoulTomb::ASoulTomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Particle"));
	NiagaraComponent->AddRelativeLocation(FVector(0,0,5.f));
	RootComponent = NiagaraComponent;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxComponent->SetCollisionProfileName("Item");
	BoxComponent->SetupAttachment(RootComponent);
	
	bIsSpatiallyLoaded = false;
	
	static ConstructorHelpers::FObjectFinder<class UNiagaraSystem> particle(TEXT(
	"Script/Niagara.NiagaraSystem'/Game/Blueprints/00_Character/06_SoulTomb/Bottom05-04.Bottom05-04'"));
	if (particle.Succeeded())
	{
		NiagaraComponent->SetAsset(particle.Object);
	}
}

void ASoulTomb::OnEndPlayEvent(AActor* Actor, EEndPlayReason::Type EndPlayReason)
{
	UE_LOGFMT(LogTemp,Warning,"영혼 무덤 제거됨 : {0}",StaticEnum<EEndPlayReason::Type>()->GetValueAsString(EndPlayReason));
}

// Called when the game starts or when spawned
void ASoulTomb::BeginPlay()
{
	Super::BeginPlay();
	UE_LOGFMT(LogSave,Log,"영혼 무덤 생성됨 : {0}",GetNameSafe(this));
	OnEndPlay.AddUniqueDynamic(this,&ASoulTomb::OnEndPlayEvent);
}

void ASoulTomb::Activate(float exp, const FVector& Location)
{
	SetActorEnableCollision(true);
	SetEXP(exp);
	SetActorHiddenInGame(false);
	SetActorLocation(Location);
	UE_LOGFMT(LogSave,Log,"영혼 무덤 위치 이동 : {0} {1}",GetNameSafe(this),GetActorLocation().ToString());
	 
	
	FHitResult hit;
	if(UKismetSystemLibrary::LineTraceSingle(this,GetActorLocation(),GetActorLocation() + (GetActorUpVector() * -1000.f),UEngineTypes::ConvertToTraceType(ECC_Visibility),false,TArray<AActor*>(),EDrawDebugTrace::None,hit,true))
	{
		SetActorLocation(hit.Location);
	}
}

void ASoulTomb::Deactivate()
{
	SetActorEnableCollision(false);
	SetEXP(0);
	SetActorHiddenInGame(true);
}

void ASoulTomb::Absorption(APlayerCharacter* PlayerCharacter)
{
	if(auto attComp = PlayerCharacter->GetAttributeComponent())
	{
		attComp->SetEXP(attComp->GetEXP() + EXP);
		attComp->OnCharacterInformationUpdate.Broadcast();

		if(auto instance = UGameplayStatics::GetGameInstance(this))
		{
			Cast<USoulLikeInstance>(instance)->SaveAttribute(PlayerCharacter);
		}

		Destroy();
	}
}
