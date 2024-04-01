// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
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

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	//virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	                            EBTNodeResult::Type TaskResult) override;
	virtual FString GetStaticDescription() const override;
	//가져올 범위 반지름
	UPROPERTY(EditAnywhere)
	float Radius;
	UPROPERTY()
	TObjectPtr<AAIController> AICon;
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
