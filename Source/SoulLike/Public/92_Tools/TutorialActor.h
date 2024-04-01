// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "TutorialActor.generated.h"

UCLASS()
class SOULLIKE_API ATutorialActor : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UBoxComponent> BoxComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UBillboardComponent> BillboardComponent;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTutorialWidget> TutorialWidgetObject;

	UPROPERTY(EditAnywhere)
	FGameplayTag TutorialTag;

	/*UPROPERTY(EditAnywhere)
	class UTexture2D* TutoImage;
	UPROPERTY(EditAnywhere,meta=(MultiLine = "true"))
	FText TutoMsg;*/

	UPROPERTY(Transient)
	TWeakObjectPtr<UTutorialWidget> TutoWidget;

public:
	// Sets default values for this actor's properties
	ATutorialActor();

protected:
	UFUNCTION()
	void OnDestroyedEvent(AActor* DestroyedActor);

	virtual void PostInitializeComponents() override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnActorBeginOverlapEvent(AActor* OverlappedActor, AActor* OtherActor);
};
