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

UENUM(BlueprintType)
enum class EJumpProcess : uint8
{
	JUMP_START,
	JUMPING,
	JUMP_LAND,
	JUMP_END,
	NONE
};

UCLASS()
class SOULLIKE_API AJumpPad : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

	friend class UJumpMovementComponent;

protected:
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* RootSceneComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBillboardComponent* JumpPoint_Top;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBillboardComponent* JumpPoint_Bottom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* TopBoxComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* BottomBoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* ActivateMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UNiagaraComponent* ActivateParticleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsActivated = false;
	//컴포넌트 무브 함수를 통해서 플레이어를 강제 이동시킬 때, 이동 시간입니다. 짧을수록 빠르게 움직입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ComponentMoveTime = 0.5f;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class APlayerCharacter> OverlappedPlayer;
	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> CurOverlappedComponent;
	UPROPERTY(Transient, VisibleAnywhere)
	EJumpMoveType JumpMoveType = EJumpMoveType::NONE;
	UPROPERTY(Transient, VisibleAnywhere)
	EJumpProcess JumpProcess = EJumpProcess::NONE;

	UPROPERTY(Transient)
	FVector MoveTargetPoint;
	//점프중 이동하게되는 속도입니다.
	UPROPERTY(EditAnywhere)
	float MoveSpeed = 1000.f;
	//점프로 이동중 목표지점과의 거리가 이 수치 이하가 된다면, 이동완료처리를 합니다.
	UPROPERTY(EditAnywhere)
	float MoveCompleteCheckDistance = 25.f;

public:
	// Sets default values for this actor's properties
	AJumpPad();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaSeconds) override;

	void TryActivatePad();

	virtual void Interaction_Implementation(ABaseCharacter* Start) override;
	virtual void FinishInteraction_Implementation() override;

	UFUNCTION()
	void OnBoxComponentBeginOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                     const FHitResult& SweepResult);
	UFUNCTION()
	void OnBoxComponentEndOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UWidgetComponent* ShowInteractionWidget();
	void HideInteractionWidget();

	bool IsNearActivateMesh(UPrimitiveComponent* CheckTargetComponent, UPrimitiveComponent* CompareComponent);


	UFUNCTION()
	void PlayJumpStartAnimation();
	UFUNCTION()
	void JumpMove();
	UFUNCTION()
	void JumpDown();
	UFUNCTION()
	void JumpLand();

	UFUNCTION()
	void StartActivatePad();

public:
	void SetActivate();
};
