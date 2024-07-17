// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "02_Ability/AbilityBase.h"
#include "04_Item/ItemActor.h"
#include "AbilityItemActor.generated.h"

USTRUCT(BlueprintType)
struct FAbilityItemInformation : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UAbilityBase> AbilityClass;
};

UCLASS()
class SOULLIKE_API AAbilityItemActor : public AItemActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAbilityItemActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	const FAbilityItemInformation* GetAbilityInformation() const;
	
};
