// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/01_Component/ItemDropComponent.h"

#include "04_Item/ItemActor.h"
#include "00_Character/BaseCharacter.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/00_Player/00_Controller/UserController.h"
#include "00_Character/00_Player/01_Component/AbilityTalentComponent.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "00_Character/03_Monster/ExpOrb.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "97_Interface/BossMonsterInterface.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

// Sets default values for this component's properties
UItemDropComponent::UItemDropComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...

	{
		static ConstructorHelpers::FObjectFinder<UDataTable> table(TEXT(
			"/Script/Engine.DataTable'/Game/Blueprints/03_Monster/EXP/DT_Exp.DT_Exp'"));
		if (table.Succeeded())
		{
			ExpTable = table.Object;
		}
	}

	{
		static ConstructorHelpers::FClassFinder<AExpOrb> table(TEXT(
			"/Script/Engine.Blueprint'/Game/Blueprints/03_Monster/EXP/BP_ExpOrb.BP_ExpOrb_C'"));
		if (table.Succeeded())
		{
			ExpOrbObject = table.Class;
		}
	}
}

// Called when the game starts
void UItemDropComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	CreateDropItemListFromTable();
	GetDropExpFromTable();
}

void UItemDropComponent::CreateDropItemListFromTable()
{
	if (DropTable)
	{
		TArray<FDropItem*> out;
		DropTable->GetAllRows<FDropItem>("", out);

		for (auto o : out)
		{
			DropItems.Emplace(*o);
		}
	}
}

void UItemDropComponent::GetDropExpFromTable()
{
	if (ExpTable)
	{
		if (auto data = ExpTable->FindRow<FExp>(GetOwner<ABaseMonster>()->GetMonsterTag().GetTagName(), ""))
		{
			Exp = data->Exp;
		}
		else
		{
			UE_LOGFMT(LogTemp, Error, "경험치 테이블에서 정보를 가져올 수 없습니다.");
		}
	}
}

void UItemDropComponent::PostInitProperties()
{
	Super::PostInitProperties();
}

void UItemDropComponent::BossDropItem(ABaseCharacter* DropBy)
{
	APlayerCharacter* player = nullptr;

	if (DropBy->IsA<APlayerCharacter>())
	{
		player = Cast<APlayerCharacter>(DropBy);
	}
	else
	{
		player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}

	if (player)
	{
		//보스는 드롭아이템 전부를 플레이어에게 다이렉트로 추가합니다.
		for (auto i : DropItems)
		{
			if (auto load = i.DropItem.LoadSynchronous())
			{
				FActorSpawnParameters spawnParam;
				spawnParam.Owner = DropBy;
				spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				if (const auto item = GetWorld()->SpawnActor<AItemActor>(load, spawnParam))
				{
					//TODO 여기서 아이템 획득 위젯을 대문짝만하게 보여줍니다.
					UWidgetHelperLibrary::ShowBossItemGetWidget(Cast<AUserController>(player->GetController()), item);
					player->GetInventoryComponent()->AddItem(item, false);
				}
			}
		}
	}
}

AItemActor* UItemDropComponent::DropItem(ABaseCharacter* DropBy)
{
	/*if (auto interface = Cast<IBossMonsterInterface>(GetOwner()))
	{
		BossDropItem(DropBy);
		return;
	}*/

	TArray<FDropItem> DropItemList;
	for (auto i : DropItems)
	{
		const float randomValue = FMath::FRand();
		UE_LOGFMT(LogTemp, Warning, "주사위값 : {0} / 드롭확률 : {1}", randomValue, i.DropRate);
		if (randomValue <= i.DropRate)
		{
			DropItemList.Emplace(i);
		}
	}

	if (DropItemList.Num() > 0)
	{
		const auto& pick = DropItemList[FMath::RandRange(0, DropItemList.Num() - 1)];
		if (auto load = pick.DropItem.LoadSynchronous())
		{
			FActorSpawnParameters spawnParam;
			spawnParam.Owner = DropBy;
			spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			if (const auto item = GetWorld()->SpawnActor<AItemActor>(load, spawnParam))
			{
				item->GetSphereComponent()->SetSphereRadius(item->GetSphereComponent()->GetScaledSphereRadius() * 1.5f);
				//item->AttachToActor(GetOwner(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),ItemAttachSocketName);
				item->AttachToComponent(GetOwner<ABaseCharacter>()->GetRootComponent(),
				                        FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
				                        ItemAttachSocketName);

				GetOwner<ABaseCharacter>()->OnDestroyed.
				                            AddUniqueDynamic(item, &AItemActor::OnAttachedOwnerDestroyEvent);

				return item;
			}
		}
	}

	return nullptr;
}

void UItemDropComponent::GiveExp(ABaseCharacter* DropBy)
{
	if (Exp <= 0)
	{
		return;
	}

	//험치 구슬을 생성하고 경험치를 설정해줍니다. 이 구슬은 플레이어와 겹칠때까지 따라가며, 겹치면 가지고 있던 경험치를 플레이어에게 줍니다.
	if (DropBy != nullptr && ExpOrbObject != nullptr)
	{
		//몬스터들끼리 팀킬하는 경우에, 플레이어에게 경험치가 갈 수 있도록 합니다.
		if (DropBy->IsA<APlayerCharacter>() == false)
		{
			DropBy = Cast<ABaseCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
		}

		//우두머리의 경우 다이렉트로 플레이어에게 경험치를 추가해줍니다.
		if (UKismetSystemLibrary::DoesImplementInterface(GetOwner(), UBossMonsterInterface::StaticClass()))
		{
			if (auto attComp = Cast<APlayerCharacter>(DropBy)->GetAttributeComponent())
			{
				UE_LOGFMT(LogCharacter, Warning, "보스 처치 보상 즉시 지급");
				attComp->SetEXP(attComp->GetEXP() + Exp);
				attComp->OnCharacterInformationUpdate.Broadcast();
			}
			return;
		}

		FActorSpawnParameters spawnParam;
		spawnParam.Owner = DropBy;
		spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		if (auto expOrb = GetWorld()->SpawnActor<AExpOrb>(ExpOrbObject, GetOwner()->GetActorLocation(),
		                                                  FRotator::ZeroRotator, spawnParam))
		{
			auto exp = Exp + Cast<APlayerCharacter>(DropBy)->GetAbilityTalentComponent()->
			                                                 CalculateModifiedIncreaseGetExpWithTraits(Exp);
			UE_LOGFMT(LogTemp, Log, "오브에 특성이 적용된 경험치 설정 : {0}", exp);

			expOrb->SetExp(exp);
			expOrb->StartProjectile(DropBy);
		}
	}
}
