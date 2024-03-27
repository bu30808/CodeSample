// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/04_NPC/99_Component/OrbMatrixSlotOpenComponent.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/00_Player/00_Controller/UserController.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "00_Character/04_NPC/NPCBase.h"
#include "03_Widget/01_Menu/05_Orb/OrbMatrixElementWidget.h"
#include "03_Widget/05_Alert/AlertWidget.h"
#include "03_Widget/07_NPC/OrbMatrixSlotOpenWidget.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

#define LOCTEXT_NAMESPACE "OrbMatrixSlotOpenComponent"
// Sets default values for this component's properties
UOrbMatrixSlotOpenComponent::UOrbMatrixSlotOpenComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	static ConstructorHelpers::FClassFinder<UOrbMatrixSlotOpenWidget> widget(TEXT(
		"/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/02_Widget/NPC/UMG_OrbSlotOpen.UMG_OrbSlotOpen_C'"));
	if (widget.Succeeded())
	{
		OrbMatrixSlotOpenWidgetObject = widget.Class;
	}

	static ConstructorHelpers::FObjectFinder<USlotOpenRewardDataAsset> dataAsset(TEXT(
		"/Script/SoulLike.SlotOpenRewardDataAsset'/Game/Blueprints/04_Item/Equip/Orb/SlotOpenReward/DA_OrbSlotOpenReward.DA_OrbSlotOpenReward'"));
	if (dataAsset.Succeeded())
	{
		SlotOpenRewardDataAsset = dataAsset.Object;
	}
}

// Called when the game starts
void UOrbMatrixSlotOpenComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UOrbMatrixSlotOpenComponent::PostInitProperties()
{
	Super::PostInitProperties();

	if (auto instance = Cast<USoulLikeInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		OnSlotOpen.AddUniqueDynamic(instance, &USoulLikeInstance::OnSlotOpenEvent);
	}
}


void UOrbMatrixSlotOpenComponent::OnOrbMatrixSlotOpenWidgetVisibilityChangedEvent(ESlateVisibility InVisibility)
{
	if (OrbMatrixSlotOpenWidget.IsValid())
	{
		if (!OrbMatrixSlotOpenWidget->IsVisible())
		{
			OrbMatrixSlotOpenWidget = nullptr;
		}
	}
}

void UOrbMatrixSlotOpenComponent::CreateOrbMatrixSlotOpenWidget(ABaseCharacter* InteractPlayer)
{
	if (OrbMatrixSlotOpenWidget == nullptr)
	{
		if (OrbMatrixSlotOpenWidgetObject && InteractPlayer != nullptr)
		{
			if (const auto pc = InteractPlayer->GetController<APlayerController>())
			{
				if (OrbMatrixSlotOpenWidget.IsValid() == false)
				{
					OrbMatrixSlotOpenWidget = CreateWidget<UOrbMatrixSlotOpenWidget>(pc,
						OrbMatrixSlotOpenWidgetObject);

					OrbMatrixSlotOpenWidget->Button_Close->OnClicked.AddUniqueDynamic(
						GetOwner<ANPCBase>(), &ANPCBase::FinishInteraction);
				}

				OrbMatrixSlotOpenWidget->OnVisibilityChanged.AddUniqueDynamic(
					this, &UOrbMatrixSlotOpenComponent::OnOrbMatrixSlotOpenWidgetVisibilityChangedEvent);
				OrbMatrixSlotOpenWidget->SetOrbMatrixSlotOpenComponent(this);
			}
		}
	}
}

void UOrbMatrixSlotOpenComponent::AddOrbMatrixSlotOpenWidget(ABaseCharacter* InteractPlayer)
{
	CreateOrbMatrixSlotOpenWidget(InteractPlayer);
	if (OrbMatrixSlotOpenWidget->IsInViewport() == false)
	{
		OrbMatrixSlotOpenWidget->AddToViewport();
	}
}

