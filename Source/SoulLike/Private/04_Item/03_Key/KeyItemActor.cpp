// Fill out your copyright notice in the Description page of Project Settings.


#include "04_Item/03_Key/KeyItemActor.h"

#include "99_Subsystem/ItemManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"


class UItemManagerSubsystem;
// Sets default values
AKeyItemActor::AKeyItemActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AKeyItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKeyItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const FItemInformation* AKeyItemActor::GetItemInformation()
{
	if (auto instance = UGameplayStatics::GetGameInstance(this))
	{
		if (auto system = instance->GetSubsystem<UItemManagerSubsystem>())
		{
			//여기서 무엇인가 하세요.
			return system->GetKeyItemInformation(ItemTag);
		}
	}
#ifndef WITH_EDITOR
	UE_LOGFMT(LogTemp,Error,"서브시스템을 가져올 수 없습니다 : {0} {1}",__FUNCTION__,__LINE__);
#endif

	return nullptr;
}

