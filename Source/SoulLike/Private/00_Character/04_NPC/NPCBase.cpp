// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/04_NPC/NPCBase.h"

#include "BrainComponent.h"
#include "NavigationInvokerComponent.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/04_NPC/99_Component/BonfireComponent.h"
#include "00_Character/04_NPC/99_Component/EnhancementComponent.h"
#include "00_Character/04_NPC/99_Component/LevelUpComponent.h"
#include "00_Character/04_NPC/99_Component/MerchantComponent.h"
#include "00_Character/04_NPC/99_Component/MatrixSlotOpenComponent.h"
#include "00_Character/04_NPC/99_Component/PotionEnhancementComponent.h"

#include "97_Interface/00_NPC/MerchantInterface.h"
#include "Components/CapsuleComponent.h"
#include "96_Library/ComponentHelperLibrary.h"

#include "00_Character/00_Player/00_Controller/00_Component/WidgetManagerComponent.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/01_Component/UROComponent.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "00_Character/04_NPC/NPCAIController.h"
#include "00_Character/04_NPC/99_Component/RegisterAbilityComponent.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Damage.h"


FNPCState::FNPCState(ANPCBase* NPC)
{
	bHasMet = NPC->HasMet();
	bJoinBasement = NPC->HasJoinNPCAtBasement();
	SafeName = GetNameSafe(NPC);
	NPCLocation = NPC->GetActorLocation();
	
}

FNPCState::FNPCState(ANPCBase* NPC, bool Destroyed) : FNPCState(NPC)
{
	bIsDestroyed = Destroyed;
}

void FNPCState::Update(ANPCBase* NPC)
{
	bHasMet = NPC->HasMet();
	bJoinBasement = NPC->HasJoinNPCAtBasement();
}

ANPCBase::ANPCBase()
{
	GetCapsuleComponent()->SetCollisionProfileName("NPC");
	GetMesh()->SetCollisionProfileName("NoCollision");

	AIControllerClass = ANPCAIController::StaticClass();
	bEnableAutoLODGeneration = false;

	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComponent->SetupAttachment(GetMesh(), "HealthBar");

	HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidgetComponent->SetDrawSize(FVector2D(150, 5));

	static ConstructorHelpers::FClassFinder<UUserWidget> widget(TEXT(
	"/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/02_Widget/UMG_HealthBar.UMG_HealthBar_C'"));
	if (widget.Succeeded())
	{
		HealthBarWidgetComponent->SetWidgetClass(widget.Class);
	}
	HealthBarWidgetComponent->SetCollisionProfileName("NoCollision");

	SET_LDMaxDrawDistance(HealthBarWidgetComponent,3500.f);

	NavigationInvokerComponent = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavInvoker"));
	NavigationInvokerComponent->SetGenerationRadii(5000.f, 7000.f);
	

	UROComponent = CreateDefaultSubobject<UUROComponent>(TEXT("UROComponent"));
	
#if WITH_EDITOR
	//RuntimeGrid = "NPCGrid";
#endif
	//MerchantComponent = CreateDefaultSubobject<UMerchantComponent>("MerchantComponent");
}

void ANPCBase::SetMetNPC(bool bSave)
{
	bHasMetNPC = true;
	if(bSave)
	{
		if(UWidgetHelperLibrary::IsUseSaveGameMode(this))
		{
			SaveMet();
		}
	}
}

void ANPCBase::SetJoinNPCAtBasement(bool bSave)
{
	bHasNPCJoined = true;

	if(bSave)
	{
		if(UWidgetHelperLibrary::IsUseSaveGameMode(this))
		{
			SaveJoin();
		}
	}
}

void ANPCBase::SaveMet()
{
	if(auto instance = Cast<USoulLikeInstance>(GetGameInstance()))
	{
		instance->SaveNPCMet(this);
	}
}

void ANPCBase::SaveJoin()
{
	if(auto instance = Cast<USoulLikeInstance>(GetGameInstance()))
	{
		instance->SaveNPCJoin(this);
	}
}

void ANPCBase::SaveDestroyed()
{
	if(auto instance = Cast<USoulLikeInstance>(GetGameInstance()))
	{
		instance->SaveNPCDestoryed(this);
	}
}

void ANPCBase::BeginPlay()
{
	Super::BeginPlay();

	LoadNPCState();
}

void ANPCBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	
	for (auto iter : NPCActions)
	{
		switch (iter)
		{
		case ENPCActionType::Merchant:
			if (UKismetSystemLibrary::DoesImplementInterface(this, UMerchantInterface::StaticClass()))
			{
				if (MerchantComponent == nullptr)
				{
					// 액터에 커스텀 액터 컴포넌트 생성
					MerchantComponent = NewObject<UMerchantComponent>(this, TEXT("MerchantComponent"));
					MerchantComponent->RegisterComponent();
				}

				OnLoadedItemList(MerchantItemPath.LoadSynchronous());
				OnLoadedAbilityList(MerchantAbilityPath.LoadSynchronous());
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
		case ENPCActionType::MatrixSlotOpen:
			if (MatrixSlotOpenComponent == nullptr)
			{
				// 액터에 커스텀 액터 컴포넌트 생성
				MatrixSlotOpenComponent = UComponentHelperLibrary::CreateActorComponent<UMatrixSlotOpenComponent>(
					this, TEXT("MatrixSlotOpenComponent"));
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
		case ENPCActionType::Chest:
			//상자 액터 내부에서 생성됨.
			break;
		case ENPCActionType::RegisterAbility:
			if(RegisterAbilityComponent == nullptr)
			{
				RegisterAbilityComponent = UComponentHelperLibrary::CreateActorComponent<URegisterAbilityComponent>(
						this, TEXT("RegisterAbilityComponent"));
			}
			break;
		default: ;
		}
	}


	if(bCanHostile)
	{
		if (HealthBarWidgetComponent)
		{
			HealthBarWidgetComponent->InitWidget();
			if (HealthBarWidgetComponent->GetUserWidgetObject())
			{
				HealthBarWidgetComponent->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Collapsed);
			}

			HealthBarWidgetComponent->AttachToComponent(
				GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), "HealthBar");
		
		}
		AttributeComponent->OnChangeHPValue.AddUniqueDynamic(this, &ABaseCharacter::UpdateHealthBar);
		AttributeComponent->OnDamagedHP.AddUniqueDynamic(this,&ABaseCharacter::UpdateDamagedHealthBar);
		
		if(bSetAroundNPCToHostileWhenBeHostile)
		{
			OnChangedHostile.AddUniqueDynamic(this,&ANPCBase::ChangeHostileAround);
		}
		
	}else
	{
		HealthBarWidgetComponent->DestroyComponent();
	}
	
}

void ANPCBase::Interaction_Implementation(ABaseCharacter* Start)
{
	IInteractionInterface::Interaction_Implementation(Start);

	InteractionActor = Start;

	if(InteractionActor.IsValid())
	{
		auto player = Cast<APlayerCharacter>(InteractionActor);
		auto pc = player->GetController<AUserController>();

		player->SetBlockedShowInteraction(true);
		player->HideInteractionWidget();

		pc->AddDialogWidget(this,DialogueAsset);
	}

	
	/*for (auto iter : NPCActions)
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
		case ENPCActionType::Chest:
			//상자액터 내부에서 구현되어 있습니다.
			//ChestComponent->OpenChest(Cast<APlayerCharacter>(InteractionActor.Get()));
			break;
		default: ;
		}
	}*/
}

void ANPCBase::FinishInteraction_Implementation()
{
	if (InteractionActor.IsValid())
	{
		auto player = Cast<APlayerCharacter>(InteractionActor);
		auto pc = player->GetController<AUserController>();
		
		player->SetBlockedShowInteraction(false);
		if(auto widget = pc->GetWidgetManagerComponent()->GetWidget(FGameplayTag::RequestGameplayTag("Widget.Dialogue")))
		{
			pc->RemoveFromPopUp(widget,true);
		}
		
	}

	/*
	if (InteractionActor.IsValid())
	{
		if (auto pc = InteractionActor->GetController<APlayerController>())
		{
			UWidgetHelperLibrary::CloseWidgetSetting(pc, nullptr);
		}
	}
	*/

	InteractionActor = nullptr;
}

void ANPCBase::LoadNPCState()
{
	if(UWidgetHelperLibrary::IsUseSaveGameMode(this))
	{
		if(auto instance = Cast<USoulLikeInstance>(GetGameInstance()))
		{
			instance->LoadNPCState(this);
		}
	}
}


void ANPCBase::MerchantEvent_Implementation()
{
	if(InteractionActor.IsValid() ==false)
	{
		UE_LOGFMT(LogTemp,Error,"{0} {1} : 상호작용 대상이 유효하지 않음",__FUNCTION__,__LINE__);
		return;
	}

	MerchantComponent->ShowMerchantWidget(InteractionActor.Get());
	Execute_FinishInteraction(this);
}

