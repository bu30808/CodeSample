// Fill out your copyright notice in the Description page of Project Settings.


#include "04_Item/01_Equipment/02_Ring/EquipmentItemActor_Ring.h"

#include "99_Subsystem/ItemManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"

const FItemInformation* AEquipmentItemActor_Ring::GetItemInformation()
{
	if (auto instance = UGameplayStatics::GetGameInstance(this))
	{
		if (auto system = instance->GetSubsystem<UItemManagerSubsystem>())
		{
			//여기서 무엇인가 하세요.
			return system->GetRingItemInformation(ItemTag);
		}
	}
#ifndef WITH_EDITOR
	UE_LOGFMT(LogTemp,Error,"서브시스템을 가져올 수 없습니다 : {0} {1}",__FUNCTION__,__LINE__);
#endif

	return nullptr;
}

/*
void AEquipmentItemActor_Ring::OnOverrideSelfEffectEvent_Implementation(const TArray<UAbilityEffect*>& SelfEffect,
	AActor* Target)
{
	//아무것도 안함. 필요하면 블루프린트에서 덮어 쓸 것.
}
*/
