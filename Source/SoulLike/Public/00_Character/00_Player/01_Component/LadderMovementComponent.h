// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/BaseCharacter.h"
#include "Components/ActorComponent.h"
#include "LadderMovementComponent.generated.h"

UENUM(BlueprintType)
enum class ELadderHandOrder : uint8
{
	LEFT,
	RIGHT
};

UENUM(BlueprintType)
enum class ELadderMove : uint8
{
	UP,
	DOWN
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULLIKE_API ULadderMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULadderMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



protected:
	//이 컴포넌트의 주인
	UPROPERTY()
	TObjectPtr<class ABaseCharacter> Pawn;

	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	bool bUseLadderMovement = false;

	//상호작용중인 사다리입니다.
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class ALadder> LadderActor;

	//사다리를 타는 몽타주(올라갈 때)
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Montage")
	class UAnimMontage* LadderEnter_BottomMontage;
	//사다리를 타는 몽타주(내려갈 때)
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Montage")
	class UAnimMontage* LadderEnter_TopMontage;

	//사다리를 빠져나가는 몽타주(내려갈 때)
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Montage")
	class UAnimMontage* LadderExit_BottomMontage;
	//사다리를 빠져나가는 몽타주(올라갈 때)
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Montage")
	class UAnimMontage* LadderExit_TopMontage;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Montage")
	class UAnimMontage* LadderMoveMontage;

	//발견한 사다리의 난간에서 이 수치만큼을 제외하고 남은 Z만큼 내려갑니다 숫자가 작아지면 더 많이 내려가게 됩니다.
	UPROPERTY(EditAnywhere)
	float LadderMoveOffset = 110.f;
	UPROPERTY(Transient,BlueprintReadWrite)
	bool bIsMoving = false;
	UPROPERTY(Transient,BlueprintReadWrite)
	FVector HandLocation;
	UPROPERTY(Transient,BlueprintReadWrite)
	ELadderHandOrder LadderHandOrder = ELadderHandOrder::RIGHT;
	UPROPERTY(Transient,BlueprintReadWrite)
	ELadderMove LadderMoveType;
	//다음 손 위치를 구할 때, 얼마나 Z축으로 위, 아래에서부터 체크를 시작할 것인가 정합니다.
	UPROPERTY(EditAnywhere)
	float FindNextUpHandLocation_ZOffset = 30.f;
	UPROPERTY(EditAnywhere)
	float FindNextDownHandLocation_ZOffset = 30.f;
	//사다리를 한칸 올라가는데 걸리는 시간입니다.
	UPROPERTY(EditAnywhere)
	float MoveTime = 0.5f;

	//손 위치가 정확하지 않다면 이 값으로 조정하세요.
	UPROPERTY(EditAnywhere)
	FVector HandIKOffset = FVector(0,0,-17.5);

	UFUNCTION()
	void MoveFinish();
	
	FVector GetClimbMoveLocation();
	
public:
	UFUNCTION(BlueprintCallable,BlueprintPure)
	FVector GetNextHandLocation();
	UFUNCTION(BlueprintCallable)
	void SetUseLadderMovement(bool Value, class ALadder* LadderToUse);
	
	UFUNCTION(BlueprintCallable,BlueprintPure)
	bool GetUseLadderMovement(){return bUseLadderMovement;}

	void AddLadderMovementInput(float ScaleValue = 1.f);

	UFUNCTION()
	void OnPlayMontageBlendOutEvent(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintCallable,BlueprintPure)
	float GetLadderGap();

	void SetLadderHandOrder(ELadderHandOrder NewValue){LadderHandOrder = NewValue;}
};
