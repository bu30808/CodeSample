// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/BaseCharacter.h"
#include "03_Widget/07_NPC/OrbMatrixSlotOpenWidget.h"
#include "Components/ActorComponent.h"
#include "Engine/DataAsset.h"
#include "OrbMatrixSlotOpenComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlotOpen,class APlayerCharacter*,Player,class UOrbMatrix*,Matrix);

UCLASS(BlueprintType)
class SOULLIKE_API USlotOpenRewardDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<class UAbilityBase>> PhysicalLineReward;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<class UAbilityBase>> MagicalLineReward;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<class UAbilityBase>> DefenceLineReward;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<class UAbilityBase>> FreeLineReward;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULLIKE_API UOrbMatrixSlotOpenComponent : public UActorComponent
{
	GENERATED_BODY()
	friend class UOrbMatrix;
public:
	// Sets default values for this component's properties
	UOrbMatrixSlotOpenComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void PostInitProperties() override;
	
	UPROPERTY()
	TSubclassOf<class UOrbMatrixSlotOpenWidget> OrbMatrixSlotOpenWidgetObject;
	UPROPERTY(Transient)
	TWeakObjectPtr<class UOrbMatrixSlotOpenWidget> OrbMatrixSlotOpenWidget;
	UPROPERTY()
	class USlotOpenRewardDataAsset* SlotOpenRewardDataAsset;
	UPROPERTY()
	FOnSlotOpen OnSlotOpen;
public:
	UFUNCTION()
	void OnOrbMatrixSlotOpenWidgetVisibilityChangedEvent(ESlateVisibility InVisibility);
	void CreateOrbMatrixSlotOpenWidget(class ABaseCharacter* InteractPlayer);
	void AddOrbMatrixSlotOpenWidget(class ABaseCharacter* InteractPlayer);
	void UnLock(APlayerController* PlayerController, class UOrbMatrix* CoreMatrix,
	            const FOrbMatrixElementInfo& SelectedMatrixElement, UOrbMatrixElementWidget
	            * SelectedMatrixElementWidget, const TArray<FOrbSlotUnlockCost*>& UnlockTable);

private:
	bool IsAllOpen(const TArray<FOrbMatrixElementInfo>& Line);
	void GivePhysicalLineCompleteReward(APlayerController* PlayerController, bool bShowAlert);
	void GiveMagicalLineCompleteReward(APlayerController* PlayerController, bool bShowAlert);
	void GiveDefenceLineCompleteReward(APlayerController* PlayerController, bool bShowAlert);
	void GiveFreeLineCompleteReward(APlayerController* PlayerController, bool bShowAlert);
	//라인이 완성되었는지 확인하고, 그렇다면 보상을 줍니다.
	void GiveLineCompleteReward(APlayerController* PlayerController, const UOrbMatrix* OrbMatrix,
	                            EOrbMatrixSlotType OrbMatrixSlot, bool bShowAlert = true);
	void OpenSlot(UOrbMatrix* CoreMatrix, const FOrbMatrixElementInfo& SelectedMatrixElement);
	bool HaveEnoughCost(APlayerController* PlayerController, const int32& Cost);
	bool IsOpenPreSlot(const TArray<FOrbMatrixElementInfo>& Line, const int32& Index);
	TArray<FOrbMatrixElementInfo> GetLine(UOrbMatrix* CoreMatrix, EOrbMatrixSlotType OrbMatrixSlot);
};
