// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/04_NPC/NPCBase.h"
#include "03_Widget/07_NPC/00_Bonfire/BonfireTeleportWidget.h"
#include "Components/ActorComponent.h"
#include "BonfireComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBonfire, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRest, class APlayerCharacter*, Player);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULLIKE_API UBonfireComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBonfireComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;

public:
	UPROPERTY(Blueprintable)
	FOnRest OnRest;
	UPROPERTY(Transient)
	TWeakObjectPtr<class UBonfireTeleportWidget> BonfireTeleportListWidget;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UBonfireTeleportWidget> BonfireTeleportListWidgetObject;
	UPROPERTY(EditAnywhere)
	class UDataTable* TeleportDataList;

	void Rest(APlayerCharacter* Player);
	UFUNCTION()
	void OnBonfireTeleportListWidgetVisibilityChangedEvent(ESlateVisibility InVisibility);
	void CreateTeleportListWidget(APlayerCharacter* Player);
	void AddTeleportListWidget(class APlayerCharacter* Player);

protected:
	//체력 및 신력을 회복합니다.
	UFUNCTION()
	void Recover(APlayerCharacter* Player);
	//회복약을 보충합니다.
	UFUNCTION()
	void PotionReplenishment(APlayerCharacter* Player);
};
