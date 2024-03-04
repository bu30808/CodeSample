// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OrbElementListWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/CheckBox.h"
#include "OrbElementFilterWidget.generated.h"

enum class EOrbMatrixSlotType : uint8;
enum class EAttributeType : uint8;

/**
 * 
 */
UCLASS()
class SOULLIKE_API UOrbElementFilterWidget : public UUserWidget
{
	GENERATED_BODY()

	friend class UOrbElementListWidget;

protected:
	UPROPERTY(meta=(BindWidget))
	class UImage* Image;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Item;
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox;


	UPROPERTY(EditAnywhere)
	bool bOrbSlotFilter = false;
	//초기 체크 여부
	UPROPERTY(EditAnywhere)
	bool bInitCheck = false;
	UPROPERTY(EditAnywhere, meta=(EditCondition="!bOrbSlotFilter"))
	TMap<EAttributeType, TSoftObjectPtr<class UTexture2D>> Images;
	UPROPERTY(EditAnywhere, meta=(EditCondition="!bOrbSlotFilter"))
	TMap<EAttributeType, FText> Texts;
	UPROPERTY(EditAnywhere, meta=(EditCondition="!bOrbSlotFilter"))
	EAttributeType FilterType;

	UPROPERTY(EditAnywhere, meta=(EditCondition="bOrbSlotFilter"))
	TMap<EOrbMatrixSlotType, TSoftObjectPtr<class UTexture2D>> SlotImage;
	UPROPERTY(EditAnywhere, meta=(EditCondition="bOrbSlotFilter"))
	TMap<EOrbMatrixSlotType, FText> SlotText;
	UPROPERTY(EditAnywhere, meta=(EditCondition="bOrbSlotFilter"))
	EOrbMatrixSlotType SlotType;

	UPROPERTY()
	TWeakObjectPtr<class UOrbElementListWidget> OrbElementListWidget;

public:
	FOnCheckBoxComponentStateChanged OnCheckBoxComponentStateChanged;

	UFUNCTION()
	void OnCheckStateChangedEvent(bool bIsChecked);

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
};
