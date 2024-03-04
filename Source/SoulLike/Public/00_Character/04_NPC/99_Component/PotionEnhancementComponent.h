// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "Components/ActorComponent.h"
#include "PotionEnhancementComponent.generated.h"


UENUM(BlueprintType)
enum class EEnhancedPotionType:uint8
{
	HP,MP
};

struct FPotionInformation;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpgradePotion,const FGuid&,ID,class APotionItemActor*, PotionItemActor);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULLIKE_API UPotionEnhancementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPotionEnhancementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void PostInitProperties() override;

public:
	UFUNCTION()
	void OnPotionEnhancementWidgetVisibilityChangedEvent(ESlateVisibility InVisibility);
	
	void CreatePotionEnhancementWidget(const class ABaseCharacter* InteractPlayer);
	void AddPotionEnhancementWidget(const class ABaseCharacter* InteractPlayer);

protected:
	UPROPERTY()
	TSubclassOf<class UPotionEnhancementWidget> PotionEnhancementWidgetObject;
	UPROPERTY(Transient)
	TWeakObjectPtr<class UPotionEnhancementWidget> PotionEnhancementWidget;

	UPROPERTY()
	FOnUpgradePotion OnUpgradePotion;
	
	TArray<FEnhancementMaterial*> PotionEnhancementMaterial;

public:
	const FEnhancementMaterial& GetNextEnhancementInfo(const int32& CurEnhancement);
	bool CanEnhancePotion(class UInventoryComponent* InventoryComponent,EEnhancedPotionType EnhancedPotionType);
	void UpgradePotion(UInventoryComponent* InventoryComponent,EEnhancedPotionType EnhancedPotionType);


private:
	const FInventoryItem* GetHPPotionItem(UInventoryComponent* InventoryComponent);
	const FInventoryItem* GetMPPotionItem(UInventoryComponent* InventoryComponent);

};