void UOrbMatrixSlotOpenComponent::UnLock(APlayerController* PlayerController,
                                         UOrbMatrix* CoreMatrix, const FOrbMatrixElementInfo& SelectedMatrixElement,
                                         UOrbMatrixElementWidget*
                                         SelectedMatrixElementWidget, const TArray<FOrbSlotUnlockCost*>& UnlockTable)
{
	//이미 열려있다면 아무것도 하지 않습니다.
	if (SelectedMatrixElement.bLock == false)
	{
		return;
	}


	//사본임.
	TArray<FOrbMatrixElementInfo> line = GetLine(CoreMatrix, SelectedMatrixElement.OrbMatrixSlotType);

	const auto& index = line.IndexOfByKey(SelectedMatrixElement);
	if (UnlockTable.IsValidIndex(index))
	{
		//이전 슬롯이 열려있는지 확인합니다.
		if (IsOpenPreSlot(line, index))
		{
			auto cost = UnlockTable[index]->Cost;
			//비용이 충분한지 확인합니다.
			if (HaveEnoughCost(PlayerController, cost))
			{
				//코스트를 지불합니다.
				if (const auto attComp = PlayerController->GetPawn<ABaseCharacter>()->GetAttributeComponent())
				{
					attComp->SetEXP(attComp->GetEXP() - cost);
				}
				//슬롯을 개방합니다.
				OpenSlot(CoreMatrix, SelectedMatrixElement);
				//개방한 슬롯을 위젯(슬롯 잠금해제 위젯)에 업데이트 합니다.
				SelectedMatrixElementWidget->Refresh(
					CoreMatrix->OrbMatrix[SelectedMatrixElement.Row][SelectedMatrixElement.Col]);

				//한 라인의 슬롯이 모두 개방된 경우 보상을 줍니다.
				GiveLineCompleteReward(PlayerController, CoreMatrix, SelectedMatrixElement.OrbMatrixSlotType);
				OnSlotOpen.Broadcast(PlayerController->GetPawn<APlayerCharacter>(), CoreMatrix);
			}
			else
			{
				const FText warningText =  NSLOCTEXT("OrbMatrixSlotOpenComponent","OrbMatrixSlotOpenCostWarningText","개방에 필요한 비용이 부족합니다.");
				UWidgetHelperLibrary::ShowAlertMsg(Cast<AUserController>(PlayerController), EAlertMsgType::Warning,
				                                   warningText, FOnButtonClicked());
			}
		}
		else
		{
			const FText warningText =  NSLOCTEXT("OrbMatrixSlotOpenComponent","OrbMatrixSlotOpenWarningText","이전 슬롯이 먼저 개방되어야 합니다.");
			UWidgetHelperLibrary::ShowAlertMsg(Cast<AUserController>(PlayerController), EAlertMsgType::Warning,
			                                   warningText, FOnButtonClicked());
		}
	}
}

bool UOrbMatrixSlotOpenComponent::IsAllOpen(const TArray<FOrbMatrixElementInfo>& Line)
{
	for (const auto& iter : Line)
	{
		if (iter.bLock)
		{
			return false;
		}
	}

	return true;
}

void UOrbMatrixSlotOpenComponent::GivePhysicalLineCompleteReward(APlayerController* PlayerController, bool bShowAlert)
{
	if (SlotOpenRewardDataAsset)
	{
		if (auto abComp = PlayerController->GetPawn<APlayerCharacter>()->GetAbilityComponent())
		{
			FString msg = "\n";

			for (const auto& iter : SlotOpenRewardDataAsset->PhysicalLineReward)
			{
				msg += iter.GetDefaultObject()->GetAbilityInformation().AbilityName.ToString() + "\n";
				abComp->GiveAbility(iter);
				abComp->ActivateAbility(iter.GetDefaultObject()->GetAbilityTag(), PlayerController->GetPawn());
			}


			if (bShowAlert)
			{
				const FText alertText =FText::Format( NSLOCTEXT("OrbMatrixSlotOpenComponent","PhysicalLineCompleteRewardText","물리 라인을 완성하여 다음 능력을 얻었습니다 : {0}"),FText::FromString(msg));
				UWidgetHelperLibrary::ShowAlertMsg(Cast<AUserController>(PlayerController), EAlertMsgType::NoUseDeco,
				                                  alertText, FOnButtonClicked());
			}
		}
	}
}

