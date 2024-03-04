// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_SetFocus.generated.h"


UENUM(BlueprintType)
enum class EAIFocusType : uint8
{
	Target,
	Location
};

/**
 * 
 */
UCLASS()
class SOULLIKE_API UBTTask_SetFocus : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	bool bUseControllerRotationYaw;
	UPROPERTY(EditAnywhere)
	float SmoothFocusInterpSpeed = 30.0f;
	UPROPERTY(EditAnywhere)
	EAIFocusType AIFocusType;
	//이 값이 참이면, 괴물의 애니메이션 인스턴스 내부에서 포커스를 향해 본을 회전시킵니다.
	UPROPERTY(EditAnywhere)
	bool bModifySkeletonTransform;

	UBTTask_SetFocus();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
