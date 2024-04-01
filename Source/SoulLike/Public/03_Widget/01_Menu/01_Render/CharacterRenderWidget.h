// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "CharacterRenderWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UCharacterRenderWidget : public UUserWidget
{
	GENERATED_BODY()
	friend class UCharacterTotalInfoWidget;

protected:
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Reset;
	UPROPERTY(meta=(BindWidget))
	UButton* Button_Right;
	UPROPERTY(meta=(BindWidget))
	UButton* Button_Left;
	UPROPERTY(meta=(BindWidget))
	UButton* Button_Up;
	UPROPERTY(meta=(BindWidget))
	UButton* Button_Down;
	UPROPERTY(meta=(BindWidget))
	class UImage* Image_PlayerRenderTarget;

	UPROPERTY(Transient)
	bool bPressed = false;

	UPROPERTY()
	FVector2D StartLoc;

	UPROPERTY()
	TWeakObjectPtr<class APlayerCharacter> Player;

	UPROPERTY(EditAnywhere)
	float PitchMax = 45.f;


	bool bPressedDown;
	bool bPressedUp;
	bool bPressedLeft;
	bool bPressedRight;

	UFUNCTION()
	void OnPressedDownButton();
	UFUNCTION()
	void OnPressedUpButton();
	UFUNCTION()
	void OnPressedLeftButton();
	UFUNCTION()
	void OnPressedRightButton();
	UFUNCTION()
	void OnReleasedDownButton();
	UFUNCTION()
	void OnReleasedUpButton();
	UFUNCTION()
	void OnReleasedLeftButton();
	UFUNCTION()
	void OnReleasedRightButton();

	UFUNCTION()
	void OnClickedResetButton();

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void Init();
};
