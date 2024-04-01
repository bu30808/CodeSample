// Fill out your copyright notice in the Description page of Project Settings.


#include "04_Item/01_Equipment/03_Orb/EquipmentItemActor_OrbCore.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "03_Widget/MainWidget.h"
#include "03_Widget/01_Menu/05_Orb/OrbWidget.h"
#include "99_Subsystem/ItemManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"


AEquipmentItemActor_OrbCore::AEquipmentItemActor_OrbCore()
{
	{
		static ConstructorHelpers::FObjectFinder<UNiagaraSystem> system(TEXT(
			"/Script/Niagara.NiagaraSystem'/Game/Blueprints/04_Item/NS_Item_Orb.NS_Item_Orb'"));
		if (system.Succeeded())
		{
			NiagaraComponent->SetAsset(system.Object);
		}
	}
}

void AEquipmentItemActor_OrbCore::SetCoreMatrixPtr()
{
	if (auto info = const_cast<FOrbCoreInformation*>(static_cast<const FOrbCoreInformation*>(GetItemInformation())))
	{
		if (info->OrbMatrix == nullptr)
		{
			info->OrbMatrix = DuplicateObject(info->OrbMatrix_Original.Get(), this);
			info->OrbMatrix->SetMatrix(info->OrbMatrix_Original.Get());
		}
	}
}

void AEquipmentItemActor_OrbCore::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SetCoreMatrixPtr();
}

const FItemInformation* AEquipmentItemActor_OrbCore::GetItemInformation()
{
	if (auto instance = UGameplayStatics::GetGameInstance(this))
	{
		if (auto system = instance->GetSubsystem<UItemManagerSubsystem>())
		{
			//여기서 무엇인가 하세요.
			return system->GetOrbCoreItemInformation(ItemTag);
		}
	}
#ifndef WITH_EDITOR
	UE_LOGFMT(LogTemp,Error,"서브시스템을 가져올 수 없습니다 : {0} {1}",__FUNCTION__,__LINE__);
#endif

	return nullptr;
}

bool AEquipmentItemActor_OrbCore::UseItem_Implementation(AActor* Target, const FGuid& ThisItemsUniqueID)
{
	Super::UseItem_Implementation(Target, ThisItemsUniqueID);
	if (auto player = Cast<APlayerCharacter>(Target))
	{
		if (auto invenComp = player->GetInventoryComponent())
		{
			invenComp->SetEquippedCoreID(ThisItemsUniqueID);

			if (auto main = player->GetMainWidget())
			{
				main->UMG_Orb->OnEquipCoreEvent(invenComp->GetInventoryItem(ThisItemsUniqueID));
			}

			return true;
		}
	}
	return false;
}

void AEquipmentItemActor_OrbCore::UnEquip_Implementation(AActor* Target, const FGuid& ThisItemsUniqueID)
{
	Super::UnEquip_Implementation(Target, ThisItemsUniqueID);
	UE_LOGFMT(LogTemp, Log, "기존 장착중이던 코어를 장착해제합니다 : {0}", ItemTag.ToString());
}

void AEquipmentItemActor_OrbCore::BeginPlay()
{
	Super::BeginPlay();
}

void AEquipmentItemActor_OrbCore::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	/*if(auto info = const_cast<FOrbCoreInformation*>(static_cast<const FOrbCoreInformation*>(GetItemInformation())))
	{
		if(info->OrbMatrix->IsRooted())
		{
			//info->OrbMatrix->RemoveFromRoot();
		}
	}*/
}

void AEquipmentItemActor_OrbCore::Destroyed()
{
	if (auto info = const_cast<FOrbCoreInformation*>(static_cast<const FOrbCoreInformation*>(GetItemInformation())))
	{
		if (info->OrbMatrix != nullptr)
		{
			if (info->OrbMatrix->IsRooted())
			{
				info->OrbMatrix->RemoveFromRoot();
			}
		}
	}
	Super::Destroyed();
}
