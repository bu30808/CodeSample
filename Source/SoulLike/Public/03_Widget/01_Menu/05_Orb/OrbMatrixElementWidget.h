// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraEmitterHandle.h"
#include "OrbListButtonWidget.h"
#include "OrbWidget.h"
#include "03_Widget/07_NPC/OrbMatrixSlotOpenWidget.h"
#include "03_Widget/99_Drag/DropTargetWidget.h"
#include "95_OrbCreator/OrbMatrix.h"
#include "OrbMatrixElementWidget.generated.h"

enum class EAttributeType : uint8;
enum class EOrbMatrixSlotType : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnClickedMatrixElement, const FOrbMatrixElementInfo&, MatrixInfo,
                                             class UOrbMatrixElementWidget*, Widget);

/**
 * 
 */
UCLASS()
class SOULLIKE_API UOrbMatrixElementWidget : public UDraggableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FOnEquipCore OnEquipCore;
	UPROPERTY()
	FOnEquipFragment OnEquipFragment;
	UPROPERTY()
	FLinearColor CoreColor;

	//슬롯 잠김 여부
	UPROPERTY(VisibleAnywhere)
	bool bIsLocked;

	/*UPROPERTY(meta=(BindWidget))
	class UButton* Button;*/
	UPROPERTY(meta=(BindWidget))
	class UImage* Image_Line;

	UPROPERTY()
	FOnClickedMatrixElement OnClickedMatrixElement;

	//이 버튼이 가지고 있는 슬롯 정보.
	UPROPERTY(EditAnywhere)
	FOrbMatrixElementInfo MatrixElementInfo;

	//파편 장착시 재생될 효과음
	UPROPERTY(EditAnywhere)
	class USoundBase* FragmentEquippedSound;

	UPROPERTY(EditAnywhere)
	class USoundBase* HoveredSound;
	UPROPERTY(EditAnywhere)
	USoundBase* ExchangeSound;

	UPROPERTY()
	EOrbMatrixElementType ElementType = EOrbMatrixElementType::Normal;

protected:
	UPROPERTY(EditAnywhere)
	TMap<EOrbMatrixSlotType, FLinearColor> OrbColor;


	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	class UNiagaraSystemWidget* OrbParticle;
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	class UNiagaraSystemWidget* OrbParticle_Empty;
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	class UImage* Image_Locked;

	UFUNCTION()
	void UnEquipPreCore(FGuid PreCoreUniqueID);
	UFUNCTION()
	bool UnEquipPreFragment(FGuid PreFragmentUniqueID);


	UFUNCTION()
	void OnEquipFragmentEvent(const FInventoryItem& OrbFragmentItem);

	UFUNCTION()
	void OnClicked();

	UFUNCTION()
	void OnHoveredEvent();

	virtual void NativeConstruct() override;

	bool IsSameType(EOrbMatrixSlotType SlotType);
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	                                  UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	                          UDragDropOperation* InOperation) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	UPROPERTY(EditAnywhere)
	TMap<EOrbMatrixSlotType, FLinearColor> ColorForSlotType;
	UPROPERTY(EditAnywhere)
	class UTexture2D* LineSlotTexture;

	void ChangeElementImageStyle(UObject* NewTexture, float Alpha = 1.f) const;
	const FOrbFragmentInformation* GetOrbFragmentInformation(const FOrbMatrixElementInfo& Info);
	bool ExchangeCheck(const FOrbMatrixElementInfo& A, const FOrbMatrixElementInfo& B);

	bool CanExchange(const FOrbMatrixElementInfo& OtherMatrixElementInfo);
	void ExchangeData(UOrbMatrixElementWidget* OtherWidget);

	UFUNCTION()
	void SetLine();
	UFUNCTION()
	void SetCore();
	UFUNCTION()
	void SetPoint();
	void ChangeOrbParticleState();

	UPROPERTY()
	TWeakObjectPtr<class UOrbWidget> OrbWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UOrbMatrixElementDragShadowWidget> CustomDragShadowWidgetObject;

public:
	void SetInfo(const FOrbMatrixElementInfo& OrbMatrixElementInfo);

	EOrbMatrixSlotType GetOrbMatrixSlotType() const { return MatrixElementInfo.OrbMatrixSlotType; }

	void SetOwnerWidget(TWeakObjectPtr<class UOrbWidget> OwnerWidget) { OrbWidget = OwnerWidget; }
	void ClearSlot();

	void UpdateToolTip(const FGuid& UniqueID);
	void UpdateToolTip(const FInventoryItem& OrbItem);
	void Refresh(const FOrbMatrixElementInfo& RefreshedInfo);

	const FOrbMatrixElementInfo& GetCurMatrixElementInfo() const { return MatrixElementInfo; }
	//저장기능을 위해서 기존 메트릭스에 행렬값을 이용해 정보를 덮어씁니다.
	void OverrideMatrixSlot();
	//저장되었던 이 슬롯의 정보를 가져와 갱신합니다.
	void LoadMatrixSlot();

	void EquipOrbFragment(const FInventoryItem& OrbFragmentItem);
	virtual UUserWidget* CreateCustomDragShadowWidget() override;
};

UCLASS()
class SOULLIKE_API UOrbMatrixElementDragShadowWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	class UNiagaraSystemWidget* OrbParticle;

	//이 슬롯을 드래그했습니다.
	UPROPERTY()
	TWeakObjectPtr<class UOrbMatrixElementWidget> DraggedWidget;
	UPROPERTY()
	FLinearColor ColorToChange;

	virtual void NativeConstruct() override;
	void SetParticleColor(FLinearColor Color);
};
