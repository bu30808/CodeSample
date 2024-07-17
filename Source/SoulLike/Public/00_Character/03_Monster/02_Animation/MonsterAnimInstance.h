// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/02_Animation/BaseAnimInstance.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "MonsterAnimInstance.generated.h"

/**
 * 몬스터 전용 애니메이션 인스턴스 클래스
 * 단순하게, 피격시 애니메이션을 재생하는 경우, 애니메이션 종료 후 다시 비헤이비어트리를 작동시킵니다.
 */
UCLASS()
class SOULLIKE_API UMonsterAnimInstance : public UBaseAnimInstance
{
	GENERATED_BODY()

protected:
	//특정 대상에 대해서 포커스를 가지고 있나?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsFocus = false;
	//AI가 플레이어를 찾아서 감지했는가?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsFindTarget = false;

	//죽었나요?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDead = false;

	//ChangeBoneTransform함수 내부에서 변경됩니다.
	//AI타겟을 바라보는데 사용됩니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator NeckRotation;


	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
		
	virtual void AnimNotify_OnHitEnter_Implementation() override;
	/*virtual void AnimNotify_OnHitExit_Implementation() override;*/

protected:
	virtual void ChangeBoneTransform_Implementation(float DeltaTime) override;
	virtual void ClearBoneTransform_Implementation(float DeltaTime) override;
};
