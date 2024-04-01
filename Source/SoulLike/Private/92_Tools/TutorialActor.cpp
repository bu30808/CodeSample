// Fill out your copyright notice in the Description page of Project Settings.


#include "92_Tools/TutorialActor.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "03_Widget/11_Tutorial/TutorialWidget.h"
#include "96_Library/SaveGameHelperLibrary.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATutorialActor::ATutorialActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;

	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillboardComponent"));
	BillboardComponent->SetupAttachment(RootComponent);
}

void ATutorialActor::OnDestroyedEvent(AActor* DestroyedActor)
{
	if (auto instance = UGameplayStatics::GetGameInstance(GetWorld()))
	{
		Cast<USoulLikeInstance>(instance)->SaveTutorial(this);
	}
}

void ATutorialActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OnActorBeginOverlap.AddUniqueDynamic(this, &ATutorialActor::OnActorBeginOverlapEvent);
	OnDestroyed.AddUniqueDynamic(this, &ATutorialActor::OnDestroyedEvent);
}

// Called when the game starts or when spawned
void ATutorialActor::BeginPlay()
{
	Super::BeginPlay();

	if (USaveGameHelperLibrary::IsAlreadyReadTutorial(this))
	{
		Destroy();
	}
}

void ATutorialActor::OnActorBeginOverlapEvent(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor->IsA<APlayerCharacter>())
	{
		SetActorEnableCollision(false);

		if (auto pc = Cast<APlayerController>(Cast<APlayerCharacter>(OtherActor)->GetController()))
		{
			if (!TutoWidget.IsValid())
			{
				TutoWidget = CreateWidget<UTutorialWidget>(pc, TutorialWidgetObject);
			}

			TutoWidget->SetTutorial(TutorialTag, this);
			TutoWidget->AddToViewport();
		}
	}
}
