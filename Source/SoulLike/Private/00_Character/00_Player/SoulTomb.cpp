// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/00_Player/SoulTomb.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ASoulTomb::ASoulTomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxComponent->SetCollisionProfileName("Item");
	RootComponent = BoxComponent;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Particle"));
	NiagaraComponent->SetupAttachment(RootComponent);
	NiagaraComponent->AddRelativeLocation(FVector(0,0,5.f));
	
	static ConstructorHelpers::FObjectFinder<class UNiagaraSystem> particle(TEXT(
	"Script/Niagara.NiagaraSystem'/Game/Blueprints/00_Character/06_SoulTomb/Bottom05-04.Bottom05-04'"));
	if (particle.Succeeded())
	{
		NiagaraComponent->SetAsset(particle.Object);
	}
}

// Called when the game starts or when spawned
void ASoulTomb::BeginPlay()
{
	Super::BeginPlay();

	FHitResult hit;
	if(UKismetSystemLibrary::LineTraceSingle(this,GetActorLocation(),GetActorLocation() + (GetActorUpVector() * -1000.f),UEngineTypes::ConvertToTraceType(ECC_Visibility),false,TArray<AActor*>(),EDrawDebugTrace::None,hit,true))
	{
		SetActorLocation(hit.Location);
	}
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
