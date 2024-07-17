// Fill out your copyright notice in the Description page of Project Settings.


#include "04_Item/05_Ability/AbilityItemActor.h"

#include "99_Subsystem/ItemManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"


class UItemManagerSubsystem;
// Sets default values
AAbilityItemActor::AAbilityItemActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AAbilityItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}

const FAbilityItemInformation* AAbilityItemActor::GetAbilityInformation() const
{
	if (auto instance = UGameplayStatics::GetGameInstance(this))
	{
		if (auto system = instance->GetSubsystem<UItemManagerSubsystem>())
		{
			//여기서 무엇인가 하세요.
			return system->GetAbilityInformation(ItemTag);
		}
	}
#ifndef WITH_EDITOR
	UE_LOGFMT(LogTemp,Error,"서브시스템을 가져올 수 없습니다 : {0} {1}",__FUNCTION__,__LINE__);
#endif

	return nullptr;
}


