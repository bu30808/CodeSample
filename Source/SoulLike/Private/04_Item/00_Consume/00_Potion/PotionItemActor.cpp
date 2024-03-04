// Fill out your copyright notice in the Description page of Project Settings.


#include "04_Item/00_Consume/00_Potion/PotionItemActor.h"

#include "99_Subsystem/ItemManagerSubsystem.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"


class UItemManagerSubsystem;

const FItemInformation* APotionItemActor::GetItemInformation()
{
	return &PotionItemInformation;
}

void APotionItemActor::GetPotionInformation()
{
	if (auto instance = UGameplayStatics::GetGameInstance(this))
	{
		if (auto system = instance->GetSubsystem<UItemManagerSubsystem>())
		{
			PotionItemInformation = *static_cast<const FPotionInformation*>(system->GetPotionItemInformation(ItemTag));
		}
	}
#ifndef WITH_EDITOR
	UE_LOGFMT(LogTemp,Error,"서브시스템을 가져올 수 없습니다 : {0} {1}",__FUNCTION__,__LINE__);
#endif
}

void APotionItemActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//회복약의 기본 정보를 가져와 기억합니다.
	GetPotionInformation();
}

FString APotionItemActor::GetEnhancedDescription()
{
	return TEXT("강화단계 : ") +FString::FormatAsNumber(GetEnhancementValue());
}

// Sets default values
APotionItemActor::APotionItemActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void APotionItemActor::BeginPlay()
{
	Super::BeginPlay();
}


int32 APotionItemActor::GetEnhancementValue()
{
	return PotionItemInformation.Enhancement.CurEnhancement;
}

void APotionItemActor::IncreaseEnhance()
{
	PotionItemInformation.Enhancement.IncreaseEnhance();
	PotionItemInformation.MaxStack = FMath::Clamp(PotionItemInformation.Enhancement.CurEnhancement + 1, 1, 10);
	UE_LOGFMT(LogTemp, Log, "현재 회복약의 최대 누적 갯수 : {0}", PotionItemInformation.MaxStack);
}
