// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "00_Character/BaseCharacter.h"
#include "97_Interface/BoneTransformInterface.h"
#include "97_Interface/LockOnInterface.h"
#include "97_Interface/01_Monster/AIInterface.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Engine/DataAsset.h"
#include "BaseMonster.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMonster, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFinishSpawnAlly, class UBehaviorTreeComponent*, OwnerComp,
                                             EBTNodeResult::Type, Result);
//파라미터로 타겟을 기억하던 본인을 받습니다.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBlackboardTargetIsNotValid,class AAIController*,SelfController);
/**
 * 
 */

UENUM(BlueprintType)
enum class EMonsterState : uint8
{
	// 평화 상태
	Peaceful UMETA(DisplayName = "평화"),
	// 적을 인식하고, 전투로 진입한 상태
	Aggressive UMETA(DisplayName = "전투"),
	//적을 인식했지만 전투는 아닌 상태
	Beware UMETA(DisplayName = "경계"),
	//가드중인 상태
	Guard UMETA(DisplayName = "방어"),
	MAX
};

UENUM(BlueprintType)
enum class EMonsterType :uint8
{
	//야수
	BEAST,
	//언데드
	UNDEAD,
	//트롤
	TROLL,
	//골램
	GOLEM,
	//인간형
	HUMANOID
	//Humanoid
};


UCLASS(BlueprintType)
class SOULLIKE_API UMonsterDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	//몬스터를 구분짓는 유일한 태그입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MonsterTag;
	//몬스터 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText MonsterName;
	//AI에 사용할 행동트리
	UPROPERTY(EditAnywhere)
	class UBehaviorTree* BehaviorTree;
	UPROPERTY(EditAnywhere)
	bool bStartBehaviorTreeImmediately;
	//이 값이 참이면 플레이어가 화톳불에서 휴식할 때, 부활합니다.
	UPROPERTY(EditAnywhere)
	bool bRespawnWhenPlayerRest = true;
	UPROPERTY(EditAnywhere)
	EMonsterType MonsterType;

	//참이면 경직시스템을 사용합니다.
	UPROPERTY(EditAnywhere)
	bool bUseStun = false;
	//이 수치를 초과하면, 누적된 경직수치를 초기화 하고, 피격애니메이션을 강제 플레이합니다. 보스타입 몬스터들에게 경직을 발생시키기 좋습니다.
	UPROPERTY(EditAnywhere, Category="Stun", meta=(EditCondition = "bUseStun"))
	float MaxStunIntensity;
	UPROPERTY(EditAnywhere, Category="Stun", meta=(EditCondition = "bUseStun"))
	UAnimMontage* StunMontage;

	//시야 범위
	UPROPERTY(EditAnywhere, Category="Perception")
	float SightRadius = 1200.f;
	//시야범위에 들어와 인지하는 도중 이 거리 밖으로 빠져나가면 시야에서 사라진 것으로 봅니다.
	//덮어 쓸 컨피그
	UPROPERTY(EditAnywhere, Category="Perception")
	float LoseSightRadius = 1500.f;
	//정면에서 이 수치만큼을 시야라고 생각합니다.
	UPROPERTY(EditAnywhere, Category="Perception")
	float PeripheralVisionAngleDegrees = 60.f;
	//시야 퍼셉션에서 기억할 최대 시간입니다
	UPROPERTY(EditAnywhere, Category="Perception")
	float MaxAge = 5.f;
	UPROPERTY(EditAnywhere, Category="Perception")
	bool bForceCombatStateWhenPerceptedTarget = true;
	//레그돌용
	UPROPERTY(EditAnywhere)
	class UPhysicsAsset* RagdollPhysics;
};


/* 
 *	모든 몬스터의 기본 클래스
 *	IBossMonsterInterface를 상속받으면 보스몬스터 취급 합니다.
 *	MonsterDataAsset내부의 몬스터 태그를 꼭 설정해 주세요.
 */
