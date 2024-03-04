// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "OrbMatrixButtonWidget.generated.h"

enum class EOrbMatrixSlotContent : uint8;
enum class EOrbMatrixSlotLineDirection : uint8;
enum class EOrbMatrixSlotType : uint8;
/**
 * 
 */
UCLASS()
class SOULLIKEEDITOR_API UOrbMatrixButtonWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UButton* Button;
	UPROPERTY(meta=(BindWidget))
	class UImage* Image_Lock;

	UPROPERTY(EditAnywhere)
	TMap<EOrbMatrixSlotType, FLinearColor> ColorForSlotType;


	UPROPERTY(EditAnywhere)
	class UTexture2D* PointSlotTexture;
	UPROPERTY(EditAnywhere)
	class UTexture2D* LineSlotTexture;


	UPROPERTY()
	EOrbMatrixSlotType OrbMatrixSlotType;
	UPROPERTY()
	EOrbMatrixSlotLineDirection LineDirection;
	UPROPERTY()
	EOrbMatrixSlotContent SlotContent;
	UPROPERTY()
	bool bLock = false;


	UPROPERTY()
	int32 Row;
	UPROPERTY()
	int32 Col;
	UPROPERTY()
	TWeakObjectPtr<class UOrbCreatorWidget> OrbCreatorWidget;

	void ChangeButtonStyle(UTexture2D* NewTexture);

	UFUNCTION()
	void OnClickedButton();

	virtual void NativePreConstruct() override;
	void SetColRow(int32 R, int32 C);
	void SetCore();
	void SetLine(EOrbMatrixSlotLineDirection OrbMatrixSlotLineDirection);
	void SetPoint(EOrbMatrixSlotType NewOrbMatrixSlotType);

	void ForceUpdate(EOrbMatrixSlotLineDirection SlotLineDirection, EOrbMatrixSlotType OrbMatrixSlot, bool bLocked);
	void SetLocked(bool bIsLocked);
};
