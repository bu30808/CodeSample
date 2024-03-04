// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GradientBaseWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UGradientBaseWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicMat;
	UPROPERTY()
	class UImage* ParentImage;
	UPROPERTY()
	float PrePercent = 0;
	UPROPERTY()
	float Percent = 0;
	UPROPERTY()
	float CurAlpha;
	UPROPERTY(EditAnywhere)
	class UMaterialInterface* GradientBarMaterial;
	UPROPERTY(EditAnywhere)
	FLinearColor Color;
	UPROPERTY(EditAnywhere)
	float ResponseTime = 0.5f;
	UPROPERTY(EditAnywhere)
	FName ProgressParamName = "percentage";

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void SetDynamicMaterial();

public:
	//주어진 값을 가지고 프로그래스를 업데이트 합니다.
	UFUNCTION(BlueprintCallable)
	virtual void UpdateProgress(float cur, float max);
	//	void InitProgress(float HP, float MaxHP);
};
