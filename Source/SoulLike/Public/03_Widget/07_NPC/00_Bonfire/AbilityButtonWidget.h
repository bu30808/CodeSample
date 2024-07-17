// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "02_Ability/AbilityBase.h"
#include "03_Widget/99_Drag/DraggableWidget.h"
#include "AbilityButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UAbilityButtonWidget : public UDraggableWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class UImage* Image;
	UPROPERTY(meta=(BindWidget))
	class UImage* Image_Equipped;
	//이 버튼이 가지고 있는 어빌리티 정보
	UPROPERTY()
	FAbilityInformation AbilityInformation;

public:
	void SetInfo(const FAbilityInformation& Info);
	void SetEquipped(bool bCond);

	const FAbilityInformation& GetAbilityData(){return AbilityInformation;}
	
	
};

