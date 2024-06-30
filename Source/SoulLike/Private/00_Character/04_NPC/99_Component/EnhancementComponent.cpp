// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/04_NPC/99_Component/EnhancementComponent.h"

#include "00_Character/01_Component/AttributeComponent.h"
#include "00_Character/04_NPC/NPCBase.h"
#include "03_Widget/06_Enhancement/EnhancementWidget.h"
#include "04_Item/ItemActor.h"
#include "04_Item/01_Equipment/EquipmentItemActor.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "99_Subsystem/EnhancementSubsystem.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values for this component's properties
UEnhancementComponent::UEnhancementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	static ConstructorHelpers::FClassFinder<UEnhancementWidget> widget(TEXT(
		"/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/02_Widget/ItemEnhancement/UMG_Enhancement.UMG_Enhancement_C'"));
	if (widget.Succeeded())
	{
		EnhancementWidgetObject = widget.Class;
	}
}


// Called when the game starts
void UEnhancementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UEnhancementComponent::PostInitProperties()
{
	Super::PostInitProperties();

	if (GetWorld())
	{
		if (auto instance = Cast<USoulLikeInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
		{
			OnUpgradeEquipment.AddUniqueDynamic(instance, &USoulLikeInstance::OnUpgradeEquipmentEvent);

			if (const auto system = instance->GetSubsystem<UEnhancementSubsystem>())
			{
				EnhancementMaterial = system->GetEquipmentEnhancementMaterial();
				AttributeValuePerEnhancement = system->GetAttributeValuePerEnhancement();
			}
		}
	}
}


void UEnhancementComponent::OnEnhancementWidgetVisibilityChangedEvent(ESlateVisibility InVisibility)
{
	if (EnhancementWidget.IsValid())
	{
		if (!EnhancementWidget->IsVisible())
		{
			EnhancementWidget = nullptr;
		}
	}
}

void UEnhancementComponent::CreateEnhancementWidget(const ABaseCharacter* InteractPlayer)
{
	if (EnhancementWidget == nullptr)
	{
		if (EnhancementWidgetObject)
		{
			if (const auto pc = InteractPlayer->GetController<APlayerController>())
			{
				if (EnhancementWidget.IsValid() == false)
				{
					EnhancementWidget = CreateWidget<UEnhancementWidget>(pc,
					                                                     EnhancementWidgetObject);
					EnhancementWidget->Button_Close->OnClicked.AddUniqueDynamic(
						GetOwner<ANPCBase>(), &ANPCBase::FinishInteraction);
				}
				EnhancementWidget->OnVisibilityChanged.AddUniqueDynamic(
					this, &UEnhancementComponent::OnEnhancementWidgetVisibilityChangedEvent);
				EnhancementWidget->SetEnhancementComponent(this);
			}
		}
	}
}

void UEnhancementComponent::AddEnhancementWidget(const class ABaseCharacter* InteractPlayer, EEnhancementType EnhancementType)
{
	CreateEnhancementWidget(InteractPlayer);
	
	if (EnhancementWidget->IsInViewport() == false)
	{
		EnhancementWidget->SetEnhancementType(EnhancementType);
		EnhancementWidget->AddToViewport();
	}
}

const FEnhancementMaterial& UEnhancementComponent::GetNextEnhancementInfo(const int32 CurEnhancement)
{
	return *EnhancementMaterial[CurEnhancement];
}

const TArray<FAttributeValuePerEnhancement*>& UEnhancementComponent::GetAttributeValuePerEnhancement()
{
	return AttributeValuePerEnhancement;
}

bool UEnhancementComponent::CanEnhance(const ABaseCharacter* Character, const FInventoryItem& Item)
{
	if (Character)
	{
		const auto attComp = Character->GetAttributeComponent();
		const auto& enhanceInfo = static_cast<const FEquipmentInformation*>(Item.GetItemInformation())->Enhancement;

		//1. 강화수치가 맥스를 찍었는지 확인
		bool bCan = enhanceInfo.CanEnhance();
		if (bCan == false)
		{
			UE_LOGFMT(LogTemp, Error, "최대강화수치기 때문에 더 이상 강화 불가능");
			return false;
		}

		const auto& nextNeed = GetNextEnhancementInfo(enhanceInfo.CurEnhancement);
		//2. 소지 금액이 충분한지 확인
		bool bEnoughExp = attComp->GetEXP() >= nextNeed.NeedSouls;
		if (bEnoughExp == false)
		{
			UE_LOGFMT(LogTemp, Error, "필요한 경험치량이 모자람");
			return false;
		}


		//3. 필요 아이템이 모두 있는지 확인
		auto invenComp = Character->GetInventoryComponent();
		for (auto iter : nextNeed.NeedItems)
		{
			if (invenComp->K2_HasItem(iter.Key))
			{
				const FInventoryItem* item = invenComp->GetItemByTag(iter.Key);
				if (item->ItemCount < iter.Value)
				{
					UE_LOGFMT(LogTemp, Error, "필요한 아이템의 갯수가 모자람 :{0}",
					          item->GetItemInformation()->Item_Name.ToString());
					return false;
				}
			}
			else
			{
				UE_LOGFMT(LogTemp, Error, "필요한 아이템을 찾을 수 없음 : {0}", iter.Key.ToString());
				return false;
			}
		}

		return true;
	}

	return false;
}

bool UEnhancementComponent::Upgrade(const ABaseCharacter* Character, const FGuid& UniqueID,
                                    EAttributeType IncreaseAttribute)
{
	UE_LOGFMT(LogTemp, Log, "강화11111111111111111111111");
	if (Character)
	{
		const auto attComp = Character->GetAttributeComponent();
		auto invenComp = Character->GetInventoryComponent();

		FInventoryItem findItem;
		//프래그먼트인 경우
		if (invenComp->GetFragments().Contains(UniqueID))
		{
			findItem = invenComp->GetFragments()[UniqueID];
		}
		else
		{
			//그 외의 경우
			findItem = invenComp->GetInventoryItem(UniqueID);
		}

		UE_LOGFMT(LogTemp, Log, "강화22222222222222222222");
		const auto& enhanceInfo = Cast<AEquipmentItemActor>(findItem.GetSpawndItemActor())->GetEnhancementInfo();
		const auto& nextNeed = GetNextEnhancementInfo(enhanceInfo.CurEnhancement);

		for (auto iter : nextNeed.NeedItems)
		{
			if (invenComp->K2_HasItem(iter.Key))
			{
				const FInventoryItem* item = invenComp->GetItemByTag(iter.Key);
				//소재를 제거합니다.
				invenComp->DecreaseItemCount(item->UniqueID, iter.Value);
			}
		}

		//경험치를 제거합니다.
		attComp->SetEXP(attComp->GetEXP() - nextNeed.NeedSouls);
		UE_LOGFMT(LogTemp, Log, "강화3333333333333333333333333");
		Cast<AEquipmentItemActor>(findItem.GetSpawndItemActor())->IncreaseEnhance(IncreaseAttribute);
		//TODO 강화된 장비 상태를 저장합니다.
		OnUpgradeEquipment.Broadcast(UniqueID, Cast<AEquipmentItemActor>(findItem.GetSpawndItemActor()));


		return true;
	}

	return false;
}
