// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "03_Widget/01_Menu/05_Orb/OrbWidget.h"
#include "OrbMatrixSlotOpenWidget.generated.h"

USTRUCT(BlueprintType)
struct FOrbSlotUnlockCost : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 Cost = 0.f;
};

UENUM(BlueprintType)
enum class EOrbMatrixElementType : uint8
{
	//일반적인 신력구슬 텝
	Normal,
	//슬롯 개방용
	SlotOpen
};

/**
 * 
 */
UCLASS()
class SOULLIKE_API UOrbMatrixSlotOpenWidget : public UOrbWidget
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<class UOrbMatrixSlotOpenComponent> OrbMatrixSlotOpenComponent;
	UPROPERTY()
	TWeakObjectPtr<class UOrbMatrix> CoreMatrix;

	TArray<FOrbSlotUnlockCost*> UnlockTable;

	UPROPERTY()
	FOrbMatrixElementInfo SelectedMatrixElement;
	UPROPERTY()
	TWeakObjectPtr<class UOrbMatrixElementWidget> SelectedMatrixElementWidget;

	void ShowUnlockCost(int32 Cost);

	class UOrbMatrix* GetCoreMatrix(const FInventoryItem& OrbCoreItem);

	UFUNCTION()
	void OnClickedOrbMatrixElement(const FOrbMatrixElementInfo& MatrixInfo,
	                               UOrbMatrixElementWidget* MatrixElementWidget);

protected:
	UPROPERTY(EditAnywhere)
	class UDataTable* FragmentUnlockTable;

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_UnLock;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_UnLock;

public:
	void SetOrbMatrixSlotOpenComponent(class UOrbMatrixSlotOpenComponent* Component);

	UFUNCTION()
	void OnClickedUnLock();

	UFUNCTION()
	void OnClickedCloseButton();

	virtual void NativeConstruct() override;

	void Init();


	virtual void CreateOrbMatrixFirst(const FInventoryItem& OrbCoreItem) override;
	virtual void OverrideOrbMatrix(const FInventoryItem& OrbCoreItem) override;

	virtual void OnVisibilityChangedEvent(ESlateVisibility InVisibility) override;
};
