// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "97_Interface/InteractionInterface.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "DoorActor.generated.h"

UENUM(BlueprintType)
enum class EDoorState : uint8
{
	OPEN,
	CLOSED
};

UENUM(BlueprintType)
enum class EDoorInteractionPoint : uint8
{
	FRONT,
	BACK
};

UENUM(BlueprintType)
enum class EDoorType : uint8
{
	PUSH,
	SLIDE
};


UCLASS()
class SOULLIKE_API ADoorActor : public AActor ,public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorActor();

protected:
	
	virtual void PostInitializeComponents() override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class USceneComponent* SceneComponent;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UStaticMeshComponent* DoorComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UBoxComponent* BoxComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UNiagaraComponent* NiagaraComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UTimelineComponent* TimelineComponent;
	
	UPROPERTY(EditAnywhere)
	class UCurveFloat* DoorMoveCurve;
	
	FOnTimelineFloat OnUpdateTransform;
	FOnTimelineEvent OnFinishDoorMove;

	//상호작용중인 플레이어 캐릭터
	UPROPERTY()
	TWeakObjectPtr<class APlayerCharacter> Player;

	UFUNCTION(BlueprintNativeEvent)
	void OnUpdateDoorTransformEvent(float Alpha);
	virtual void OnUpdateDoorTransformEvent_Implementation(float Alpha);
	
	
	UFUNCTION()
	void OnFinishDoorMoveEvent();

	//문의 현 상태를 나타냅니다.
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EDoorState DoorState = EDoorState::CLOSED;
	//문의 정면에서 행동하는것인가? 후면에서 행동하는 것인가 저장하는 변수
	UPROPERTY(VisibleAnywhere)
	EDoorInteractionPoint DoorInteractionPoint;
	UPROPERTY(EditAnywhere)
	EDoorType DoorType;
	//이 시간에 걸쳐 문이 열리거나 닫힙니다.
	UPROPERTY(EditAnywhere)
	float DoorMoveTime = 0.3f;
	UPROPERTY(EditAnywhere)
	class USoundBase* DoorOpenSound;
	
	UFUNCTION(BlueprintCallable,BlueprintPure)
	bool IsOpen() const {return DoorState == EDoorState::OPEN;}

	UPROPERTY(EditAnywhere)
	float FrontYawValue = 90.f;
	UPROPERTY(EditAnywhere)
	float BackYawValue = -90.f;
	//닫힌 상태의 Yaw로테이션 값입니다.
	UPROPERTY(Transient)
	float ClosedYawRotation;
	

	UFUNCTION()
	void OnActorBeginOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
								  const FHitResult& SweepResult);
	UFUNCTION()
	void OnActorEndOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	
#if WITH_EDITOR
	UFUNCTION(CallInEditor,BlueprintCallable)
	virtual void UpdateBoxExtent();
	UFUNCTION(CallInEditor,BlueprintCallable)
	void SetBoxLocationZ();
	UFUNCTION(CallInEditor,BlueprintCallable)
	void AddXLocation_Right();
	UFUNCTION(CallInEditor,BlueprintCallable)
	void AddXLocation_Left();
	UFUNCTION(CallInEditor,BlueprintCallable)
	void AddYLocation_Right();
	UFUNCTION(CallInEditor,BlueprintCallable)
	void AddYLocation_Left();
#endif
	virtual void Interaction_Implementation(ABaseCharacter* Start) override;
};

UCLASS()
class SOULLIKE_API ADoorActor_Slide : public ADoorActor
{
	GENERATED_BODY()
public:
	ADoorActor_Slide();

protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UStaticMeshComponent* SecondDoorComponent;
	//닫힌 상태일때의 세컨드 슬라이드 도어의 위치
	UPROPERTY(Transient)
	FVector ClosedRelativeLocation;
	
	virtual void PostInitializeComponents() override;
	
	virtual void OnUpdateDoorTransformEvent_Implementation(float Alpha) override;
#if WITH_EDITOR
	virtual void UpdateBoxExtent() override;
#endif
	virtual void Interaction_Implementation(ABaseCharacter* Start) override;
};