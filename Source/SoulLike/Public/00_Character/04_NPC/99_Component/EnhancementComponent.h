// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/04_NPC/NPCBase.h"
#include "03_Widget/06_Enhancement/EnhancementWidget.h"
#include "04_Item/ItemActor.h"
#include "Components/ActorComponent.h"
#include "EnhancementComponent.generated.h"


struct FEnhancementMaterial;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpgradeEquipment, const FGuid&, ID, class AEquipmentItemActor*,
                                             EquipmentItemActor);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULLIKE_API UEnhancementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEnhancementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void PostInitProperties() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UEnhancementWidget> EnhancementWidgetObject;
	UPROPERTY(Transient)
	TWeakObjectPtr<UEnhancementWidget> EnhancementWidget;

public:
	UPROPERTY()
	FOnUpgradeEquipment OnUpgradeEquipment;

protected:
	TArray<FEnhancementMaterial*> EnhancementMaterial;
	TArray<FAttributeValuePerEnhancement*> AttributeValuePerEnhancement;

public:
	UFUNCTION()
	void OnEnhancementWidgetVisibilityChangedEvent(ESlateVisibility InVisibility);
	void CreateEnhancementWidget(const class ABaseCharacter* InteractPlayer);
	void AddEnhancementWidget(const class ABaseCharacter* InteractPlayer, EEnhancementType EnhancementType);
	const FEnhancementMaterial& GetNextEnhancementInfo(const int32 CurEnhancement);
	const TArray<FAttributeValuePerEnhancement*>& GetAttributeValuePerEnhancement();
	bool CanEnhance(const class ABaseCharacter* Character, const FInventoryItem& Item);
	bool Upgrade(const class ABaseCharacter* Character, const FGuid& UniqueID, EAttributeType IncreaseAttribute);
};
