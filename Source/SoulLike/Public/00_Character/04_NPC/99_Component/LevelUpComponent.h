// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LevelUpComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULLIKE_API ULevelUpComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULevelUpComponent();

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ULevelUpWidget> LevelUpWidgetObject;
	UPROPERTY(Transient)
	TWeakObjectPtr<ULevelUpWidget> LevelUpWidget;

public:
	UFUNCTION()
	void OnLevelUpWidgetVisibilityChangedEvent(ESlateVisibility InVisibility);

	void CreateLevelUpWidget();
	void ShowLevelUpWidget();
};
