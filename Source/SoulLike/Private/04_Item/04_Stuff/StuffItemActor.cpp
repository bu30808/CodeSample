// Fill out your copyright notice in the Description page of Project Settings.


#include "04_Item/04_Stuff/StuffItemActor.h"

#include "99_Subsystem/ItemManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"


class UItemManagerSubsystem;
// Sets default values
AStuffItemActor::AStuffItemActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AStuffItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStuffItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const FItemInformation* AStuffItemActor::GetItemInformation()
{
	if (auto instance = UGameplayStatics::GetGameInstance(this))
	{
		if (auto system = instance->GetSubsystem<UItemManagerSubsystem>())
		{
			//여기서 무엇인가 하세요.
			return system->GetStuffItemInformation(ItemTag);
		}
	}
#ifndef WITH_EDITOR
	UE_LOGFMT(LogTemp,Error,"서브시스템을 가져올 수 없습니다 : {0} {1}",__FUNCTION__,__LINE__);
#endif

	return nullptr;
}

