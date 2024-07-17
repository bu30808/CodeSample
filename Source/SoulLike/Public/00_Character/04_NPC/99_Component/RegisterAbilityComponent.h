// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "Components/ActorComponent.h"
#include "RegisterAbilityComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULLIKE_API URegisterAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URegisterAbilityComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
public:
	void AddRegisterAbilityWidget(class APlayerCharacter* Player);
protected:
	UPROPERTY(Transient)
	TWeakObjectPtr<class URegisterAbilityWidget> RegisterAbilityWidget;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class URegisterAbilityWidget> RegisterAbilityWidgetClass;
};
