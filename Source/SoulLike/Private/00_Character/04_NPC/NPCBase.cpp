// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/04_NPC/NPCBase.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/04_NPC/99_Component/BonfireComponent.h"
#include "00_Character/04_NPC/99_Component/EnhancementComponent.h"
#include "00_Character/04_NPC/99_Component/LevelUpComponent.h"
#include "00_Character/04_NPC/99_Component/MerchantComponent.h"
#include "00_Character/04_NPC/99_Component/OrbMatrixSlotOpenComponent.h"
#include "00_Character/04_NPC/99_Component/PotionEnhancementComponent.h"

#include "96_Library/WidgetHelperLibrary.h"
#include "97_Interface/00_NPC/MerchantInterface.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Logging/StructuredLog.h"
#include "03_Widget/07_NPC/NPCWidget.h"
#include "94_Task/GameplayTask_LoadAsync.h"
#include "96_Library/ComponentHelperLibrary.h"


ANPCBase::ANPCBase()
{
	GetCapsuleComponent()->SetCollisionProfileName("NPC");
	GetMesh()->SetCollisionProfileName("NoCollision");

	//MerchantComponent = CreateDefaultSubobject<UMerchantComponent>("MerchantComponent");
}

void ANPCBase::BeginPlay()
{
	Super::BeginPlay();
}

void ANPCBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	for (auto iter : NPCActions)
	{
		switch (iter)
		{
		case ENPCActionType::Talk:
			break;
		case ENPCActionType::Merchant:
			if (UKismetSystemLibrary::DoesImplementInterface(this, UMerchantInterface::StaticClass()))
			{
				if (MerchantComponent == nullptr)
				{
					// 액터에 커스텀 액터 컴포넌트 생성
					MerchantComponent = NewObject<UMerchantComponent>(this, TEXT("MerchantComponent"));
					MerchantComponent->RegisterComponent();
				}

				{
					auto task = UGameplayTask_LoadAsync::LoadSoftObject(this, MerchantItemPath);
					task->OnLoadCompleted.AddUniqueDynamic(this, &ANPCBase::OnLoadedItemList);
					task->ReadyForActivation();
				}
				{
					auto task = UGameplayTask_LoadAsync::LoadSoftObject(this, MerchantAbilityPath);
					task->OnLoadCompleted.AddUniqueDynamic(this, &ANPCBase::OnLoadedAbilityList);
					task->ReadyForActivation();
				}
			}

			break;
		case ENPCActionType::LevelUp:

			if (LevelUpComponent == nullptr)
			{
				// 액터에 커스텀 액터 컴포넌트 생성
				LevelUpComponent = UComponentHelperLibrary::CreateActorComponent<ULevelUpComponent>(
					this,TEXT("LevelUpComponent"));
			}

			break;
		case ENPCActionType::Enhancement:
			if (EnhancementComponent == nullptr)
			{
				// 액터에 커스텀 액터 컴포넌트 생성
				EnhancementComponent = UComponentHelperLibrary::CreateActorComponent<UEnhancementComponent>(
					this, TEXT("EnhancementComponent"));
			}
			break;
		case ENPCActionType::PotionEnhancement:
			if (PotionEnhancementComponent == nullptr)
			{
				// 액터에 커스텀 액터 컴포넌트 생성
				PotionEnhancementComponent = UComponentHelperLibrary::CreateActorComponent<UPotionEnhancementComponent>(
					this, TEXT("PotionEnhancementComponent"));
			}
			break;
		case ENPCActionType::OrbMatrixSlotOpen:
			if (OrbMatrixSlotOpenComponent == nullptr)
			{
				// 액터에 커스텀 액터 컴포넌트 생성
				OrbMatrixSlotOpenComponent = UComponentHelperLibrary::CreateActorComponent<UOrbMatrixSlotOpenComponent>(
					this, TEXT("OrbMatrixSlotOpenComponent"));
			}

			break;
		case ENPCActionType::TeleportBonfire:
			if (BonfireComponent == nullptr)
			{
				// 액터에 커스텀 액터 컴포넌트 생성
				BonfireComponent = UComponentHelperLibrary::CreateActorComponent<UBonfireComponent>(
					this, TEXT("BonfireComponent"));
			}
			break;
		default: ;
		}
	}
}