void UOrbMatrixSlotOpenComponent::GiveMagicalLineCompleteReward(APlayerController* PlayerController, bool bShowAlert)
{
	if (SlotOpenRewardDataAsset)
	{
		if (auto abComp = PlayerController->GetPawn<APlayerCharacter>()->GetAbilityComponent())
		{
			FString msg = "\n";
			for (const auto& iter : SlotOpenRewardDataAsset->MagicalLineReward)
			{
				msg += iter.GetDefaultObject()->GetAbilityInformation().AbilityName.ToString() + "\n";
				abComp->GiveAbility(iter);
				abComp->ActivateAbility(iter.GetDefaultObject()->GetAbilityTag(), PlayerController->GetPawn());
			}

			if (bShowAlert)
			{
				const FText alertText =FText::Format( NSLOCTEXT("OrbMatrixSlotOpenComponent","MagicalLineCompleteRewardText","마법 라인을 완성하여 다음 능력을 얻었습니다 : {0}"),FText::FromString(msg));
			
				UWidgetHelperLibrary::ShowAlertMsg(Cast<AUserController>(PlayerController), EAlertMsgType::NoUseDeco,
				                                 alertText, FOnButtonClicked());
			}
		}
	}
}

void UOrbMatrixSlotOpenComponent::GiveDefenceLineCompleteReward(APlayerController* PlayerController, bool bShowAlert)
{
	if (SlotOpenRewardDataAsset)
	{
		if (auto abComp = PlayerController->GetPawn<APlayerCharacter>()->GetAbilityComponent())
		{
			FString msg = "\n";
			for (const auto& iter : SlotOpenRewardDataAsset->DefenceLineReward)
			{
				msg += iter.GetDefaultObject()->GetAbilityInformation().AbilityName.ToString() + "\n";
				abComp->GiveAbility(iter);
				abComp->ActivateAbility(iter.GetDefaultObject()->GetAbilityTag(), PlayerController->GetPawn());
			}


			if (bShowAlert)
			{
				const FText alertText =FText::Format( NSLOCTEXT("OrbMatrixSlotOpenComponent","DefenceLineCompleteRewardText","방어 라인을 완성하여 다음 능력을 얻었습니다 : {0}"),FText::FromString(msg));
			
				UWidgetHelperLibrary::ShowAlertMsg(Cast<AUserController>(PlayerController), EAlertMsgType::NoUseDeco,
				                                  alertText, FOnButtonClicked());
			}
		}
	}
}

void UOrbMatrixSlotOpenComponent::GiveFreeLineCompleteReward(APlayerController* PlayerController, bool bShowAlert)
{
	if (SlotOpenRewardDataAsset)
	{
		if (auto abComp = PlayerController->GetPawn<APlayerCharacter>()->GetAbilityComponent())
		{
			FString msg = "\n";
			for (const auto& iter : SlotOpenRewardDataAsset->FreeLineReward)
			{
				msg += iter.GetDefaultObject()->GetAbilityInformation().AbilityName.ToString() + "\n";
				abComp->GiveAbility(iter);
				abComp->ActivateAbility(iter.GetDefaultObject()->GetAbilityTag(), PlayerController->GetPawn());
			}

			if (bShowAlert)
			{
				const FText alertText =FText::Format( NSLOCTEXT("OrbMatrixSlotOpenComponent","FreeLineCompleteRewardText","방어 라인을 완성하여 다음 능력을 얻었습니다 : {0}"),FText::FromString(msg));
			
				UWidgetHelperLibrary::ShowAlertMsg(Cast<AUserController>(PlayerController), EAlertMsgType::NoUseDeco,
				                                   alertText, FOnButtonClicked());
			}
		}
	}
}

