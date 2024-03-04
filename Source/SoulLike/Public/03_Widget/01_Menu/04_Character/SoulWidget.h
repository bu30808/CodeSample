// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TimelineComponent.h"
#include "SoulWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API USoulWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Soul;
	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* HorizontalBox_GetSoul;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_SoulAdd;
	
	UPROPERTY(Transient)
	float CurExp;
	UPROPERTY(Transient)
	float NextExp;
	UPROPERTY()
	class UTimelineComponent* EXPLerpTimeLine;
	UPROPERTY()
	FOnTimelineFloat UpdateDissolve;
	UPROPERTY(EditAnywhere)
	UCurveFloat* EXPLerpCurve;
	UPROPERTY(EditAnywhere)
	float LerpLength = 3.f;

public:
	virtual void NativeConstruct() override;
	/*virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;*/
	
	UFUNCTION()
	void OnCharacterInformationUpdateEvent();
	UFUNCTION()
	void OnUpdateEXPEvent(float AddEXP);
	
	UFUNCTION()
	void OnLerpEXPEvent(float Alpha);
};