void ANPCBase::Interaction_Implementation(ABaseCharacter* Start)
{
	IInteractionInterface::Interaction_Implementation(Start);

	InteractionActor = Start;

	for (auto iter : NPCActions)
	{
		switch (iter)
		{
		case ENPCActionType::Talk:
			break;
		case ENPCActionType::Merchant:
			MerchantComponent->CreateMerchantWidget(InteractionActor.Get());
			break;
		case ENPCActionType::LevelUp:
			LevelUpComponent->CreateLevelUpWidget();
			break;
		case ENPCActionType::Enhancement:
			EnhancementComponent->CreateEnhancementWidget(InteractionActor.Get());
			break;
		case ENPCActionType::PotionEnhancement:
			PotionEnhancementComponent->CreatePotionEnhancementWidget(InteractionActor.Get());
			break;
		case ENPCActionType::OrbMatrixSlotOpen:
			OrbMatrixSlotOpenComponent->CreateOrbMatrixSlotOpenWidget(InteractionActor.Get());
			break;
		case ENPCActionType::TeleportBonfire:
			BonfireComponent->CreateTeleportListWidget(Cast<APlayerCharacter>(InteractionActor.Get()));
			break;
		default: ;
		}
	}
}

void ANPCBase::FinishInteraction_Implementation()
{
	//IInteractionInterface::FinishInteraction_Implementation();

	if (InteractionActor.IsValid())
	{
		if (auto pc = InteractionActor->GetController<APlayerController>())
		{
			UWidgetHelperLibrary::CloseWidgetSetting(pc, nullptr);
		}
	}

	InteractionActor = nullptr;
}

void ANPCBase::OnNPCWidgetVisibilityChangedEvent(ESlateVisibility InVisibility)
{
	if (NPCWidget.IsValid())
	{
		if (!NPCWidget->IsVisible())
		{
			NPCWidget = nullptr;
		}
	}
}

void ANPCBase::AddNPCWidget()
{
	if (InteractionActor.IsValid())
	{
		auto pc = InteractionActor->GetController<APlayerController>();
		if (NPCWidget.IsValid() == false)
		{
			NPCWidget = CreateWidget<UNPCWidget>(pc, NPCWidgetObject);
		}
		/*UWidgetHelperLibrary::OpenWidgetSetting(pc, NPCWidget.Get());*/

		NPCWidget->SetNPC(this);
		NPCWidget->SetNPCName(NPCName);
		NPCWidget->OnVisibilityChanged.AddUniqueDynamic(this, &ANPCBase::OnNPCWidgetVisibilityChangedEvent);

		if (NPCWidget->IsInViewport() == false)
		{
			NPCWidget->AddToViewport();
		}

		NPCWidget->CreateActionList();
		NPCWidget->BindAction();
	}
}

void ANPCBase::TalkEvent_Implementation()
{
	//아직 없음
}

void ANPCBase::MerchantEvent_Implementation()
{
	if (NPCWidget.IsValid())
	{
		NPCWidget->SetVisibility(ESlateVisibility::Collapsed);
		NPCWidget = nullptr;
		MerchantComponent->ShowMerchantWidget(InteractionActor.Get());
	}
}

void ANPCBase::LevelUpEvent_Implementation()
{
	if (NPCWidget.IsValid())
	{
		NPCWidget->SetVisibility(ESlateVisibility::Collapsed);
		NPCWidget = nullptr;
	}

	if (LevelUpComponent == nullptr)
	{
		// 액터에 커스텀 액터 컴포넌트 생성
		LevelUpComponent = UComponentHelperLibrary::CreateActorComponent<ULevelUpComponent>(
			this,TEXT("LevelUpComponent"));
	}

	LevelUpComponent->ShowLevelUpWidget();
}

void ANPCBase::ExitEvent_Implementation()
{
	if (NPCWidget.IsValid())
	{
		NPCWidget->SetVisibility(ESlateVisibility::Collapsed);
		NPCWidget = nullptr;
		//UWidgetHelperLibrary::CloseWidgetSetting(InteractionActor->GetController<APlayerController>(), NPCWidget.Get());
		//NPCWidget->RemoveFromParent();
	}
}

void ANPCBase::EnhancementEvent_Implementation()
{
	if (NPCWidget.IsValid())
	{
		NPCWidget->SetVisibility(ESlateVisibility::Collapsed);
		NPCWidget = nullptr;
	}

	if (EnhancementComponent == nullptr)
	{
		// 액터에 커스텀 액터 컴포넌트 생성
		EnhancementComponent = UComponentHelperLibrary::CreateActorComponent<UEnhancementComponent>(
			this, TEXT("EnhancementComponent"));
	}

	EnhancementComponent->AddEnhancementWidget(InteractionActor.Get());
}