void UOrbMatrixSlotOpenComponent::GiveLineCompleteReward(APlayerController* PlayerController,
                                                         const UOrbMatrix* OrbMatrix, EOrbMatrixSlotType OrbMatrixSlot,
                                                         bool bShowAlert)
{
	switch (OrbMatrixSlot)
	{
	case EOrbMatrixSlotType::PHYSICAL:
		if (IsAllOpen(OrbMatrix->PhysicalLine))
		{
			UE_LOGFMT(LogTemp, Warning, "물리 라인 완성 보너스 지급");
			GivePhysicalLineCompleteReward(PlayerController, bShowAlert);
		}
		break;
	case EOrbMatrixSlotType::MAGICAL:
		if (IsAllOpen(OrbMatrix->MagicalLine))
		{
			UE_LOGFMT(LogTemp, Warning, "마법 라인 완성 보너스 지급");
			GiveMagicalLineCompleteReward(PlayerController, bShowAlert);
		}
		break;
	case EOrbMatrixSlotType::DEFENCE:
		if (IsAllOpen(OrbMatrix->DefenceLine))
		{
			UE_LOGFMT(LogTemp, Warning, "방어 라인 완성 보너스 지급");
			GiveDefenceLineCompleteReward(PlayerController, bShowAlert);
		}
		break;
	case EOrbMatrixSlotType::FREE:
		if (IsAllOpen(OrbMatrix->FreeLine))
		{
			UE_LOGFMT(LogTemp, Warning, "자유 라인 완성 보너스 지급");
			GiveFreeLineCompleteReward(PlayerController, bShowAlert);
		}
		break;
	default: ;
	}
}

void UOrbMatrixSlotOpenComponent::OpenSlot(UOrbMatrix* CoreMatrix, const FOrbMatrixElementInfo& SelectedMatrixElement)
{
	switch (SelectedMatrixElement.OrbMatrixSlotType)
	{
	case EOrbMatrixSlotType::PHYSICAL:
		{
			const auto& index = CoreMatrix->PhysicalLine.IndexOfByKey(SelectedMatrixElement);
			CoreMatrix->PhysicalLine[index].bLock = false;
		}
		break;
	case EOrbMatrixSlotType::DEFENCE:
		{
			const auto& index = CoreMatrix->DefenceLine.IndexOfByKey(SelectedMatrixElement);
			CoreMatrix->DefenceLine[index].bLock = false;
		}
		break;
	case EOrbMatrixSlotType::MAGICAL:
		{
			const auto& index = CoreMatrix->MagicalLine.IndexOfByKey(SelectedMatrixElement);
			CoreMatrix->MagicalLine[index].bLock = false;
		}
		break;
	case EOrbMatrixSlotType::FREE:
		{
			const auto& index = CoreMatrix->FreeLine.IndexOfByKey(SelectedMatrixElement);
			CoreMatrix->FreeLine[index].bLock = false;
		}
		break;
	default: ;
	}

	CoreMatrix->OrbMatrix[SelectedMatrixElement.Row][SelectedMatrixElement.Col].bLock = false;
}

bool UOrbMatrixSlotOpenComponent::HaveEnoughCost(APlayerController* PlayerController, const int32& Cost)
{
	if (const auto attComp = PlayerController->GetPawn<ABaseCharacter>()->GetAttributeComponent())
	{
		return attComp->GetEXP() >= Cost;
	}

	return false;
}

bool UOrbMatrixSlotOpenComponent::IsOpenPreSlot(const TArray<FOrbMatrixElementInfo>& Line, const int32& Index)
{
	if (Line.IsValidIndex(Index - 1))
	{
		return !Line[Index - 1].bLock;
	}

	return false;
}

TArray<FOrbMatrixElementInfo> UOrbMatrixSlotOpenComponent::GetLine(UOrbMatrix* CoreMatrix,
                                                                   EOrbMatrixSlotType OrbMatrixSlot)
{
	switch (OrbMatrixSlot)
	{
	case EOrbMatrixSlotType::PHYSICAL:
		return CoreMatrix->PhysicalLine;
	case EOrbMatrixSlotType::DEFENCE:
		return CoreMatrix->DefenceLine;
	case EOrbMatrixSlotType::MAGICAL:
		return CoreMatrix->MagicalLine;
	case EOrbMatrixSlotType::FREE:
		return CoreMatrix->FreeLine;
	default: ;
	}

	return TArray<FOrbMatrixElementInfo>();
}
#undef LOCTEXT_NAMESPACE