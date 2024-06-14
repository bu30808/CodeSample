// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/04_NPC/Chest.h"

#include "NiagaraComponent.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "96_Library/SaveGameHelperLibrary.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#define LOCTEXT_NAMESPACE "Chest"
namespace GlobalChestActionText
{
	static const FText openText = NSLOCTEXT("Chest", "ChestOpen", "연다");
	static const FText pickUpText = NSLOCTEXT("Chest", "ChestItemPickUp", "획득");
}
#undef LOCTEXT_NAMESPACE
// Sets default values
AChest::AChest()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;

	ChestMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestMeshComponent"));
	ChestMeshComponent->SetupAttachment(RootComponent);
	
	ChestMeshTopComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestMeshTopComponent"));
	ChestMeshTopComponent->SetupAttachment(ChestMeshComponent);
	
	ItemSpawnPointComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ItemSpawnPointComponent"));
	ItemSpawnPointComponent->SetupAttachment(ChestMeshComponent);

	ChestItemGlowNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ChestItemGlowNiagaraComponent"));
	ChestItemGlowNiagaraComponent->SetupAttachment(ItemSpawnPointComponent);
	
	ChestOpenTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("ChestOpenTimelineComponent"));
}

// Called when the game starts or when spawned
void AChest::BeginPlay()
{
	Super::BeginPlay();

	if(IsAlreadyOpen())
	{
		SetOpen();
		if(!IsAlreadyGetChestItem()){
			ChestItemGlowNiagaraComponent->Activate();
		}else
		{
			ChestItemGlowNiagaraComponent->Deactivate();
		}
	}
}

void AChest::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	BoxComponent->OnComponentBeginOverlap.AddUniqueDynamic(this,&AChest::OnBeginOverlapEvent);
	
	OnUpdateChestOpenTimeLine.BindUObject(this,&AChest::OnUpdateChestOpenTimeLineEvent);
	OnFinishChestOpenTimeLine.BindUObject(this,&AChest::OnFinishChestOpenTimeLineEvent);
	
	ChestOpenTimelineComponent->AddInterpFloat(ChestOpenCurve, OnUpdateChestOpenTimeLine);
	ChestOpenTimelineComponent->SetTimelineFinishedFunc(OnFinishChestOpenTimeLine);

	ChestItemGlowNiagaraComponent->Deactivate();
}

void AChest::OnBeginOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->IsA<APlayerCharacter>())
	{
		if(auto player = Cast<APlayerCharacter>(OtherActor))
		{
			UE_LOGFMT(LogTemp,Log,"상자 오버렙 1");
			if(!bOpened)
			{
				UE_LOGFMT(LogTemp,Log,"상자 오버렙 2");
				player->ShowInteractionWidget(this,player->GetPickUpAction(),GlobalChestActionText::openText);
			}else
			{
				UE_LOGFMT(LogTemp,Log,"상자 오버렙 3");
				if(bAlreadyGetChestItem)
				{
					UE_LOGFMT(LogTemp,Log,"상자 오버렙 4");
					player->HideInteractionWidget();
					BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				}else
				{
					UE_LOGFMT(LogTemp,Log,"상자 오버렙 5");
					player->ShowInteractionWidget(this,player->GetPickUpAction(),GlobalChestActionText::pickUpText);
				}
			}
		}
	}
}

void AChest::OpenChest(APlayerCharacter* Player)
{
	bOpened = true;
	ChestItemGlowNiagaraComponent->Activate();
	UGameplayStatics::PlaySoundAtLocation(this,OpenSound,GetActorLocation(),FRotator::ZeroRotator);
			
	Player->HideInteractionWidget();
	Player->ShowInteractionWidget(this,Player->GetPickUpAction(),GlobalChestActionText::pickUpText);

	ChestOpenTimelineComponent->PlayFromStart();
	
	OnOpenChest.Broadcast(this,Player);

	//상자가 열렸다고 저장합니다.
	USaveGameHelperLibrary::SaveChestOpen(this);
}

void AChest::Interaction_Implementation(ABaseCharacter* Start)
{
	InteractionActor = Start;

	if(auto player = Cast<APlayerCharacter>(InteractionActor.Get()))
	{
		if (bOpened == false)
		{
			OpenChest(player);
		}else
		{
			if(IsAlreadyGetChestItem() == false)
			{
				//아이템 플레이어에게 추가해줌.
				GiveChestItemToPlayer();
			}
		}
	}
}

bool AChest::IsAlreadyOpen()
{
	return USaveGameHelperLibrary::IsAlreadyOpenChest(this);
}

void AChest::SetOpen()
{
	bOpened = true;
	ChestMeshTopComponent->SetRelativeRotation(FRotator(90,0,0));
}

bool AChest::IsAlreadyGetChestItem()
{
	bAlreadyGetChestItem = USaveGameHelperLibrary::IsAlreadyGetChestItem(this);
	return bAlreadyGetChestItem;
}

void AChest::GiveChestItemToPlayer()
{
	if(auto player = Cast<APlayerCharacter>(InteractionActor.Get()))
	{
		bAlreadyGetChestItem = true;
		ChestItemGlowNiagaraComponent->Deactivate();
		//이 상자의 아이템을 획득했다고 저장합니다.
		USaveGameHelperLibrary::SaveEarnedChestItem(this);
		player->HideInteractionWidget();
		//플레이어에게 아이템을 줍니다.
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = player;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if(AItemActor* chestItem = GetWorld()->SpawnActor<AItemActor>(ChestItemObject, spawnParams))
		{
			chestItem->SetItemCount(ItemCount);
			//이 함수 내부에서 자동 저장.
			player->GetInventoryComponent()->AddItem(chestItem);
		}
	}
}

void AChest::OnUpdateChestOpenTimeLineEvent(float Pitch)
{
	ChestMeshTopComponent->SetRelativeRotation(FRotator(Pitch,0,0));
}

void AChest::OnFinishChestOpenTimeLineEvent()
{

}