void ANPCBase::PotionEnhancementEvent_Implementation()
{
	if (NPCWidget.IsValid())
	{
		NPCWidget->SetVisibility(ESlateVisibility::Collapsed);
		NPCWidget = nullptr;
	}

	if (PotionEnhancementComponent == nullptr)
	{
		// 액터에 커스텀 액터 컴포넌트 생성
		PotionEnhancementComponent = UComponentHelperLibrary::CreateActorComponent<UPotionEnhancementComponent>(
			this, TEXT("PotionEnhancementComponent"));
	}
	PotionEnhancementComponent->AddPotionEnhancementWidget(InteractionActor.Get());
}

void ANPCBase::OrbMatrixSlotOpenEvent_Implementation()
{
	if (NPCWidget.IsValid())
	{
		NPCWidget->SetVisibility(ESlateVisibility::Collapsed);
		NPCWidget = nullptr;
	}

	if (OrbMatrixSlotOpenComponent == nullptr)
	{
		// 액터에 커스텀 액터 컴포넌트 생성
		OrbMatrixSlotOpenComponent = UComponentHelperLibrary::CreateActorComponent<UOrbMatrixSlotOpenComponent>(
			this, TEXT("OrbMatrixSlotOpenComponent"));
	}

	OrbMatrixSlotOpenComponent->AddOrbMatrixSlotOpenWidget(InteractionActor.Get());
}

void ANPCBase::OnTeleportWidgetOpenEvent_Implementation()
{
	//이동 가능한 다른 화톳불 목록을 보여줍니다.
	if (NPCWidget.IsValid())
	{
		NPCWidget->SetVisibility(ESlateVisibility::Collapsed);
		NPCWidget = nullptr;
	}

	if (BonfireComponent == nullptr)
	{
		// 액터에 커스텀 액터 컴포넌트 생성
		BonfireComponent = UComponentHelperLibrary::CreateActorComponent<UBonfireComponent>(
			this, TEXT("BonfireComponent"));
	}

	BonfireComponent->AddTeleportListWidget(Cast<APlayerCharacter>(InteractionActor.Get()));
}

void ANPCBase::BonfireEvent_Implementation()

{
	if (NPCWidget.IsValid())
	{
		NPCWidget->SetVisibility(ESlateVisibility::Collapsed);
		NPCWidget = nullptr;
	}


	if (BonfireComponent == nullptr)
	{
		// 액터에 커스텀 액터 컴포넌트 생성
		BonfireComponent = UComponentHelperLibrary::CreateActorComponent<UBonfireComponent>(
			this, TEXT("BonfireComponent"));
	}

	BonfireComponent->Rest(Cast<APlayerCharacter>(InteractionActor.Get()));
}

FOnButtonClickedEvent ANPCBase::GetAction(ENPCActionType ActionType)
{
	FOnButtonClickedEvent OnButtonClickedEvent;
	switch (ActionType)
	{
	case ENPCActionType::Talk:
		OnButtonClickedEvent.AddUniqueDynamic(this, &ANPCBase::TalkEvent);
		break;
	case ENPCActionType::Merchant:
		OnButtonClickedEvent.AddUniqueDynamic(this, &ANPCBase::MerchantEvent);
		break;
	case ENPCActionType::LevelUp:
		OnButtonClickedEvent.AddUniqueDynamic(this, &ANPCBase::LevelUpEvent);
		break;
	case ENPCActionType::Enhancement:
		OnButtonClickedEvent.AddUniqueDynamic(this, &ANPCBase::EnhancementEvent);
		break;
	case ENPCActionType::PotionEnhancement:
		OnButtonClickedEvent.AddUniqueDynamic(this, &ANPCBase::PotionEnhancementEvent);
		break;
	case ENPCActionType::OrbMatrixSlotOpen:
		OnButtonClickedEvent.AddUniqueDynamic(this, &ANPCBase::OrbMatrixSlotOpenEvent);
		break;
	case ENPCActionType::TeleportBonfire:
		OnButtonClickedEvent.AddUniqueDynamic(this, &ANPCBase::OnTeleportWidgetOpenEvent);
		break;
	default: ;
	}

	return OnButtonClickedEvent;
}

void ANPCBase::OnLoadedItemList(UObject* ItemTable)
{
	if (MerchantComponent)
	{
		MerchantComponent->CreateSellItemList(Cast<UDataTable>(ItemTable));
	}
}

void ANPCBase::OnLoadedAbilityList(UObject* AbilityTable)
{
	if (MerchantComponent)
	{
		MerchantComponent->CreateSellAbilityList(Cast<UDataTable>(AbilityTable));
	}
}
