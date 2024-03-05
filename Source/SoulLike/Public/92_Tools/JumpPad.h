// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "97_Interface/InteractionInterface.h"
#include "GameFramework/Actor.h"
#include "JumpPad.generated.h"

UENUM(BlueprintType)
enum class EJumpMoveType : uint8
{
	UP,
	DOWN,
	NONE
};

UCLASS()
class SOULLIKE_API AJumpPad : public AActor,public IInteractionInterface
{
	GENERATED_BODY()

	friend class UJumpMovementComponent;
protected:
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* RootSceneComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UBillboardComponent* JumpPoint_Top;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UBillboardComponent* JumpPoint_Bottom;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UBoxComponent* TopBoxComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UBoxComponent* BottomBoxComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UStaticMeshComponent* ActivateMeshComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UNiagaraComponent* ActivateParticleComponent;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bIsActivated = false;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float MoveTime = 0.5f;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class APlayerCharacter> OverlappedPlayer;
	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> CurOverlappedComponent;
	UPROPERTY(Transient,VisibleAnywhere)
	EJumpMoveType JumpMoveType = EJumpMoveType::NONE;

public:	
	// Sets default values for this actor's properties
	AJumpPad();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	void TryActivatePad();

	virtual void Interaction_Implementation(ABaseCharacter* Start) override;
	virtual void FinishInteraction_Implementation() override;
	
	UFUNCTION()
	void OnBoxComponentBeginOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                     const FHitResult& SweepResult);
	UFUNCTION()
	void OnBoxComponentEndOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UWidgetComponent* SetInteractionWidget();
	bool IsNearActivateMesh(UPrimitiveComponent* CheckTargetComponent,UPrimitiveComponent* CompareComponent);


	UFUNCTION()
	void PlayJumpStartAnimation();
	UFUNCTION()
	void JumpMove();
	UFUNCTION()
	void JumpLand();
	UFUNCTION()
	void JumpEnd();
	
	UFUNCTION()
	void StartActivatePad();
public:
	void SetActivate();
};