void ANPCBase::LevelUpEvent_Implementation()
{
	
	if (LevelUpComponent == nullptr)
	{
		// 액터에 커스텀 액터 컴포넌트 생성
		LevelUpComponent = UComponentHelperLibrary::CreateActorComponent<ULevelUpComponent>(
			this,TEXT("LevelUpComponent"));
	}

	LevelUpComponent->ShowLevelUpWidget();
}


void ANPCBase::EnhancementEvent_Implementation(EEnhancementType EnhancementType)
{
	
	if (EnhancementComponent == nullptr)
	{
		// 액터에 커스텀 액터 컴포넌트 생성
		EnhancementComponent = UComponentHelperLibrary::CreateActorComponent<UEnhancementComponent>(
			this, TEXT("EnhancementComponent"));
	}

	EnhancementComponent->AddEnhancementWidget(InteractionActor.Get(),EnhancementType);
	Execute_FinishInteraction(this);
}

void ANPCBase::PotionEnhancementEvent_Implementation()
{

	if (PotionEnhancementComponent == nullptr)
	{
		// 액터에 커스텀 액터 컴포넌트 생성
		PotionEnhancementComponent = UComponentHelperLibrary::CreateActorComponent<UPotionEnhancementComponent>(
			this, TEXT("PotionEnhancementComponent"));
	}
	PotionEnhancementComponent->AddPotionEnhancementWidget(InteractionActor.Get());
	Execute_FinishInteraction(this);
}

void ANPCBase::MatrixSlotOpenEvent_Implementation()
{

	if (MatrixSlotOpenComponent == nullptr)
	{
		// 액터에 커스텀 액터 컴포넌트 생성
		MatrixSlotOpenComponent = UComponentHelperLibrary::CreateActorComponent<UMatrixSlotOpenComponent>(
			this, TEXT("OrbMatrixSlotOpenComponent"));
	}

	MatrixSlotOpenComponent->AddOrbMatrixSlotOpenWidget(InteractionActor.Get());
	Execute_FinishInteraction(this);
}

void ANPCBase::OnTeleportWidgetOpenEvent_Implementation()
{

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
	if (BonfireComponent == nullptr)
	{
		// 액터에 커스텀 액터 컴포넌트 생성
		BonfireComponent = UComponentHelperLibrary::CreateActorComponent<UBonfireComponent>(
			this, TEXT("BonfireComponent"));
	}

	BonfireComponent->Rest(Cast<APlayerCharacter>(InteractionActor.Get()));
}

