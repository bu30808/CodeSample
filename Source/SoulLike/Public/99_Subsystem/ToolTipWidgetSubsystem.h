// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "03_Widget/SimpleToolTipWidget.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ToolTipWidgetSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UToolTipWidgetSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UToolTipWidgetSubsystem();

	UPROPERTY()
	TSubclassOf<class USimpleToolTipWidget> SimpleToolTipWidgetObject;


	UFUNCTION(Blueprintable, BlueprintPure)
	class USimpleToolTipWidget* CreateSimpleToolTipWidget(class APlayerController* PC, FString Msg) const;
};