UCLASS()
class SOULLIKE_API ABaseMonster : public ABaseCharacter, public ILockOnInterface,
                                  public IAIInterface
{
	GENERATED_BODY()

	friend class AMonsterAIController;
	/**********************************************기본 상속 함수*********************************************************/
protected:
	ABaseMonster();

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaSeconds) override;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	void DrawPerceptionDebug();
#endif
	
	void DetachDroppedItem();
	UFUNCTION()
	void OnEndPlayEvent(AActor* Actor, EEndPlayReason::Type EndPlayReason);
	UFUNCTION()
	void OnDestroyedEvent(AActor* DestroyedActor);
	/**********************************************컴포넌트*********************************************************/
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class UWidgetComponent* HealthBarWidgetComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class UItemDropComponent* ItemDropComponent;
	//보스몬스터 전용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class UAudioComponent* MusicComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class UNavigationInvokerComponent* NavigationInvokerComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class UUROComponent* UROComponent;


	UPROPERTY(Transient)
	TObjectPtr<class AItemActor> DroppedItem;
	

public:

	FORCEINLINE class UItemDropComponent* GetItemDropComponent() const {return ItemDropComponent;}
	
	class UWidgetComponent* GetHealthBarWidgetComponent() const { return HealthBarWidgetComponent; }

protected:
	FTimerHandle HealthBarVisibleTimerHandle;

	UFUNCTION()
	void UpdateHealthBar(float Value, float MaxValue);


	//레그돌에서 복구할 때 사용할 컴포넌트 기본 트렌스폼
	FTransform DefaultMeshTr;
	FTransform DefaultHealthBarTr;
	//액터가 배치된 위치 초기값.
	FTransform InitTransform;

public:
	void EnableRagdoll() const;
	void StopAITree();
	void RunDeactivateTimer();

	virtual void Activate();
	virtual void SetCharacterState(ECharacterState NewState) override;
	
protected:
	//이 몬스터를 비활성화 합니다. AI를 멈추고, 콜리전 반응을 지우고, 숨깁니다.
	virtual void Deactivate();
	void DeadPresetting();
	void RunAITree() const;
	void RestoreFromRagdoll();
	
	//레그돌 활성화로 망가진 컴포넌트 상속관계와 위치를 되돌립니다. 필요하다면 덮어쓰세요.
	virtual void RestoreComponentAttachment() const;
	/**********************************************락온*********************************************************/
public:
	virtual bool IsLockOnAble_Implementation() override { return bLockOnAble; }
	virtual void SetLockOnAble_Implementation(bool newVal) override { bLockOnAble = newVal; }
	/**********************************************아군 스폰기능*********************************************************/
protected:
	//아군 스폰이 끝난 후 호출하세요.
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnFinishSpawnAlly OnFinishSpawnAllyEvent;

	//몬스터 사망시 n초후에 몬스터를 제거하는 타이머 핸들
	UPROPERTY()
	FTimerHandle DeadTimerHandle;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, DisplayName= "SpawnAlly")
	void K2_SpawnAlly(int32 SpawnCount);
	virtual void K2_SpawnAlly_Implementation(int32 SpawnCount);

public:
	void SpawnAlly(int32 SpawnCount, const FOnFinishSpawnAlly& OnFinishSpawnAlly);


	void ResetMonster(const FTransform& DefaultTr);

protected:
	//이 몬스터가 락온 가능한 상태면 참입니다.
	UPROPERTY(EditAnywhere)
	bool bLockOnAble = true;

	/**********************************************상태*********************************************************/
	//적을 인식했는지 저장하는 변수입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient)
	EMonsterState MonsterState;
	//몬스터 기본정보를 저장하는 데이터 에셋입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="Default")
	class UMonsterDataAsset* MonsterDataAsset;
	//이 값이 참이면, 비헤이비어 트리 즉시 시작 옵션을 덮어씁니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="Default")
	bool bOverrideStartBehaviorTreeImmediately = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="Default",meta=(EditCondition="bOverrideStartBehaviorTreeImmediately"))
	bool bStartBehaviorTreeImmediately  = false;
	//이 값이 참이라면, 비헤이비어 트리가 실행되기 전에 타겟이 될 만한 대상을 발견해도 비헤이비어 트리를 실행하지 않습니다. 피해에 대해서는 반응합니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="Default",meta=(EditCondition="!bStartBehaviorTreeImmediately"))
	bool bIgnorePerceptionTargetBeforeRunBehaviorTree = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="Default",meta=(EditCondition="bStartBehaviorTreeImmediately"))
	bool bFollowLeader = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="Default",meta=(EditCondition="bFollowLeader"))
	class AActor* Leader;
	
