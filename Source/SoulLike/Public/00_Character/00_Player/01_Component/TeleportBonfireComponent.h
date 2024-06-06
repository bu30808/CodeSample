// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "93_SaveGame/SoulLikeSaveGame.h"
#include "Components/ActorComponent.h"
#include "TeleportBonfireComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULLIKE_API UTeleportBonfireComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTeleportBonfireComponent();

protected:
	UPROPERTY()
	TObjectPtr<class APlayerCharacter> ComponentOwnerCharacter;
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Animation")
	class UAnimMontage* TeleportMontage;
	//이동 후에 플레이할 몽타주
	UPROPERTY(EditAnywhere, Category="Animation")
	class UAnimMontage* TeleportEndMontage;

	UFUNCTION()
	void PlayTeleportEndMontage();
	
	void StartTeleportToOtherBonfire();

private:
	//이동할 다른 화톳불이 있는 레벨 이름입니다.
	UPROPERTY()
	FBonfireInformation TeleportOtherBonfireInformation;

public:
	//다른 화톳불로 이동하는 함수입니다.
	void TeleportToOtherBonfire(const FBonfireInformation& TeleportBonfireInforation);
	UFUNCTION()
	void OnPlayTeleportMontage(UAnimMontage* Montage, bool bInterrupted);
};