void ANPCBase::RegisterAbilityEvent_Implementation()
{
	if(RegisterAbilityComponent == nullptr)
	{
		RegisterAbilityComponent = UComponentHelperLibrary::CreateActorComponent<URegisterAbilityComponent>(
				this, TEXT("RegisterAbilityComponent"));
	}

	RegisterAbilityComponent->AddRegisterAbilityWidget(Cast<APlayerCharacter>(InteractionActor.Get()));
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

void ANPCBase::OnRemoveAttributeEffectAdditionalInformationEvent_Implementation(const FAttributeEffect& Effect,
	UAbilityEffectAdditionalInformation* AdditionalInformation, float DeltaTime)
{
	if(bCanHostile)
	{
		if(Execute_IsHostile(this) == false)
		{
			if (Effect.Attribute == EAttributeType::HP)
			{
				if (auto aiCon = GetController<ANPCAIController>())
				{
					UE_LOGFMT(LogTemp,Log,"피해 리포트");
					UAISense_Damage::ReportDamageEvent(this, this, AdditionalInformation->HitBy.Get(), 1,
													   GetActorLocation(), AdditionalInformation->Hit.Location);
				}
			}
		}else{
			UE_LOGFMT(LogTemp,Log,"적대적입니다. 피해를 줍니다.!!!!");
			Super::OnRemoveAttributeEffectAdditionalInformationEvent_Implementation(Effect,AdditionalInformation,DeltaTime);
		}
	}
}

void ANPCBase::OnDeadEvent(AActor* Who, AActor* DeadBy, EDeadReason DeadReason)
{
	UE_LOGFMT(LogTemp,Log,"NPC 사망");
	Super::OnDeadEvent(Who, DeadBy, DeadReason);
	bCanInteraction = false;
	if(auto aiCon = GetController<AAIController>())
	{
		SetCharacterState(ECharacterState::DEAD);
		if(auto btComp = Cast<UBehaviorTreeComponent>(aiCon->GetBrainComponent()))
		{
			aiCon->GetBlackboardComponent()->ClearValue("Target");
			btComp->StopTree(EBTStopMode::Forced);
			btComp->Cleanup();
			aiCon->GetPerceptionComponent()->ForgetAll();
		}
	}
}


void ANPCBase::BuyItemFromPlayer(ANPCBase* Merchant, APlayerCharacter* PlayerCharacter, FGuid ItemUniqueID,
                                 int32 BuyCount)
{
	if (auto invenComp = PlayerCharacter->GetInventoryComponent())
	{
		const auto& item = invenComp->GetInventoryItem(ItemUniqueID);
		//아이템이 갯수만큼 있는지 확인
		if (item.ItemCount >= BuyCount)
		{
			//아이템 카운트 제거
			if (invenComp->DecreaseItemCount(ItemUniqueID, BuyCount))
			{
				UE_LOGFMT(LogActor, Log, "플레이어에게 {0}을 {1}개 구매했습니다.", item.GetItemInformation()->Item_Tag.ToString(),
				          BuyCount);
				//비용 지불
				auto attComp = PlayerCharacter->GetAttributeComponent();
				attComp->SetEXP(attComp->GetEXP() + item.GetItemInformation()->Item_Price * BuyCount);

				UpdateExpWidget(PlayerCharacter, item.GetItemInformation()->Item_Price * BuyCount);

				attComp->OnCharacterInformationUpdate.Broadcast();
				Merchant->GetMerchantComponent()->OnBuyItemFromPlayer.Broadcast(PlayerCharacter, ItemUniqueID);

				//재구매 리스트에 추가함.
				Merchant->GetMerchantComponent()->AddRepurchaseItem(item, BuyCount);
			}
		}
	}
}

void ANPCBase::SellItemToPlayer(ANPCBase* Merchant, APlayerCharacter* PlayerCharacter, FGuid ItemUniqueID,
                                          int32 TradeCount)
{
	
	UE_LOGFMT(LogTemp, Log, "NPC가 플레이어에게 아이템을 파려고 합니다 : {0}, {1} 개", ItemUniqueID.ToString(), TradeCount);


	class UMerchantComponent* MComp = Merchant->GetMerchantComponent();
	if (MComp->IsMerchandiseItem(ItemUniqueID) || MComp->
		IsRepurchaseMerchandiseItem(ItemUniqueID))
	{
		const bool bIsMerchandise = MComp->IsMerchandiseItem(ItemUniqueID);

		// 판매 또는 재구매하는 아이템 정보를 가져옵니다.
		const auto& merchandise = bIsMerchandise
			                          ? MComp->GetMerchandiseItem(ItemUniqueID)
			                          : MComp->GetRepurchaseMerchandiseItem(ItemUniqueID);

		// 구입하려는 갯수가 충분한지 확인합니다.
		if (merchandise.MerchandiseData.bSellInfinite == false)
		{
			if (merchandise.MerchandiseData.Count < TradeCount)
			{
				UWidgetHelperLibrary::ShowAlertMsg(PlayerCharacter->GetController<AUserController>(),
				                                   EAlertMsgType::Normal, GlobalMerchantText::buyText,
				                                   FOnButtonClicked());
				return;
			}
		}

		// 충분한 재화가 있는지 확인합니다.
		const int32 TotalPrice = merchandise.MerchandiseData.Price * TradeCount;
		if (PlayerCharacter->GetAttributeComponent()->GetEXP() < TotalPrice)
		{
			UWidgetHelperLibrary::ShowAlertMsg(PlayerCharacter->GetController<AUserController>(), EAlertMsgType::Error,
			                                   GlobalMerchantText::alertText, FOnButtonClicked());
			return;
		}

		// 갯수를 차감하고 비용을 받습니다.
		if (bIsMerchandise)
		{
			if (MComp->DecreaseMerchandiseItemCount(ItemUniqueID, TradeCount) == false)
			{
				UE_LOGFMT(LogTemp, Error, "상품의 카운트를 줄이는데 실패했습니다.");
				return;
			}
		}
		else
		{
			if (MComp->DecreaseRepurchaseMerchandiseItemCount(ItemUniqueID, TradeCount) == false)
			{
				UE_LOGFMT(LogTemp, Error, "재구매 상품의 카운트를 줄이는데 실패했습니다.");
				return;
			}
		}

		PlayerCharacter->GetAttributeComponent()->SetEXP(
			PlayerCharacter->GetAttributeComponent()->GetEXP() - TotalPrice);
		UpdateExpWidget(PlayerCharacter, TotalPrice * -1);

		// 아이템을 넘깁니다.
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = PlayerCharacter;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AItemActor* sellItem = PlayerCharacter->GetWorld()->SpawnActor<AItemActor>(
			merchandise.MerchandiseData.ItemActorObject.LoadSynchronous(), spawnParams);
		sellItem->SetItemCount(TradeCount);

		//이 함수 내부에서 자동 저장.
		PlayerCharacter->GetInventoryComponent()->AddItem(sellItem, false);

		PlayerCharacter->GetAttributeComponent()->OnCharacterInformationUpdate.Broadcast();
		MComp->OnSellItemToPlayer.Broadcast(PlayerCharacter, ItemUniqueID);
	}
}

void ANPCBase::BuyAbilityFromPlayer(ANPCBase* Merchant, APlayerCharacter* PlayerCharacter,
                                              FMerchandiseAbility MerchandiseAbility, int32 BuyCount)
{
	class UMerchantComponent* MComp =Merchant->GetMerchantComponent();

	if (auto abComp = PlayerCharacter->GetAbilityComponent())
	{
		if (abComp->HasAbility(MerchandiseAbility.MerchandiseAbilityData.Tag))
		{
			abComp->RemoveAbility(MerchandiseAbility.MerchandiseAbilityData.Tag);
			UE_LOGFMT(LogActor, Log, "플레이어에게 {0}을 구매했습니다.", MerchandiseAbility.MerchandiseAbilityData.Tag.ToString());
			//비용 지불
			auto attComp = PlayerCharacter->GetAttributeComponent();
			attComp->SetEXP(attComp->GetEXP() + MerchandiseAbility.MerchandiseAbilityData.Price * BuyCount);
			UpdateExpWidget(PlayerCharacter, MerchandiseAbility.MerchandiseAbilityData.Price * BuyCount);

			attComp->OnCharacterInformationUpdate.Broadcast();
			MComp->OnBuyAbilityFromPlayer.Broadcast(PlayerCharacter);

			//재구매 리스트에 추가함.
			MComp->AddRepurchaseAbility(MerchandiseAbility);
		}
	}
}

void ANPCBase::SellAbilityToPlayer(ANPCBase* Merchant, APlayerCharacter* PlayerCharacter,
                                             FMerchandiseAbility MerchandiseAbility, int32 TradeCount)
{
	class UMerchantComponent* MComp =Merchant->GetMerchantComponent();

	auto abilityTag = MerchandiseAbility.MerchandiseAbilityData.Tag;
	UE_LOGFMT(LogTemp, Log, "NPC가 플레이어에게 어빌리티를 파려고 합니다 : {0}, {1} 개", abilityTag.ToString(), TradeCount);
	if (MComp->IsMerchandiseAbility(abilityTag) || MComp->
		IsRepurchaseMerchandiseAbility(abilityTag))
	{
		const bool bIsMerchandise = MComp->IsMerchandiseAbility(abilityTag);

		// 판매 또는 재구매하는 아이템 정보를 가져옵니다.
		const auto& merchandise = bIsMerchandise
			                          ? MComp->GetMerchandiseAbility(abilityTag)
			                          : MComp->GetRepurchaseMerchandiseAbility(abilityTag);

		// 구입하려는 갯수가 충분한지 확인합니다.
		if (merchandise.MerchandiseAbilityData.bSellInfinite == false)
		{
			if (merchandise.MerchandiseAbilityData.Count < TradeCount)
			{
				UWidgetHelperLibrary::ShowAlertMsg(PlayerCharacter->GetController<AUserController>(),
				                                   EAlertMsgType::Normal, GlobalMerchantText::buyText,
				                                   FOnButtonClicked());
				return;
			}
		}

		// 충분한 재화가 있는지 확인합니다.
		const int32 TotalPrice = merchandise.MerchandiseAbilityData.Price * TradeCount;
		if (PlayerCharacter->GetAttributeComponent()->GetEXP() < TotalPrice)
		{
			UWidgetHelperLibrary::ShowAlertMsg(PlayerCharacter->GetController<AUserController>(), EAlertMsgType::Error,
			                                   GlobalMerchantText::alertText, FOnButtonClicked());
			return;
		}

		// 갯수를 차감하고 비용을 받습니다.
		if (bIsMerchandise)
		{
			if (MComp->DecreaseMerchandiseAbilityCount(abilityTag, TradeCount) == false)
			{
				UE_LOGFMT(LogTemp, Error, "상품의 카운트를 줄이는데 실패했습니다.");
				return;
			}
		}
		else
		{
			if (MComp->DecreaseRepurchaseMerchandiseAbilityCount(abilityTag, TradeCount) == false)
			{
				UE_LOGFMT(LogTemp, Error, "재구매 상품의 카운트를 줄이는데 실패했습니다.");
				return;
			}
		}

		PlayerCharacter->GetAttributeComponent()->SetEXP(
			PlayerCharacter->GetAttributeComponent()->GetEXP() - TotalPrice);

		UpdateExpWidget(PlayerCharacter, TotalPrice * -1);

		//어빌리티를 넘깁니다.
		PlayerCharacter->GetAbilityComponent()->GiveAbility(
			merchandise.MerchandiseAbilityData.AbilityObject.LoadSynchronous());
		PlayerCharacter->GetAttributeComponent()->OnCharacterInformationUpdate.Broadcast();
		MComp->OnSellAbilityToPlayer.Broadcast(PlayerCharacter);
	}
}

void ANPCBase::UpdateExpWidget(APlayerCharacter* Player, const int32& AddExp)
{
	if (auto attComp = Player->GetAttributeComponent())
	{
		attComp->OnUpdateExp.Broadcast(AddExp);
	}
}

UMerchantComponent* ANPCBase::GetMerchantComponent()
{
	return MerchantComponent;
}


void ANPCBase::SetActorHiddenInGame(bool bNewHidden)
{
	Super::SetActorHiddenInGame(bNewHidden);
	
	SetActorEnableCollision(!bNewHidden);
}

bool ANPCBase::IsLockOnAble_Implementation()
{
	return bLockOnAble;
}

void ANPCBase::SetLockOnAble_Implementation(bool newVal)
{
	bLockOnAble = newVal;
}

void ANPCBase::SetHostile_Implementation(ABaseCharacter* HostileBy)
{
	UE_LOGFMT(LogTemp,Log,"적대적으로 변경 : {0}", GetName());
	if(auto aiCon= GetController<ANPCAIController>())
	{
		bCanInteraction = false;
		aiCon->SetGenericTeamId(FGenericTeamId(static_cast<uint8>(ETeam::Monster)));
		if(UKismetSystemLibrary::DoesImplementInterface(this,ULockOnInterface::StaticClass()))
		{
			Execute_SetLockOnAble(this,true);
		}
		
		GetCapsuleComponent()->SetCollisionProfileName("Pawn");
		GetMesh()->SetCollisionProfileName("Monster");
		
		aiCon->RunBehaviorTree(Execute_GetHostileBehaviorTree(this));
		if(auto bbComp = aiCon->GetBlackboardComponent()){
			bbComp->SetValueAsObject("Target",HostileBy);
		}
	}

	OnChangedHostile.Broadcast(HostileBy);
}

bool ANPCBase::IsHostile_Implementation() const
{
	return GetController<AAIController>()->GetGenericTeamId() != static_cast<uint8>(ETeam::Player);
}

bool ANPCBase::CanInteraction_Implementation()
{
	return bCanInteraction;
}

void ANPCBase::ChangeHostileAround(ABaseCharacter* HostileBy)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> objTypes;
	objTypes.Emplace(UEngineTypes::ConvertToObjectType(ECC_NPC));
	
	TArray<FHitResult> hits;
	if(UKismetSystemLibrary::SphereTraceMultiForObjects(this,GetActorLocation(),GetActorLocation(),SetAroundNPCToHostileRadius,objTypes,false,TArray<AActor*>(),EDrawDebugTrace::ForOneFrame,hits,true))
	{
		for(const auto& hit : hits)
		{
			if(UKismetSystemLibrary::DoesImplementInterface(hit.GetActor(),UHostileInterface::StaticClass()))
			{
				if(!Execute_IsHostile(hit.GetActor()))
				{
					Execute_SetHostile(hit.GetActor(),HostileBy);
				}
			}
		}
	}
}
