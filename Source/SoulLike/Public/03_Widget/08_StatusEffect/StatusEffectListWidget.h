// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "Blueprint/UserWidget.h"
#include "StatusEffectListWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UStatusEffectListWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* VerticalBox_EffectList;

	UPROPERTY()
	TMap<EStatusEffect,class UStatusEffectProgressWidget*> AddedWidgetList;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UStatusEffectProgressWidget> StatusEffectProgressWidgetObject;
	
public:
	virtual void NativeConstruct() override;
	UFUNCTION(BlueprintCallable)
	void UpdateStatusEffect(EStatusEffect StatusEffect, float Percent);
	UFUNCTION()
	void OnUpdateStatusEffect(EStatusEffect StatusEffect, float Value, float ResistValue);
};

