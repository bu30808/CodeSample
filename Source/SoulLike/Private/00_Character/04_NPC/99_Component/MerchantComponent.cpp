// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/04_NPC/99_Component/MerchantComponent.h"

#include "00_Character/00_Player/00_Controller/00_Component/WidgetManagerComponent.h"
#include "00_Character/04_NPC/NPCBase.h"
#include "03_Widget/04_Merchant/MerchantWidget.h"
#include "00_Character/00_Player/00_Controller/UserController.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

FMerchandiseItem::FMerchandiseItem(ANPCBase* Owner, FGuid Guid, const FMerchandiseItemData& Goods)
{
	if (Owner != nullptr)
	{
		MerchantNPC = Cast<ANPCBase>(Owner);
		UniqueID = Guid;
		MerchandiseData = Goods;

		if (SpawndItemActor == nullptr)
		{
			FActorSpawnParameters spawnParams;
			spawnParams.Owner = Owner;
			spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			if (auto load = Goods.ItemActorObject.LoadSynchronous())
			{
				SpawndItemActor = Owner->GetWorld()->SpawnActor<AItemActor>(load, spawnParams);
				SpawndItemActor->SetActorEnableCollision(false);
				SpawndItemActor->SetActorHiddenInGame(true);
			}else
			{
				UE_LOGFMT(LogTemp,Error,"{0} {1} , 아이템 액터 로드에 실패했습니다",__FUNCTION__,__LINE__);
			}
		}
	}
}

const FItemInformation* FMerchandiseItem::GetItemInformation() const
{
	if (SpawndItemActor != nullptr)
	{
		return SpawndItemActor->GetItemInformation();
	}

	UE_LOGFMT(LogTemp, Error, "아이템 액터가 유효하지 않아 아이템 정보를 가져올 수 없습니다");

	return nullptr;
}

FText FMerchandiseItem::GetFormattedDescription() const
{
	//UE_LOGFMT(LogTemp,Log,"아이템 설명을 가져오려 시도합니다.");
	if (SpawndItemActor != nullptr)
	{
		//UE_LOGFMT(LogActorComponent,Log,"설명을 가져오는 클래스 타입 : {0}",SpawndItemActor->GetClass()->GetName());
		return SpawndItemActor->GetFormattedDescription();
	}

	return FText::FromString("NOT VALID");
}

FMerchandiseAbility::FMerchandiseAbility(ANPCBase* Owner, const FMerchandiseAbilityData& AbilityData)
{
	if (Owner != nullptr)
	{
		MerchantNPC = Cast<ANPCBase>(Owner);
		MerchandiseAbilityData = AbilityData;
	}
}

const FAbilityInformation* FMerchandiseAbility::GetAbilityInformation() const
{
	if (MerchandiseAbilityData.AbilityObject.IsValid())
	{
		const auto ability = MerchandiseAbilityData.AbilityObject.LoadSynchronous();
		return &ability->GetDefaultObject<UAbilityBase>()->GetAbilityInformation();
	}

	return nullptr;
}

FText FMerchandiseAbility::GetAbilityDescription() const
{
	return GetAbilityInformation()->AbilityDescription;
}

FText FMerchandiseAbility::GetAbilityReqDescription() const
{
	return GetAbilityInformation()->GetAbilityReqDescription();
}

UMerchantComponent::UMerchantComponent()
{
	static ConstructorHelpers::FClassFinder<UMerchantWidget> widget(TEXT(
		"/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/02_Widget/Merchant/UMG_Merchant.UMG_Merchant_C'"));
	if (widget.Succeeded())
	{
		MerchantWidgetObject = widget.Class;
	}
}

void UMerchantComponent::BeginPlay()
{
	Super::BeginPlay();

	if (auto instance = Cast<USoulLikeInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		OnBuyItemFromPlayer.AddUniqueDynamic(instance, &USoulLikeInstance::OnNPCBuyItemFromPlayerEvent);
	}

	
}

void UMerchantComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	for(const auto& iter : MerchandiseItem)
	{
		if(auto item = iter.Value.GetSpawndItemActor())
		{
			item->Destroy();
		}
	}

	MerchandiseItem.Empty();
}

void UMerchantComponent::CreateSellItemList(UDataTable* ItemTable)
{
	UE_LOGFMT(LogTemp, Log, "상점에서 팔 아이템 리스트를 생성합니다 : {0}",GetNameSafe(GetOwner()));
	MerchandiseItemTable = ItemTable;
	if (MerchandiseItemTable)
	{
		TArray<FMerchandiseItemData*> out;
		MerchandiseItemTable->GetAllRows<FMerchandiseItemData>("", out);

		for (auto m : out)
		{
			UE_LOGFMT(LogTemp, Log, "상품 아이템 추가 : {0}", m->Tag.ToString());
			AddMerchandise(*m);
		}

		/*for(auto iter : MerchandiseItem)
		{
			UE_LOGFMT(LogTemp,Log,"저장된 상품 체크 : {0} {1}",iter.Value.GetItemInformation()->Item_Name.ToString(),iter.Value.GetItemInformation()->Item_Image.LoadSynchronous()->GetName());
		}*/
	}
}

