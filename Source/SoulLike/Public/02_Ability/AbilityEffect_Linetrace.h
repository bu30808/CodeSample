// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "02_Ability/AbilityEffect.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilityEffect_Linetrace.generated.h"


UENUM(BlueprintType)
enum class ETraceType :uint8
{
	BY_CHANNEL,
	BY_OBJECT
};

UENUM(BlueprintType)
enum class ETraceShapeType :uint8
{
	LINE,
	BOX,
	CAPSULE,
	SPHERE
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHitActor, const FHitResult&, Hit, ABaseCharacter*, Target);

/**
 * 틱마다 라인트레이스를 생성하고, 히트된 대상에게 이팩트를 적용합니다.
 *  주의!! - 본 이름을 소켓이름으로 주면 안 됩니다.
 */
UCLASS()
class SOULLIKE_API UAbilityEffect_Linetrace : public UAbilityEffect
{
	GENERATED_BODY()

public:
	UAbilityEffect_Linetrace();

protected:
	//라인트레이스를 생성할 컴포넌트. 스태틱 or 스캘레톤
	//기존 메시가 변경될때마다 같이 수정해야 합니다.
	UPROPERTY()
	class UPrimitiveComponent* MeshComponent;

	//이 컴포넌트에 할당된 메시의 모든 소캣
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FName> Sockets;

public:
	//한 틱에 서로 다른 소켓끼리에 대해서 트레이스를 하려면 체크하세요.
	UPROPERTY(EditAnywhere, Category="TraceSetting")
	bool bTraceOtherSocketsAtSameTime = false;
	//이전 틱의 서로다른 소켓과 현 틱의 서로 다른 소켓끼리에 대해서 트레이스를 하려면 체크하세요
	UPROPERTY(EditAnywhere, Category="TraceSetting")
	bool bTraceOtherSocketsAtDifferentTime = true;

	//이 값이 참이면, 캐릭터가 아닌 다른 지형을 감지했을 때, 해당 위치에 파티클을 스폰합니다.
	UPROPERTY(EditAnywhere, Category="NiagaraTraceSetting")
	bool bSpawnNiagaraWhenHitNonCharacter = false;
	//이팩트를 스폰할 트레이스를 그리는 시간 주기를 설정합니다.
	UPROPERTY(EditAnywhere, Category="NiagaraTraceSetting")
	float NiagaraSpawnTraceTickRate = 0.15f;
	UPROPERTY(EditAnywhere, Category="NiagaraTraceSetting",meta=(EditCondition="bSpawnNiagaraWhenHitNonCharacter"))
	class UNiagaraSystem* NiagaraEmitter;

	UPROPERTY()
	TWeakObjectPtr<class UGameplayTask_LaunchEvent> EffectSpawnTickTask;
	UPROPERTY()
	TWeakObjectPtr<class AActor> NiagaraSpawnTarget;

