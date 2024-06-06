// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/04_NPC/Chest.h"
#include "Components/ActorComponent.h"
#include "ChestComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULLIKE_API UChestComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UChestComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	void OpenChest(APlayerCharacter* PlayerCharacter, TSubclassOf<AItemActor> ChestItem,const int32& ItemCount, const FVector& SpawnLocation) const;
};
