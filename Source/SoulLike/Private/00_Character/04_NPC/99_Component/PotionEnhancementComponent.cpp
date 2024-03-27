// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/04_NPC/99_Component/PotionEnhancementComponent.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "00_Character/04_NPC/NPCBase.h"
#include "03_Widget/06_Enhancement/00_Potion/PotionEnhancementWidget.h"
#include "04_Item/00_Consume/00_Potion/PotionItemActor.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "99_Subsystem/EnhancementSubsystem.h"
#include "Kismet/GameplayStatics.h"


class UEnhancementSubsystem;
// Sets default values for this component's properties
UPotionEnhancementComponent::UPotionEnhancementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
	static ConstructorHelpers::FClassFinder<UPotionEnhancementWidget> widget(TEXT(
		"/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/02_Widget/ItemEnhancement/Potion/UMG_PotionEnhancement.UMG_PotionEnhancement_C'"));
	if (widget.Succeeded())
	{
		PotionEnhancementWidgetObject = widget.Class;
	}

	// ...
}


// Called when the game starts
void UPotionEnhancementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UPotionEnhancementComponent::PostInitProperties()
{
	Super::PostInitProperties();
	if (GetWorld())
	{
		if (auto instance = Cast<USoulLikeInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
		{
			OnUpgradePotion.AddUniqueDynamic(instance, &USoulLikeInstance::OnUpgradePotionEvent);
			if (const auto system = instance->GetSubsystem<UEnhancementSubsystem>())
			{
				PotionEnhancementMaterial = system->GetPotionEnhancementMaterial();
			}
		}
		
	}
}

void UPotionEnhancementComponent::OnPotionEnhancementWidgetVisibilityChangedEvent(ESlateVisibility InVisibility)
{
	if (PotionEnhancementWidget.IsValid())
	{
		if (!PotionEnhancementWidget->IsVisible())
		{
			PotionEnhancementWidget = nullptr;
		}
	}
}

void UPotionEnhancementComponent::CreatePotionEnhancementWidget(const ABaseCharacter* InteractPlayer)
{
	if (PotionEnhancementWidget == nullptr)
	{
		if (PotionEnhancementWidgetObject)
		{
			if (const auto pc = InteractPlayer->GetController<APlayerController>())
			{
				if (PotionEnhancementWidget.IsValid() == false)
				{
					PotionEnhancementWidget = CreateWidget<UPotionEnhancementWidget>(pc,
						PotionEnhancementWidgetObject);
					PotionEnhancementWidget->Button_Close->OnClicked.AddUniqueDynamic(
						GetOwner<ANPCBase>(), &ANPCBase::FinishInteraction);
				}
				PotionEnhancementWidget->OnVisibilityChanged.AddUniqueDynamic(
					this, &UPotionEnhancementComponent::OnPotionEnhancementWidgetVisibilityChangedEvent);
				PotionEnhancementWidget->SetPotionEnhancementComponent(this);
			}
		}
	}
}


void UPotionEnhancementComponent::AddPotionEnhancementWidget(const ABaseCharacter* InteractPlayer)
{
	CreatePotionEnhancementWidget(InteractPlayer);

	if (PotionEnhancementWidget->IsInViewport() == false)
	{
		PotionEnhancementWidget->AddToViewport();
	}

	/*UWidgetHelperLibrary::OpenWidgetSetting(pc, PotionEnhancementWidget.Get());*/
}

const FEnhancementMaterial& UPotionEnhancementComponent::GetNextEnhancementInfo(const int32& CurEnhancement)
{
	return *PotionEnhancementMaterial[CurEnhancement];
}

bool UPotionEnhancementComponent::CanEnhancePotion(UInventoryComponent* InventoryComponent,EEnhancedPotionType EnhancedPotionType)
{
	if (InventoryComponent)
	{
		FGameplayTag potionTag;
		switch (EnhancedPotionType) {
		case EEnhancedPotionType::HP:
			potionTag = FGameplayTag::RequestGameplayTag("Item.Consume.Potion.Hp");
			break;
		case EEnhancedPotionType::MP:
			potionTag = FGameplayTag::RequestGameplayTag("Item.Consume.Potion.mp");
			break;
		}
		
		//강화소재가 있는지 확인합니다.
		if(auto instance =  Cast<USoulLikeInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
		{
			if (const auto system = instance->GetSubsystem<UEnhancementSubsystem>())
			{

				//해당 물약을 소지중인가요?
				if (auto potion = InventoryComponent->GetItemByTag(potionTag))
				{

					const auto attComp = InventoryComponent->GetOwner<ABaseCharacter>()->GetAttributeComponent();
					//현 강화정보
					const auto& enhanceInfo = static_cast<const FPotionInformation*>(potion->GetItemInformation())->Enhancement;
					//다음 업그레이드에 필요한 요구사항
					auto nextNeed = GetNextEnhancementInfo(enhanceInfo.CurEnhancement);

					//강화 수치가 맥스인가?
					if (bool bCan = enhanceInfo.CanEnhance(); bCan == false)
					{
						UE_LOGFMT(LogTemp, Error, "최대강화수치기 때문에 더 이상 강화 불가능");
						return false;
					}

					//2. 소지 금액이 충분한지 확인
					if (bool bEnoughExp = attComp->GetEXP() >= nextNeed.NeedSouls; bEnoughExp == false)
					{
						UE_LOGFMT(LogTemp, Error, "필요한 경험치량이 모자람");
						return false;
					}

					//3. 필요 아이템이 모두 있는지 확인
					for (auto iter : nextNeed.NeedItems)
					{
						if (InventoryComponent->K2_HasItem(iter.Key))
						{
							const FInventoryItem* item = InventoryComponent->GetItemByTag(iter.Key);
							if (item->ItemCount < iter.Value)
							{
								UE_LOGFMT(LogTemp, Error, "필요한 아이템의 갯수가 모자람 :{0}", item->GetItemInformation()->Item_Name.ToString());
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
		

			}
		}
	}

	return false;
}

void UPotionEnhancementComponent::UpgradePotion(UInventoryComponent* InventoryComponent,EEnhancedPotionType EnhancedPotionType)
{
	if (InventoryComponent)
	{
		if (CanEnhancePotion(InventoryComponent,EnhancedPotionType))
		{
			const FInventoryItem* potionItem = nullptr;
			switch (EnhancedPotionType) {
			case EEnhancedPotionType::HP:
				potionItem = GetHPPotionItem(InventoryComponent);
				break;
			case EEnhancedPotionType::MP:
				potionItem = GetMPPotionItem(InventoryComponent);
				break;
			}
			
			if ( potionItem != nullptr )
			{

				
				if (const auto potionActor = Cast<APotionItemActor>(potionItem->GetSpawndItemActor()))
				{
					//현 강화상황
					const auto& enhanceInfo = static_cast<const FPotionInformation*>(potionItem->GetItemInformation())->Enhancement;
					//다음 업그레이드에 필요한 요구사항
					auto nextNeed = GetNextEnhancementInfo(enhanceInfo.CurEnhancement);
					
					potionActor->IncreaseEnhance();
					//강화에 필요한 소재를 소모합니다.
					for (auto iter : nextNeed.NeedItems)
					{
						auto matID = InventoryComponent->GetItemByTag(iter.Key);
						InventoryComponent->DecreaseItemCount(matID->UniqueID,iter.Value);
					}
					OnUpgradePotion.Broadcast(potionItem->UniqueID, potionActor);
				}
			}
		}
	}
}


const FInventoryItem* UPotionEnhancementComponent::GetHPPotionItem(UInventoryComponent* InventoryComponent)
{
	if (const auto potion = InventoryComponent->
		GetItemByTag(FGameplayTag::RequestGameplayTag("Item.Consume.Potion.Hp")))
	{
		return potion;
	}

	return nullptr;
}

const FInventoryItem* UPotionEnhancementComponent::GetMPPotionItem(UInventoryComponent* InventoryComponent)
{
	if (const auto potion = InventoryComponent->
		GetItemByTag(FGameplayTag::RequestGameplayTag("Item.Consume.Potion.Mp")))
	{
		return potion;
	}

	return nullptr;
}

