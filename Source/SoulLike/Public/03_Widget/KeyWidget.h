// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KeyWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UKeyWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Key;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Action;

	UPROPERTY(EditAnywhere)
	FName KeyName = "NONE";
	UPROPERTY(EditAnywhere)
	FString ActionName = "NoAction";

	virtual void NativePreConstruct() override;

public:
	UFUNCTION(BlueprintCallable)
	void SetKeyAndAction(FName Key, FString Action);
};
