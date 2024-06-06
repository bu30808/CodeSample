// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "Components/SplineComponent.h"
#include "BTTask_MoveTasks.generated.h"

enum class EDirection : uint8;
/**
 * 
 */
UCLASS()
class SOULLIKE_API UBTTask_MoveRandomPoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_MoveRandomPoint();
	UFUNCTION()
	void OnMoveCompleteEvent(FAIRequestID RequestID, EPathFollowingResult::Type Result);


	FVector GetRandomMoveDirection(APawn* Pawn);
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	//virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	FOnAIMoveComplete OnAIMoveComplete;

	UPROPERTY(EditAnywhere)
	float Distance = 1500.f;
};


/*
 *	특정 방향으로 N만큼 움직입니다.
 */
UCLASS()
class SOULLIKE_API UBTTask_MoveToDirection : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_MoveToDirection();

	UFUNCTION()
	void OnMoveCompleteEvent(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	static FVector GetControlDirLocation(AAIController* AIController, EDirection Direction, float Distance = 1,
	                                     bool bRandom = false, float RandMin = 0, float RandMax = 0);
	static FVector GetDirLocation(AAIController* AIController, EDirection Direction, float Distance = 1,
	                              bool bRandom = false, float RandMin = 0, float RandMax = 0);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	                            EBTNodeResult::Type TaskResult) override;

	UPROPERTY(EditAnywhere)
	EDirection MoveDirection;
	UPROPERTY(EditAnywhere)
	bool bUseControlDirection = false;
	UPROPERTY(EditAnywhere)
	bool bRandomInMoveDistance = false;
	UPROPERTY(EditAnywhere, meta=(EditCondition="!bRandomInMoveDistance"))
	float MoveDistance;
	UPROPERTY(EditAnywhere, meta=(EditCondition="bRandomInMoveDistance"))
	float RandomRangeMin;
	UPROPERTY(EditAnywhere, meta=(EditCondition="bRandomInMoveDistance"))
	float RandomRangeMax;
	UPROPERTY(EditAnywhere)
	float AcceptanceRadius;
	UPROPERTY()
	TObjectPtr<AAIController> AICon;
};


UCLASS()
class SOULLIKE_API UBTTask_JumpMove : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<AAIController> AICon;
	UPROPERTY(EditAnywhere)
	float OffsetPower = 1.f;
	UPROPERTY(EditAnywhere)
	EDirection MoveDirection;
	UPROPERTY(EditAnywhere)
	bool bUseControlDirection = false;

	UBTTask_JumpMove();
	UFUNCTION()
	void OnJumpEnded(const FHitResult& Hit);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	                            EBTNodeResult::Type TaskResult) override;
};


UCLASS()
class SOULLIKE_API UBTTask_MoveToWithBlackboardCheck : public UBTTask_MoveTo
{
	GENERATED_BODY()

protected:
	UBTTask_MoveToWithBlackboardCheck();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};




UCLASS()
class SOULLIKE_API UBTTask_MoveAlongSpline : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_MoveAlongSpline();
	void SaveOriginalValues(ACharacter* character);
	void ApplyFocusSetting(ACharacter* character);
	void RestoreOriginalValues(ACharacter* character);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	void MoveFinish();
	void Move(const FVector& nextPoint);
	void MoveToNextPoint();

	//이 값이 참이면 거꾸로 이동합니다.
	UPROPERTY(EditAnywhere)
	bool bReverse = false;
	UPROPERTY(EditAnywhere)
	bool bUseSmoothFocus = false;
	UPROPERTY(EditAnywhere,meta=(EditCondition= "bUseSmoothFocus"))
	float SmoothFocusInterpSpeed = 30.0f;
	//값이 낮을수록 천천히 회전합니다. -값은 즉시 회전합니다.
	UPROPERTY(EditAnywhere,meta=(EditCondition= "bUseSmoothFocus"))
	FRotator RotationRate = FRotator(0,360,0);
	
	UPROPERTY()
	TWeakObjectPtr<class USplineComponent> SplineComp;
	//현재 이동 타겟으로 잡힌 스플라인 포인트 번호입니다.
	UPROPERTY(Transient)
	int32 CurMoveSplinePoint = 0;
	UPROPERTY(Transient)
	TWeakObjectPtr<class AAIController> AICon;
	FRotator OriginalRotationRate;
	float OriginalSmoothFocusInterpSpeed;
	bool bOriginalOrientRotationToMovement;
	bool bOriginalUseControllerRotationYaw;
	bool bOriginalUseControllerDesiredRotation;
	
	UFUNCTION()
	void OnAIMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);
};


//리더에게 속도와 방향만 맞춰서 움직입니다.
UCLASS()
class SOULLIKE_API UBTTask_MoveWithLeader : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTTask_MoveWithLeader();
	void MoveToLeader();
	void OverrideMoveSpeed();
	void SaveOriginalValues();
	void RestoreOriginalValues();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	
	UPROPERTY(Transient)
	TWeakObjectPtr<class AAIController> AICon;

	UPROPERTY(EditAnywhere)
	float AcceptanceRadius;
	UPROPERTY(EditAnywhere)
	float FollowDistance = 200.f;
	//이 값이 참이면, 이동속도를 리더의 이동속도로 덮어씁니다.
	UPROPERTY(EditAnywhere)
	bool bOverrideToLeaderMoveSpeed = false;
	//bOverrideToLeaderMoveSpeed가 참일때, 리더의 이동속도에 추가로 이 값을 곱한 결과를 최종 이동속도로 합니다.
	UPROPERTY(EditAnywhere,meta=(EditCondition = "bOverrideToLeaderMoveSpeed"))
	float SpeedMultiplier = 1.f;
	
	float OriginalMoveSpeed;
	float TimeSinceLastUpdate;

	//이 시간마다 이동할 위치를 갱신합니다.
	UPROPERTY(EditAnywhere)
	float UpdateInterval = 0.5f;

	UFUNCTION()
	void OnMoveFinish(FAIRequestID RequestID, EPathFollowingResult::Type Result);
};