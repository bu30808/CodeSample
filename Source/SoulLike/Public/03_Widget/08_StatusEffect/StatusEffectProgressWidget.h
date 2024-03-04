// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "Blueprint/UserWidget.h"
#include "StatusEffectProgressWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UStatusEffectProgressWidget : public UUserWidget
{
	GENERATED_BODY()
protected:

	UPROPERTY(meta=(BindWidget))
	class UImage* Image_StatusEffect;
	UPROPERTY(meta=(BindWidget))
	class UImage* Image_Progress;
	
	
	UPROPERTY(EditAnywhere)
	TMap<EStatusEffect,TSoftObjectPtr<UTexture2D>> StatusEffectImage;
	UPROPERTY(EditAnywhere)
	class UMaterialInterface* ProgressMaterial;
	UPROPERTY(EditAnywhere)
	FLinearColor DefaultProgressColor;
	UPROPERTY(EditAnywhere)
	FLinearColor ProgressFullColor;
	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicMat;

	
public:
	UPROPERTY(VisibleAnywhere)
	EStatusEffect StatusEffectType;

	void SetDynamicMaterial();
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void SetProgress(float Value);
	void SetImage(EStatusEffect StatusEffect);
	
};