	//체널을 기준으로 트레이스 할 것인지, 오브젝트를 기준으로 트레이스 할 것인지 정합니다.s
	UPROPERTY(EditAnywhere, Category = "TraceSetting")
	ETraceType TraceType;
	UPROPERTY(EditAnywhere, Category="TraceSetting", meta=(EditCondition="TraceType == ETraceType::BY_CHANNEL"))
	TEnumAsByte<ETraceTypeQuery> TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TraceSetting,
		meta = (Tooltip = "트레이스로 감지할 오브젝트 타입 배열", EditCondition="TraceType == ETraceType::BY_OBJECT"))
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {
		UEngineTypes::ConvertToObjectType(ECC_WorldStatic), UEngineTypes::ConvertToObjectType(ECC_WorldDynamic)
	};


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TraceSetting",
		meta = (Tooltip =
			"복잡한 형태의 콜리전에 대해 검사할 것인가?, 성능에 영향을 줄 수 있습니다. 또한 Mesh의 PerCollision을 감지하려는 경우, 이 값이 참이면 아무것도 감지하지 못합니다."))
	bool bTraceComplex = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TraceSetting", meta = (Tooltip = "무시할 대상"))
	TArray<AActor*> IgnoreActors;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TraceSetting")
	bool bIgnoreSelf = true;

	//무시하고자 하는 소켓이 있다면 추가하세요.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TraceSetting")
	TArray<FName> IgnoreSockets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TraceSetting, meta = (Tooltip = "트레이스 라인 컬러"))
	FLinearColor TraceColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TraceSetting, meta = (Tooltip = "트레이스가 무엇인가 감지했을 때, 표시할 컬러"))
	FLinearColor TraceHitColor = FLinearColor::Green;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TraceSetting, meta = (Tooltip = "디버깅용 라인을 어떻게 표시할 것인가?"))
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType = EDrawDebugTrace::Type::ForDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TraceSetting, meta = (Tooltip = "디버그 라인을 표시할 시간"))
	float DrawTime = 0;


	//트레이스 형태
	UPROPERTY(EditAnywhere, Category="TraceSetting")
	ETraceShapeType TraceShapeType;

	UPROPERTY(EditAnywhere, Category="TraceSetting|ShapeSetting",
		meta=(EditCondition="TraceShapeType == ETraceShapeType::BOX"))
	FVector BoxHalfSize;
	UPROPERTY(EditAnywhere, Category="TraceSetting|ShapeSetting",
		meta=(EditCondition="TraceShapeType == ETraceShapeType::BOX"))
	FRotator BoxOrientation;
	UPROPERTY(EditAnywhere, Category="TraceSetting|ShapeSetting",
		meta=(EditCondition="TraceShapeType == ETraceShapeType::CAPSULE"))
	float CapsuleRadius;
	UPROPERTY(EditAnywhere, Category="TraceSetting|ShapeSetting",
		meta=(EditCondition="TraceShapeType == ETraceShapeType::CAPSULE"))
	float CapsuleHalfHeight;
	UPROPERTY(EditAnywhere, Category="TraceSetting|ShapeSetting",
		meta=(EditCondition="TraceShapeType == ETraceShapeType::SPHERE"))
	float SphereRadius;

protected:
	//트레이스에 감지된 대상들을 저장하는 배열입니다.
	UPROPERTY(BlueprintReadWrite, Category="TraceResult")
	TArray<FHitResult> HitActors;
	

	//히트가 감지되면 호출되는 이벤트입니다.
	UPROPERTY(BlueprintAssignable)
	FOnHitActor OnHitActor;
	//히트가 감지되면 호출되는 함수입니다. 필요하다면 블루프린트에서 덮어쓰세요.
	UFUNCTION(BlueprintNativeEvent)
	void OnHitActorEvent(const FHitResult& Hit, ABaseCharacter* Target);


	//소켓의 이전 프레임의 위치를 기억하는 맵입니다.
	UPROPERTY()
	TMap<FName, FVector> LastKnownSocketLocation;

	//이 소켓 이름이 본 이름인지 확인합니다(스켈레탈 메시인 경우만)
	bool IsBoneName(FName SocketName) const;

	void UpdateLastSocketLocation();

	//소켓이름을 정해주려면 참으로 주세요.
	UPROPERTY(EditAnywhere)
	bool bUseWithCustomSockets;

	UPROPERTY(EditAnywhere, meta=(EditCondition = "bUseWithCustomSockets"))
	TArray<FName> CustomSockets;

	//타겟의 mesh 이외에 다른 메시 컴포넌트를 사용하려면 설정하세요.
	UPROPERTY(EditAnywhere)
	bool bUseOtherMeshComponent;

	//델타타임마다 피해를 쪼개서 주려면 참을 주세요
	UPROPERTY(EditAnywhere)
	bool bSplitDamageOverTime = false;

	//같은 팀(아군)인 경우에도 피해를 줄 것인가 결정합니다.
	UPROPERTY(EditAnywhere)
	bool bIgnoreAlly = true;
