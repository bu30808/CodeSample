// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/InventoryComponent.h"

#include "00_Character/03_Monster/BaseMonster.h"
#include "99_Drag/DropTargetWidget.h"
#include "MainWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UMainWidget : public UDropTargetWidget
{
	GENERATED_BODY()

	friend class UQuickSlotButtonWidget;
	friend class AEquipmentItemActor_OrbCore;
	friend class USoulLikeSaveGame;
	friend class UGameLoadHandler;
	friend class UCharacterTotalInfoWidget;
protected:
	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* CanvasPanel;

	UPROPERTY(meta = (BindWidget),BlueprintReadOnly)
	class UResourceWidget* UMG_HP;
	UPROPERTY(meta = (BindWidget),BlueprintReadOnly)
	class UResourceWidget* UMG_MP;
	UPROPERTY(meta = (BindWidget),BlueprintReadOnly)
	class UResourceWidget* UMG_SP;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UNavigationWidget* UMG_Navigation;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UCharacterTotalInfoWidget* UMG_CharacterInformation;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UInventoryWidget* UMG_Inventory;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UEquipWidget* UMG_Equipment;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UOrbWidget* UMG_Orb;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UUserWidget* UMG_Option;
	
	/*UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UMainMenuWidget* UMG_MainMenu;*/
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UQuickSlotWidget* UMG_QuickSlot;


	UPROPERTY(meta = (BindWidget))
	class UAbilityDebugWidget* UMG_AbilityDebug;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* VerticalBox_BossWidgets;
	UPROPERTY()
	TMap<FString,class UBossHealthBarWidget*> BossHealthBarWidgetMap;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UBossHealthBarWidget> BossHealthBarWidgetObject;
	/*UPROPERTY(meta = (BindWidget))
	class UBossHealthBarWidget* UMG_Boss;*/

	UPROPERTY(meta = (BindWidget))
	class UStatusEffectListWidget* UMG_StatusEffectList;


	UPROPERTY(meta=(BindWidget))
	class UButton* Button_NavigationOpen;
	
	UPROPERTY(meta=(BindWidgetAnim),Transient,BlueprintReadOnly)
	class UWidgetAnimation* OpenMenuAnimation;
	UPROPERTY()
	bool bIsNavigationOpen = false;
	
	UPROPERTY(EditAnywhere)
	class UInputMappingContext* NavigationMenuInputContext;
	
public:
	UFUNCTION(BlueprintCallable)
	void OpenNavigation(bool bForceClose = false);
	

	void ShowBossWidget(class ABaseMonster* Boss);
	void RemoveBossWidget(ABaseMonster* Boss);
	
	void ForceUpdateSpirit(const FInventoryItem& InventoryItem);

protected:
	UPROPERTY()
	TWeakObjectPtr<class ABaseCharacter> OwnerCharacter;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnClickedOpenButton();
	
	void Init();

private:
	UFUNCTION()
	void OnChangedHPEvent(float HP, float MaxHP);
	UFUNCTION()
	void OnChangedMPEvent(float MP, float MaxMP);
	UFUNCTION()
	void OnChangedSPEvent(float SP, float MaxSP);

	UFUNCTION()
	void OnChangedMaxHPEvent(float MaxValue);
	UFUNCTION()
	void OnChangedMaxMPEvent(float MaxValue);
	UFUNCTION()
	void OnChangedMaxSPEvent(float MaxValue);
	
	virtual bool WidgetMoveToDraggedPoint(class UDragAndDropOperation* Oper,
	                                      const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent) override;


	void NavigationOpenInputSetting();
public:

	UFUNCTION()
	void OpenCharacterInformation();
	UFUNCTION()
	void OpenInventory();
	UFUNCTION()
	void OpenEquipment();
	UFUNCTION()
	void OpenOrb();
	UFUNCTION()
	void OpenOption();

	
};
