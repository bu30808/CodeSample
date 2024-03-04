// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "97_Interface/InteractionInterface.h"
#include "Components/BillboardComponent.h"
#include "GameFramework/Actor.h"
#include "Ladder.generated.h"

UCLASS()
class SOULLIKE_API ALadder : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALadder();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	/*UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class USceneComponent* SceneRootComponent;*/

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class USceneComponent* PoleRootComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UStaticMeshComponent* LadderPoleRightMeshComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UStaticMeshComponent* LadderPoleLeftMeshComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UBoxComponent* TopBoxComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UBoxComponent* BottomBoxComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UInstancedStaticMeshComponent* BarInstancedStaticMeshComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UInstancedStaticMeshComponent* ScrewInstancedStaticMeshComponent;



	
	//사다리를 탈 아래 위치입니다.(올라갈 때)
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UBillboardComponent* LadderEnter_Bottom;
	//사다리를 탈 위 위치입니다.(내려갈 때)
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UBillboardComponent* LadderEnter_Top;

	//사다리를 내릴때 아래 위치입니다(내려갈 때)
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UBillboardComponent* LadderExit_Bottom;
	//사다리를 내릴때 위 위치입니다(올라갈 때)
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UBillboardComponent* LadderExit_Top;


public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float LadderGap = 17.f;

	class USceneComponent* GetEnterBottom() const {return LadderEnter_Bottom;}
	class USceneComponent* GetEnterTop() const {return LadderEnter_Top;}
	class USceneComponent* GetExitBottom() const {return LadderExit_Bottom;}
	class USceneComponent* GetExitTop() const {return LadderExit_Top;}
};
