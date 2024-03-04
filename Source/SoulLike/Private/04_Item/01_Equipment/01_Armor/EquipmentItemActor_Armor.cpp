// Fill out your copyright notice in the Description page of Project Settings.


#include "04_Item/01_Equipment/01_Armor/EquipmentItemActor_Armor.h"

#include "99_Subsystem/ItemManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"

const FItemInformation* AEquipmentItemActor_Armor::GetItemInformation()
{
	if (auto instance = UGameplayStatics::GetGameInstance(this))
	{
		if (auto system = instance->GetSubsystem<UItemManagerSubsystem>())
		{
			//여기서 무엇인가 하세요.
			return system->GetArmorItemInformation(ItemTag);
		}
	}
#ifndef WITH_EDITOR
	UE_LOGFMT(LogTemp,Error,"서브시스템을 가져올 수 없습니다 : {0} {1}",__FUNCTION__,__LINE__);
#endif

	return nullptr;
}
