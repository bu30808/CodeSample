// Fill out your copyright notice in the Description page of Project Settings.


#include "92_Tools/ClueActor.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "Components/BoxComponent.h"


// Sets default values
AClueActor::AClueActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;
	
}

// Called when the game starts or when spawned
void AClueActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AClueActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OnActorBeginOverlap.AddUniqueDynamic(this,&AClueActor::OnActorBeginOverlapEvent);
	OnActorEndOverlap.AddUniqueDynamic(this,&AClueActor::OnActorEndOverlapEvent);
}

void AClueActor::OnActorBeginOverlapEvent(AActor* OverlappedActor, AActor* OtherActor)
{
	if(OtherActor->IsA<APlayerCharacter>())
	{
		if(bCanShowMsg){
			bCanShowMsg = false;
			UWidgetHelperLibrary::ShowAlertMsg(Cast<APlayerCharacter>(OtherActor)->GetController<AUserController>(),EAlertMsgType::Normal,ShowMsg,FOnButtonClicked());
		}
	}
}

void AClueActor::OnActorEndOverlapEvent(AActor* OverlappedActor, AActor* OtherActor)
{
	if(OtherActor->IsA<APlayerCharacter>())
	{
		bCanShowMsg = true;
	}
}
