// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "02_Ability/AbilityBase.h"
#include "Blueprint/UserWidget.h"
#include "BuffIconWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UBuffIconWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class UImage* Image_Buff;
	UPROPERTY()
	FGameplayTag BuffTag;

public:
	void SetIconInfo(const FAbilityInformation& AbilityInformation);
	bool IsTagMatch(const FGameplayTag& Tag) const;
};
