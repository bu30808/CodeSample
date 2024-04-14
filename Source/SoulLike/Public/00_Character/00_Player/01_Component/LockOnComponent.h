// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/ActorComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LockOnComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLockOnComponent, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLockOnTraceEnd);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateLockOnTargetWidget, FVector, NewLocation);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLockOnTargetChangeNext);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLockOnTargetChangePre);

/**
 * 락온기능을 담은 컴포넌트입니다.
 * 카메라로부터 박스트레이스를 그려 감지합니다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULLIKE_API ULockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULockOnComponent();

protected:
	UPROPERTY()
	TWeakObjectPtr<class APlayerCharacter> Owner;
	UPROPERTY()
	TWeakObjectPtr<class UCameraComponent> Camera;
	UPROPERTY()
	TWeakObjectPtr<class USpringArmComponent> SpringArm;


	/**
	 * 트레이스가 그려질 거리입니다.
	 */
	UPROPERTY(EditAnywhere, Category="Options")
	float Distance = 1200;


	/**
	 * 트레이스로 사용되는 박스의 크기
	 */
	UPROPERTY(EditAnywhere, Category="Options")
	FVector BoxHalfSize;
	UPROPERTY(EditAnywhere, Category="Options")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	UPROPERTY(EditAnywhere, Category="Options")
	bool bTraceComplex;
	UPROPERTY(EditAnywhere, Category="Options")
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType;
	UPROPERTY(EditAnywhere, Category="Options")
	FLinearColor TraceColor;
	UPROPERTY(EditAnywhere, Category="Options")
	float DrawTime = 5.f;
	/**
	 * 락온 타겟을 바라볼 때,캐릭터 회전 보간 속도
	 */
	UPROPERTY(EditAnywhere, Category="Options")
	float CharaterRotationLerpSpeed = 20.f;
	/**
	 * 락온 타겟을 바라볼 때,카메라 회전 보간 속도
	 * 너무 빠른 경우 뚝뚝 끊기는 느낌이 들 수 있습니다.
	 */
	UPROPERTY(EditAnywhere, Category="Options")
	float CameraRotationLerpSpeed = 2.5f;


	/**
	 * 이 거리 이상 멀어지면 락온 대상에서 제외시킵니다.
	 */
	UPROPERTY(EditAnywhere, Category="Options")
	float RemoveDistance = 2000.f;


	/**
	 * 트레이스 걸린 대상들이 저장됩니다.
	 */
	UPROPERTY(VisibleAnywhere)
	TArray<FHitResult> HitActors;


	/**
	 * 락온 배열 인덱스 번호
	 */
	int32 CurIndex;

	/**
	 * 장애물에 일정시간이상 가려서 보이지 않을떄 타겟 리스트에서 제거하기 위한 타이머 핸들 맵
	 */
	UPROPERTY()
	TMap<AActor*, FTimerHandle> RemoveTimerHandles;

	/**
	 * 지금 락온으로 잡힌 타겟입니다.
	 */
	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<AActor> LookTarget;
	//이전에 락온으로 잡던 타겟입니다.
	TArray<TWeakObjectPtr<AActor>> PreLookTargetStack;

	UPROPERTY()
	TObjectPtr<class UWidgetComponent> LockOnWidgetComponent;

	/**
	 * 스프림 암의 기본값을 저장할 변수입니다.
	 */
	FVector SpringArmOriginalLocation;
	/**
	* 스프림 암의 기본값을 저장할 변수입니다.
	*/
	float OriginalArmLength;

	/**
	 * 이 값이 커지면 타겟과의 거리가 멀어질 때, 스프링암의 Z축이 빨리 변합니다.
	 */
	UPROPERTY(EditAnywhere)
	float SpringArmZFactor = 0.5f;
	/**
	 * 이 값이 커지면 타겟과의 거리가 멀어질때 스프링 암이 더 빨리 멀어집니다.
	 */
	UPROPERTY(EditAnywhere)
	float SpringArmLengthFactor = 1.f;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> LockOnWidgetClass;

	/**
	 * 이 값이 참이면 락온시 타겟을 바라보게 됩니다.
	 * 회피같이 타겟을 바라보면 안 될 경우에 사용합니다.
	 */
	UPROPERTY(BlueprintReadWrite)
	bool bEnableLockOnRotation = true;


	// Called when the game starts
	virtual void BeginPlay() override;