public:
	bool UseLeader() const {return bFollowLeader;}
	AActor* GetLeader() const {return Leader;}
	
	//이 수치가 DataAsset에 저장된 수치보다 높아지면 이 수치를 0으로 초기화 하고, 강제로 피격 애니메이션을 재생하도록 합니다.
	UPROPERTY(EditAnywhere, Transient)
	float StunIntensity;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EMonsterType GetMonsterType() { return MonsterDataAsset->MonsterType; }

	UFUNCTION(BlueprintCallable)
	void SetMonsterState(EMonsterState NewState);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EMonsterState GetMonsterState() { return MonsterState; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool ShouldForceCombatState();

protected:
	void IncreaseStunIntensity(const FAttributeEffect& Effect,
	                           UAbilityEffectAdditionalInformation* AdditionalInformation);
	UFUNCTION()
	void OnStunMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	//몬스터 사망시 숨김처리되는 시간입니다.
	UPROPERTY(EditAnywhere)
	float DeactivateTime = 30.f;
	/**********************************************리스폰*********************************************************/
protected:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsRespawnWhenPlayerRest() { return MonsterDataAsset->bRespawnWhenPlayerRest; }


	/**********************************************AI*********************************************************/

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsStartBehaviorTreeImmediately() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsIgnorePerceptionTargetBeforeRunBehaviorTree() const {return bIgnorePerceptionTargetBeforeRunBehaviorTree;}
	
public:
	//기억하던 블랙보드의 타겟이 유효하지 않게 되면 호출됩니다.
	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FOnBlackboardTargetIsNotValid OnBlackboardTargetIsNotValid;


	UFUNCTION()	
	void OnBlackboardTargetIsNotValidEvent(class AAIController* SelfController);

	/**********************************************기본정보*********************************************************/
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FText GetMonsterName() { return MonsterDataAsset->MonsterName; }

	const FGameplayTag& GetMonsterTag() const;
	/**********************************************애니메이션*********************************************************/
	
	virtual void OnTriggerHitAnimationEnterEvent(ABaseCharacter* DamagedCharacter, AActor* HitBy) override;
	
	/**********************************************사망처리*********************************************************/	
protected:

	UPROPERTY()
	class UAnimMontage* SelectedDeadMontageToPlay;
	
	virtual void OnDeadEvent(AActor* Who, AActor* DeadBy) override;
	UFUNCTION()
	void OnDeadBossEvent(AActor* Who, AActor* DeadBy);

public:
	virtual void PlayDeadAnimationSequence() override;
	virtual void PlayDeadAnimationMontage() override;

	UFUNCTION()
	virtual void OnDeadMontageEndedEvent(UAnimMontage* Montage, bool bInterrupted);

	/**********************************************어빌리티 이팩트*********************************************************/
protected:
	virtual void OnRemoveAttributeEffectAdditionalInformationEvent_Implementation(const FAttributeEffect& Effect,
		UAbilityEffectAdditionalInformation*
		AdditionalInformation, float DeltaTime = 1) override;

	//레벨에서 호출하면 렌덤하게 회전을 틉니다.
	UFUNCTION(BlueprintCallable, CallInEditor)
	void SetRandomRotationYaw();

	virtual void
	TriggerHitAnimation_Implementation(UAbilityEffectAdditionalInformation* AdditionalInformation) override;


	/**********************************************사운드*********************************************************/

public:
	//보스 인터페이스를 상속중이라면, 음악을 재생합니다.
	UFUNCTION(BlueprintCallable)
	void PlayMusic();
	/**
	 * AdjustVolumeDuration에 걸쳐서 볼륨을 0으로 만든 후, 컴포넌트를 파괴합니다.
	 * @param AdjustVolumeDuration 
	 */
	UFUNCTION(BlueprintCallable)
	void StopMusic(float AdjustVolumeDuration);

	
};
