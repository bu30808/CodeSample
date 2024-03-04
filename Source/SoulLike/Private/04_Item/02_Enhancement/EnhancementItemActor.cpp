// Fill out your copyright notice in the Description page of Project Settings.


#include "04_Item/02_Enhancement/EnhancementItemActor.h"

#include "99_Subsystem/ItemManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"

class UItemManagerSubsystem;

const FItemInformation* AEnhancementItemActor::GetItemInformation()
{
	if (auto instance = UGameplayStatics::GetGameInstance(this))
	{
		if (auto system = instance->GetSubsystem<UItemManagerSubsystem>())
		{
			//여기서 무엇인가 하세요.
			return system->GetEnhancementItemInformation(ItemTag);
		}
	}
#ifndef WITH_EDITOR
	UE_LOGFMT(LogTemp,Error,"서브시스템을 가져올 수 없습니다 : {0} {1}",__FUNCTION__,__LINE__);
#endif

	return nullptr;
}

bool AEnhancementItemActor::UseItem_Implementation(AActor* Target, const FGuid& ThisItemsUniqueID)
{
	//아무것도 안 함.
	return true;
}