void UMerchantComponent::CreateSellAbilityList(UDataTable* AbilityTable)
{
	UE_LOGFMT(LogTemp, Log, "상점에서 팔 어빌리티 리스트를 생성합니다.");
	MerchandiseAbilityTable = AbilityTable;
	if (MerchandiseAbilityTable)
	{
		TArray<FMerchandiseAbilityData*> out;
		MerchandiseAbilityTable->GetAllRows<FMerchandiseAbilityData>("", out);

		for (auto m : out)
		{
			//UE_LOGFMT(LogTemp,Log,"상품 어빌리티 추가 : {0}",m->Tag.ToString());
			AddMerchandise(*m);
		}
	}
}

void UMerchantComponent::CreateMerchantWidget(const ABaseCharacter* InteractPlayer)
{
	if (MerchantWidget == nullptr)
	{
		if (MerchantWidgetObject)
		{
			if (const auto pc = InteractPlayer->GetController<AUserController>())
			{
				MerchantWidget = Cast<UMerchantWidget>(pc->GetWidgetManagerComponent()->AddWidget(FGameplayTag::RequestGameplayTag("Widget.Merchant"),1,false,false));
				MerchantWidget->AddToViewport();
				
				MerchantWidget->SetKeyboardFocus();  // 키보드 포커스를 설정합니다.
				MerchantWidget->SetUserFocus(pc);  // 플레이어의 포커스를 설정합니다.

				MerchantWidget->OnVisibilityChanged.AddUniqueDynamic(
					this, &UMerchantComponent::OnMerchantWidgetVisibilityChangedEvent);
				MerchantWidget->Button_Close->OnClicked.AddUniqueDynamic(
							GetOwner<ANPCBase>(), &ANPCBase::FinishInteraction);
				MerchantWidget->SetOwnerNPC(GetOwner<ANPCBase>());
			}
		}
	}
	
}

void UMerchantComponent::ShowMerchantWidget(const ABaseCharacter* InteractPlayer)
{
	CreateMerchantWidget(InteractPlayer);
	if(MerchantWidget.IsValid()){
		MerchantWidget->CreateMerchandiseList(this);
	}
}

void UMerchantComponent::OnMerchantWidgetVisibilityChangedEvent(ESlateVisibility InVisibility)
{
	if (MerchantWidget.IsValid())
	{
		if (!MerchantWidget->IsVisible())
		{
			MerchantWidget = nullptr;
		}
	}
}

void UMerchantComponent::AddMerchandise(const FMerchandiseItemData& Goods)
{
	const FGuid newID = FGuid::NewGuid();
	MerchandiseItem.Add(newID, FMerchandiseItem(GetOwner<ANPCBase>(), newID, Goods));
}

void UMerchantComponent::AddMerchandise(const FMerchandiseAbilityData& Ability)
{
	if (Ability.AbilityObject != nullptr)
	{
		if (auto ab = Ability.AbilityObject.LoadSynchronous())
		{
			UE_LOGFMT(LogTemp, Log, "상점 아이템 어빌리티 추가 : {0} {1} ",ab->GetDefaultObject<UAbilityBase>()->GetAbilityTag().ToString(),ab->GetDefaultObject<UAbilityBase>()->GetAbilityInformation().GetAbilityReqDescription().ToString());
			MerchandiseAbility.Add(ab->GetDefaultObject<UAbilityBase>()->GetAbilityTag(),
			                       FMerchandiseAbility(GetOwner<ANPCBase>(), Ability));
		}
	}
	else
	{
		UE_LOGFMT(LogTemp, Error, "상점에 추가하려는 어빌리티 오브젝트가 유효하지 않습니다.");
	}
}

void UMerchantComponent::AddRepurchaseItem(const FInventoryItem& Item, int32 BuyCount)
{
	auto tag = Item.GetItemInformation()->Item_Tag;
	auto itemActor = Item.GetSpawndItemActor();
	auto price = Item.GetItemInformation()->Item_Price;
	auto id = FGuid::NewGuid();

	UE_LOGFMT(LogTemp, Warning, "재구매 상품을 추가합니다 : {0} {1}", id.ToString(), tag.ToString());

	RepurchaseMerchandiseItem.Add(id, FMerchandiseItem(GetOwner<ANPCBase>(), id,
	                                                   FMerchandiseItemData(tag, itemActor, price, BuyCount)));
}

