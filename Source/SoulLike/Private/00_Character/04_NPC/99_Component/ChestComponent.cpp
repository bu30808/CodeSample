// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/04_NPC/99_Component/ChestComponent.h"


// Sets default values for this component's properties
UChestComponent::UChestComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UChestComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UChestComponent::OpenChest(APlayerCharacter* PlayerCharacter, TSubclassOf<AItemActor> ChestItem,
                                const int32& ItemCount, const FVector& SpawnLocation) const
{
	if (PlayerCharacter != nullptr)
	{
		if (ChestItem != nullptr)
		{
			FActorSpawnParameters spawnParam;
			spawnParam.Instigator = PlayerCharacter;
			spawnParam.Owner = PlayerCharacter;
			spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			

			if (auto item = GetWorld()->SpawnActor<AItemActor>(ChestItem, SpawnLocation, FRotator::ZeroRotator,
			                                                   spawnParam))
			{
				item->SetItemCount(ItemCount);
				item->SetActorScale3D(GetOwner()->GetActorScale3D());
			}
		}
	}
}
