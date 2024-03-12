// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_AITurn.generated.h"

UENUM(BlueprintType)
enum class ETurnDirection :uint8
{
	RIGHT,
	LEFT,
	MAX
};

DECLARE_DELEGATE(FTurnMontageBlendOut);
DECLARE_DELEGATE(FTurnMontageStopCheck);
/**
 * 
 */

UENUM(BlueprintType)
enum class EAITaskTurnType :uint8
{
	//몽타주 루트모션을 이용한 회전
	RootMontage,
	//몽타주를 이용하지만, 루트모션이 없어서 틱으로 회전합니다.
	MontageWithTick,
	//몽타주를 사용하지 않고 그냥 틱으로만 회전합니다.
	TickWithoutMontage
};

UENUM(BlueprintType)
enum class EStopTraceType :uint8
{
	//컨트롤 회전
	ControlDirection,
	//캐릭터 전방
	PawnForward,
	//메시 전방
	MeshForward
};

UCLASS()
class SOULLIKE_API UBTTask_AITurn : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere)
	EAITaskTurnType TurnType;
	UPROPERTY(EditAnywhere, meta=(EditCondition= "TurnType != EAITaskTurnType::TickWithoutMontage"),Category="Montage")
	TObjectPtr<class UAnimMontage> TurnRightMontage;
	UPROPERTY(EditAnywhere, meta=(EditCondition= "TurnType != EAITaskTurnType::TickWithoutMontage"),Category="Montage")
	TObjectPtr<class UAnimMontage> TurnLeftMontage;
	
	FTurnMontageBlendOut OnMontageBlendOut;
	
	//이 각도 안으로 들어오면 멈춥니다.
	UPROPERTY(EditAnywhere,Category="Option")
	float StopDeltaYaw = 30.f;

	//이 값이 참이면, MaxTryCount만큼 회전시도했지만 회전이 끝나지 않은 경우 실패처리합니다.
	UPROPERTY(EditAnywhere,meta=(EditCondition= "TurnType == EAITaskTurnType::RootMontage"),Category="Option")
	bool bUseTurnMaxTurnCount = false;
	
	//몽타주를 이용해서 회전하는 경우 최대 시도 횟수를 설정합니다. 이 횟수만큼 시도한 후에는 실패처리 됩니다.
	UPROPERTY(EditAnywhere, meta=(EditCondition= "bUseTurnMaxTurnCount && TurnType != EAITaskTurnType::TickWithoutMontage"),Category="Option")
	uint8 MaxTryCount = 0;
	UPROPERTY(VisibleAnywhere)
	uint8 CurTryCount = 0;
	//이 값이 참이면 전방으로 라인트레이스를 그리고, 라인트레이스에 타겟이 걸리면 완료한것으로 처리합니다.
	UPROPERTY(EditAnywhere,Category="Option")
	bool bUseStopTrace = false;
	//이 값이 참이면 컨트롤러 회전의 전방으로 트레이스를 그립니다.
	UPROPERTY(EditAnywhere,meta=(EditCondition="bUseStopTrace"),Category="Option")
	EStopTraceType StopTraceType;
	//메시 전방을 사용하려면 소켓의 이름이 필요합니다. 머리나 눈같은 소켓이름을 주세요.
	UPROPERTY(EditAnywhere,meta=(EditCondition="bUseStopTrace && StopTraceType == EStopTraceType::MeshForward"),Category="Option")
	FName SocketName = "head";
	
	UPROPERTY(EditAnywhere,meta=(EditCondition= "TurnType != EAITaskTurnType::RootMontage"),Category="Option")
	float InterpSpeed = 5.f;
	//Simple Parallel노드의 부 노드로 사용할때 사용하는 변수입니다. 먼저 테스크가 종료되지 못 하도록 합니다.
	UPROPERTY(EditAnywhere,meta=(EditCondition= "TurnType != EAITaskTurnType::RootMontage"))
	bool bShouldManualFinish = false;
	UPROPERTY()
	class AAIController* OwnersAIController;
	UPROPERTY()
	TObjectPtr<class UAnimMontage> CurSelectedMontage;
	UPROPERTY()
	bool bUseTick;

	UBTTask_AITurn();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	
	void Turn();
	
	UFUNCTION()
	void OnTurnMontageBlendOutEvent(UAnimMontage* Montage, bool bInterrupted);

	
};

/*
UCLASS(Blueprintable)
class SOULLIKE_API UTickingTest : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UTickingTest();
protected:

	UPROPERTY(EditAnywhere)
	bool bUseTick = false;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};*/