public:
	//초기화
	//트레이스를 그릴 메시가 변경될때마다 호출하세요.
	UFUNCTION(BlueprintCallable)
	void Init(UPrimitiveComponent* TargetComp);

	//특정 소캣들에 대해서만 트레이스를 그리려면 이 함수로 초기화 하세요.
	UFUNCTION(BlueprintCallable)
	void InitWithSockets(UPrimitiveComponent* TargetComp);

	UFUNCTION(BlueprintCallable)
	void ActivateTrace(bool bActive, class ABaseCharacter* Target);

#if WITH_EDITOR
	virtual void CopyValues(UAbilityEffect* Effect) override;
#endif


protected:
	/*/**
	 * 어트리뷰트값이 적용된 수치를 계산된 이후 호출됩니다.
	 * 위 결과를 가지고 특성을 적용한 값을 계산합니다.
	 * @param Target 이팩트가 적용중인 대상
	 * @param DamagedCharacter 피해를 입어야 할 대상
	 #1#
	void ApplyAbilityDamageTalent(ABaseCharacter* Target, ABaseCharacter* DamagedCharacter);

	/*
	 *	피해를 받는 대상이 플레이어일 때,
	 *	플레이어의 방어특성을 적용합니다.
	 
	virtual void ApplyAbilityDecreaseDamageTalent(ABaseCharacter* Target, ABaseCharacter* DamagedCharacter) override;#1#*/

protected:
	//피격된 대상들을 배열에 추가합니다. 배열을 비워주지 않는 이상 다시 피격되지 않습니다.
	void AddHitActors(TArray<FHitResult> HitArray, ABaseCharacter* Target);

	void SpawnStaticHitParticle(const AActor* Target, const FVector& HitLocation, const FVector& ImpactNormal) const;

	//한 틱당 서로 다른 소켓에 대해서 트레이스 합니다.
	UFUNCTION(BlueprintCallable)
	void TraceOtherSocketAtSameTime(ABaseCharacter* Target);

	//이전 틱의 서로 다른 소켓과 현재 틱의 서로다른 소켓에 대해서 트레이스 합니다.
	UFUNCTION(BlueprintCallable)
	void TraceOtherSocketAtDifferentTime(ABaseCharacter* Target);

public:
	//피격 리스트를 초기화 합니다. 이로써 대상들이 다시 피격 가능해집니다.
	void ClearHitActors();

	//해당 대상이 피격당한 리스트에 있나 확인합니다.
	bool IsContainActor(AActor* Actor) const;

protected:
	//소켓의 이전 프레임의 위치를 기억하는 맵입니다.
	UPROPERTY()
	TMap<FName, FVector> LastKnownNiagaraSpawnSocketLocation;
	
	void ActivateNiagaraSpawnTrace(AActor* Target);
	UFUNCTION()
	void CreateSpawnNiagaraEffectTrace();
	void SpawnNiagaraEffect(const TArray<FHitResult>& Hits) const;
	void SpawnNiagaraEffect(const FHitResult& Hit) const;
	void UpdateLastNiagaraSpawnSocketLocation();
	
	virtual void ProcessEffect_Implementation(ABaseCharacter* Target, AActor* EffectBy,UAbilityBase* From) override;
	virtual void OnTaskTickEvent_Implementation(float DeltaTime) override;
	virtual void EndEffect_Implementation(ABaseCharacter* Target) override;

	void UpdateCueLocation(const FVector& NewLocation);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsAlreadyHit(AActor* Actor);

	
	/**
	 * Hit 대상에게 EffectedBy로 부터 넉다운을 적용하려면 호출하십시오.
	 * 무적인 대상에게는 적용되지 않습니다.
	 * @param Hit 
	 * @param EffectedBy 
	 * @param KnockdownTag 
	 * @param bRotationToHitPoint 
	 * @param bReset 이 값이 참인 경우, 이미 넉다운이 적용되어 있을 떄, 적용된 넉다운을 취소하고 새로 적용합니다.
	 */
	UFUNCTION(BlueprintCallable)
	void ApplyKnockDown(const FHitResult& Hit,class ABaseCharacter* EffectedBy, FGameplayTag KnockdownTag,bool bRotationToHitPoint, bool bReset);
};
