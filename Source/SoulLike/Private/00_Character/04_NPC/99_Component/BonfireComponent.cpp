// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/04_NPC/99_Component/BonfireComponent.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "01_GameMode/SoulLikeGameMode.h"
#include "03_Widget/07_NPC/00_Bonfire/BonfireTeleportWidget.h"
#include "96_Library/ItemHelperLibrary.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogBonfire)

// Sets default values for this component's properties
UBonfireComponent::UBonfireComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	static ConstructorHelpers::FClassFinder<UBonfireTeleportWidget> widget(TEXT(
		"/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/02_Widget/NPC/Bonfire/UMG_BonfireTeleport.UMG_BonfireTeleport_C'"));
	if (widget.Succeeded())
	{
		BonfireTeleportListWidgetObject = widget.Class;
	}
}


// Called when the game starts
void UBonfireComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (auto instance = Cast<USoulLikeInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		OnRest.AddUniqueDynamic(this, &UBonfireComponent::Recover);
		OnRest.AddUniqueDynamic(this, &UBonfireComponent::PotionReplenishment);


		if (auto gameMode = Cast<ASoulLikeGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			OnRest.AddUniqueDynamic(gameMode, &ASoulLikeGameMode::RespawnMonsters);
			OnRest.AddUniqueDynamic(gameMode, &ASoulLikeGameMode::ClearTemporarySavedMonsterData);
		}
		else
		{
			UE_LOGFMT(LogTemp, Error, "게임모드가 유효하지 않습니다. ASoulLikeGameMode인지 확인하세요");
		}
	}
}

void UBonfireComponent::InitializeComponent()
{
	Super::InitializeComponent();
}


void UBonfireComponent::Rest(APlayerCharacter* Player)
{
	OnRest.Broadcast(Player);

	if (auto instance = Cast<USoulLikeInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		instance->SaveWithLastSavePoint(Player, this);
	}
}

void UBonfireComponent::OnBonfireTeleportListWidgetVisibilityChangedEvent(ESlateVisibility InVisibility)
{
	if (BonfireTeleportListWidget.IsValid())
	{
		if (!BonfireTeleportListWidget->IsVisible())
		{
			BonfireTeleportListWidget = nullptr;
		}
	}
}

void UBonfireComponent::CreateTeleportListWidget(APlayerCharacter* Player)
{
	if (BonfireTeleportListWidget == nullptr)
	{
		if (Player)
		{
			if (BonfireTeleportListWidgetObject)
			{
				if (const auto pc = Player->GetController<APlayerController>())
				{
					if (BonfireTeleportListWidget.IsValid() == false)
					{
						BonfireTeleportListWidget = CreateWidget<UBonfireTeleportWidget>(pc,
							BonfireTeleportListWidgetObject);
						BonfireTeleportListWidget->Button_Cancel->OnClicked.AddUniqueDynamic(
							GetOwner<ANPCBase>(), &ANPCBase::FinishInteraction);
					}

					BonfireTeleportListWidget->OnVisibilityChanged.AddUniqueDynamic(
						this, &UBonfireComponent::OnBonfireTeleportListWidgetVisibilityChangedEvent);
				}
			}
		}
	}
}

void UBonfireComponent::AddTeleportListWidget(APlayerCharacter* Player)
{
	CreateTeleportListWidget(Player);
	if (BonfireTeleportListWidget->IsInViewport() == false)
	{
		BonfireTeleportListWidget->AddToViewport();
	}

	BonfireTeleportListWidget->CreateList(this);
}

void UBonfireComponent::Recover(APlayerCharacter* Player)
{
	if (auto attComp = Player->GetAttributeComponent())
	{
		attComp->SetHP(attComp->GetMaxHP());
		attComp->SetMP(attComp->GetMaxMP());
		attComp->SetSP(attComp->GetMaxSP());

		attComp->InitProgressWidget();
	}
}

void UBonfireComponent::PotionReplenishment(APlayerCharacter* Player)
{
	UItemHelperLibrary::PotionReplenishment(Player);
}