void UMerchantComponent::AddRepurchaseAbility(const FMerchandiseAbility& Ability)
{
	auto tag = Ability.MerchandiseAbilityData.Tag;
	UE_LOGFMT(LogTemp, Warning, "재구매 어빌리티를 추가합니다 : {0}", tag.ToString());
	RepurchaseMerchandiseAbility.Add(tag, Ability);
}

bool UMerchantComponent::IsMerchandiseItem(FGuid ItemUniqueID)
{
	return MerchandiseItem.Contains(ItemUniqueID);
}

bool UMerchantComponent::IsRepurchaseMerchandiseItem(FGuid ItemUniqueID)
{
	return RepurchaseMerchandiseItem.Contains(ItemUniqueID);
}

const FMerchandiseItem& UMerchantComponent::GetMerchandiseItem(FGuid ItemUniqueID)
{
	return MerchandiseItem[ItemUniqueID];
}

const FMerchandiseItem& UMerchantComponent::GetRepurchaseMerchandiseItem(FGuid ItemUniqueID)
{
	return RepurchaseMerchandiseItem[ItemUniqueID];
}

bool UMerchantComponent::DecreaseMerchandiseItemCount(FGuid ItemUniqueID, int32 Count)
{
	if (MerchandiseItem.Contains(ItemUniqueID))
	{
		if (MerchandiseItem[ItemUniqueID].MerchandiseData.bSellInfinite)
		{
			return true;
		}

		if (MerchandiseItem[ItemUniqueID].MerchandiseData.Count >= Count)
		{
			MerchandiseItem[ItemUniqueID].MerchandiseData.Count -= Count;
			return true;
		}
	}
	return false;
}

bool UMerchantComponent::DecreaseRepurchaseMerchandiseItemCount(FGuid ItemUniqueID, int32 Count)
{
	if (RepurchaseMerchandiseItem.Contains(ItemUniqueID))
	{
		if (RepurchaseMerchandiseItem[ItemUniqueID].MerchandiseData.Count >= Count)
		{
			RepurchaseMerchandiseItem[ItemUniqueID].MerchandiseData.Count -= Count;

			if (RepurchaseMerchandiseItem[ItemUniqueID].MerchandiseData.Count == 0)
			{
				RepurchaseMerchandiseItem.Remove(ItemUniqueID);
			}
			return true;
		}
	}
	else
	{
		UE_LOGFMT(LogTemp, Error, "재구매 상품을 찾을 수 없습니다.: {0} ", ItemUniqueID.ToString());
	}

	return false;
}

bool UMerchantComponent::IsMerchandiseAbility(FGameplayTag AbilityTag)
{
	return MerchandiseAbility.Contains(AbilityTag);
}

bool UMerchantComponent::IsRepurchaseMerchandiseAbility(FGameplayTag AbilityTag)
{
	return RepurchaseMerchandiseAbility.Contains(AbilityTag);
}

const FMerchandiseAbility& UMerchantComponent::GetMerchandiseAbility(FGameplayTag AbilityTag)
{
	return MerchandiseAbility[AbilityTag];
}

const FMerchandiseAbility& UMerchantComponent::GetRepurchaseMerchandiseAbility(FGameplayTag AbilityTag)
{
	return RepurchaseMerchandiseAbility[AbilityTag];
}

bool UMerchantComponent::DecreaseMerchandiseAbilityCount(FGameplayTag AbilityTag, int32 Count)
{
	if (MerchandiseAbility.Contains(AbilityTag))
	{
		if (MerchandiseAbility[AbilityTag].MerchandiseAbilityData.Count >= Count)
		{
			MerchandiseAbility[AbilityTag].MerchandiseAbilityData.Count -= Count;
			return true;
		}
	}
	return false;
}

bool UMerchantComponent::DecreaseRepurchaseMerchandiseAbilityCount(FGameplayTag AbilityTag, int32 Count)
{
	if (RepurchaseMerchandiseAbility.Contains(AbilityTag))
	{
		if (RepurchaseMerchandiseAbility[AbilityTag].MerchandiseAbilityData.Count >= Count)
		{
			RepurchaseMerchandiseAbility[AbilityTag].MerchandiseAbilityData.Count -= Count;

			if (RepurchaseMerchandiseAbility[AbilityTag].MerchandiseAbilityData.Count == 0)
			{
				RepurchaseMerchandiseAbility.Remove(AbilityTag);
			}
			return true;
		}
	}
	else
	{
		UE_LOGFMT(LogTemp, Error, "재구매 어빌리티를 찾을 수 없습니다.: {0} ", AbilityTag.ToString());
	}

	return false;
}