public:
	void SetEnableLockOnRotation(bool var) { bEnableLockOnRotation = var; }
	bool GetEnableLockOnRotation() const { return bEnableLockOnRotation; }


	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;


	/**
	 * 락온 트레이스를 그리기 시작합니다.
	 */
	UFUNCTION(BlueprintCallable)
	void Start();

	/**
	 * 트레이스를 중단합니다.
	 */
	UFUNCTION(BlueprintCallable)
	void End();

	/**
	 * 락온 트레이스가 종료되었을 때, 호출되는 이벤트입니다.
	 */
	UPROPERTY(BlueprintAssignable)
	FOnLockOnTraceEnd OnLockOnEnd;

	/**
	 * 타겟을 표시하는 위젯의 위치를 업데이트하는 이벤트입니다.
	 */
	UPROPERTY(BlueprintAssignable)
	FOnUpdateLockOnTargetWidget OnUpdateLockOnTargetWidget;

	/**
	 * 락온 타겟을 변경하려 할 때 호출되는 이벤트입니다
	 */
	UPROPERTY(BlueprintAssignable)
	FOnLockOnTargetChangeNext OnLockOnTargetChangeNext;
	/**
	 * 락온 타겟을 변경하려 할 때 호출되는 이벤트입니다
	 */
	UPROPERTY(BlueprintAssignable)
	FOnLockOnTargetChangePre OnLockOnTargetChangePre;

protected:
	//가장 가깝지만, 현재 락온중이지 않은 타겟을 가져옵니다.
	AActor* GetNonCurrentTarget();
	//이전 타겟을 가져오되, 거리를 벗어나지 않은 대상을 가져옵니다.
	AActor* GetPreTargetNotOverRemoveDistance();
	/**
	 * 어빌리티 내부에서 락온 타겟을 변경하기 위해 이 함수가 바인드됩니다.
	 */
	UFUNCTION(BlueprintCallable)
	void OnChangedNextTargetEvent();
	/**
	 * 어빌리티 내부에서 락온 타겟을 변경하기 위해 이 함수가 바인드됩니다.
	 */
	UFUNCTION(BlueprintCallable)
	void OnChangedPreTargetEvent();

public:
	UFUNCTION(BlueprintCallable)
	void BindChangeTargetEvent();
	UFUNCTION(BlueprintCallable)
	void UnBindChangeTargetEvent();

protected:
	/**
	 * 플레이어가 이동하려고 입력한 방향을 구합니다.
	 * @return 방향백터
	 */
	UFUNCTION(BlueprintCallable)
	FVector GetPlayerMoveInputDirection();

	/**
	 * 플레이아거 이동하려고 입력한 방향을 바라보기 위해 회전해야 할 값을 구합니다.
	 * @return 회전값
	 */
	UFUNCTION(BlueprintCallable)
	FRotator GetPlayerInputDirRotation();

private:
	void CheckTraceHitActorIsHide(TArray<FHitResult>& Hits);
	void CreateTrace();
	void AddTraceHitActors(const TArray<FHitResult>& Hits);
	void LookAtTarget(float DeltaTime);
	void SetLockOnWidgetPosToLookTarget();
	void SortLockOnListByDistance();

	AActor* GetNearestTarget();


	/**
	 * 너무 멀리있는 대상을 락온 리스트에서 제거합니다.
	 */
	void RemoveDistantTargetsFromLockOnList();

	/**
	 * 너무 오래 숨어있는 대상을 락온 리스트에서 제거합니다.
	 */
	void RemoveHideTargetsFromLockOnList();


	void RemoveTargetFromLockOnListByTimer(ACharacter* Character);

	//락온 인터페이스를 상속받는 대상들 중, 락온불가능 판정인 대상을 리스트에서 제거합니다.
	void RemoveCannotLockOnTargetOnList();
};
