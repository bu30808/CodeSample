// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "02_Ability/AbilityCue.h"
#include "02_Ability/AbilityEffect_Linetrace.h"
#include "97_Interface/LinetraceAbilityEffectInterface.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

#define ECC_Player ECC_GameTraceChannel5
#define ECC_Monster ECC_GameTraceChannel1

enum class EStatusEffect : uint8;
DECLARE_LOG_CATEGORY_EXTERN(LogCharacter, Log, All);


UCLASS(Blueprintable)
class UFallingDamageData : public UObject
{
	GENERATED_BODY()

public:
	//최대체력비례해서 줄 피해 퍼센트
	UPROPERTY(BlueprintReadWrite)
	float FallingDamageRatio = 0;
};




USTRUCT(BlueprintType)
struct FIgnoreInputHandler
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FGameplayTag AccruedTag;
	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<AActor> AccruedActor = nullptr;


	FIgnoreInputHandler()
	{
	}

	FIgnoreInputHandler(AActor* Actor, FGameplayTag Tag): AccruedTag(Tag), AccruedActor(Actor)
	{
	}

	FORCEINLINE bool operator==(const FIgnoreInputHandler& Other) const
	{
		return AccruedTag.MatchesTagExact(Other.AccruedTag) && AccruedActor == Other.AccruedActor;
	}

	FORCEINLINE friend uint32 GetTypeHash(const FIgnoreInputHandler& IgnoreInfo)
	{
		return HashCombine(GetTypeHash(IgnoreInfo.AccruedTag), GetTypeHash(IgnoreInfo.AccruedActor));
	}
};

//애니메이션 및 각종 설정에 사용됨.
UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Walk,
	Run
};

UENUM(BlueprintType)
enum class ETeam : uint8
{
	Player UMETA(DisplayName = "Player"),
	Monster UMETA(DisplayName = "Monster"),
	//아래는 특수한 경우에만 사용하세요.
	TeamA,
	TeamB,
	TeamC
};

UENUM(BlueprintType)
enum class EDirection : uint8
{
	Front,
	Right,
	Left,
	Back,
	FrontRight,
	FrontLeft,
	BackRight,
	BackLeft,
	Up,
	Down,
	MAX
};

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	//일반
	NORMAL,
	//피격당함
	HIT,
	//사망
	DEAD,
	//스턴
	STUN,
	//뒤잡기 당하는 중
	EXCUTE,
	MAX,
};

/*
 * 모든 캐릭터의 기본이 되는 클래스입니다.
 */

//캐릭터 사망 처리
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDead, class AActor*, Who, class AActor*, DeadBy);


//델리게이트 선언시 맵 타입을 전달하면 생기는 오류를 회피하는 방법입니다.
using FIgnoreMoveInputMap = TMap<FIgnoreInputHandler, uint8>;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTriggerIgnoreMoveInput, const FIgnoreMoveInputMap&);
/*DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamagedEvent, const FAttributeEffect& ,Effect, class UAbilityEffectAdditionalInformation* ,AdditionalInformation);*/

UCLASS()
class SOULLIKE_API ABaseCharacter : public ACharacter, public ILinetraceAbilityEffectInterface
{
	GENERATED_BODY()

	friend class UBaseAnimInstance;

	/**************************상속받은 기본 함수*************************/
public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void CheckFallDeath();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	/**************************컴포넌트*************************/
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UAttributeComponent* AttributeComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UAbilityComponent* AbilityComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UFootStepComponent* FootStepComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UInventoryComponent* InventoryComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UAnimationHelperComponent* AnimationHelperComponent;
	UPROPERTY(VisibleAnywhere, Category = "Timeline")
	class UTimelineComponent* DeadDissolveTimeLineComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAbilityTalentComponent* AbilityTalentComponent;


	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UNavigationInvokerComponent* NavigationInvokerComponent;*/

public:
	FORCEINLINE class UAbilityComponent* GetAbilityComponent() const { return AbilityComponent; }
	FORCEINLINE UAttributeComponent* GetAttributeComponent() const { return AttributeComponent; }
	FORCEINLINE class UFootStepComponent* GetFootStepComponent() const { return FootStepComponent; }
	FORCEINLINE class UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	FORCEINLINE class UAnimationHelperComponent* GetAnimationHelperComponent() const{ return AnimationHelperComponent; }
	FORCEINLINE class UTimelineComponent* GetDeadDissolveTimeLineComponent() const{ return DeadDissolveTimeLineComponent; }
	FORCEINLINE class UAbilityTalentComponent* GetAbilityTalentComponent() const { return AbilityTalentComponent; }
	/**************************어빌리티*************************/
protected:
	/**
	* 시작하자마자 기본적으로 보유해야 할 어빌리티 목록입니다.
	*/
	UPROPERTY(EditAnywhere, Category=DefaultAbility)
	TArray<TSubclassOf<class UAbilityBase>> DefaultAbility;
	/**
	 * 시작하자마자 기본적으로 사용해야 할 어빌리티 태그입니다.
	 */
	UPROPERTY(EditAnywhere, Category=DefaultAbility)
	FGameplayTagContainer DefaultUseAbilityTags;


	/**
	 * 기본 어빌리티를 부여합니다.
	 */
	void GiveDefaultAbility();
	/**
	 * 등록한 기본 어빌리티를 전부 사용합니다.
	 */
	void ActivateDefaultAbility();

	/**************************어빌리티 큐*************************/
protected:
	//이 캐릭터가 피격당했을 때, 사용할 큐 정보가 있다면 사용하세요. 주로 피격 사운드에 사용 
	UPROPERTY(EditAnywhere)
	TArray<FAbilityCueInformation> GotHitCues;

public:
	//이 캐릭터가 피격당했을 때, 큐를 생성합니다.
	UFUNCTION(BlueprintCallable)
	void ShowGotHitCue(FVector HitLocation);
	/**************************어빌리티 이팩트*************************/
protected:
	//속성을 깎는 이팩트에 추가적인 정보가 전달되었을 떄, 사용됩니다.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRemoveAttributeEffectAdditionalInformationEvent(const FAttributeEffect& Effect,
														   class UAbilityEffectAdditionalInformation*
														   AdditionalInformation, float DeltaTime = 1);
	virtual void OnRemoveAttributeEffectAdditionalInformationEvent_Implementation(const FAttributeEffect& Effect,
		class UAbilityEffectAdditionalInformation*
		AdditionalInformation, float DeltaTime = 1);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsMighty();
	

	/**************************아이템*************************/
protected:
	/**
	 * 시작하자마자 기본적으로 지급할 아이템 목록입니다.
	 */
	UPROPERTY(EditAnywhere, Category=DefaultItem)
	TArray<TSubclassOf<class AItemActor>> DefaultItem;

	virtual void GiveDefaultItem();

	/**************************애니메이션*************************/
public:
	
protected:
	UFUNCTION()
	virtual void OnTriggerHitAnimationEnterEvent(class ABaseCharacter* DamagedCharacter, AActor* HitBy);
	UFUNCTION()
	virtual void OnTriggerHitAnimationExitEvent(class ABaseCharacter* DamagedCharacter, AActor* HitBy);

	UFUNCTION(BlueprintNativeEvent)
	void TriggerHitAnimation(UAbilityEffectAdditionalInformation* AdditionalInformation);
	virtual void TriggerHitAnimation_Implementation(UAbilityEffectAdditionalInformation* AdditionalInformation);

	//이 값이 참인 경우, 피격애니메이션을 건너뜁니다.
	bool ShouldSkipHitAnimation(float Damage, float SkipThreshold);

public:
	UFUNCTION()
	virtual void OnUpdateDeadDissolveTimeLine(float Value);
	UFUNCTION()
	virtual void OnFinishDissolveTimeLine();
	
protected:
	virtual void PlayDeadAnimationSequence();
	virtual void PlayDeadAnimationMontage(){ }
	
public:
	//캐릭터가 사망했을 때 할 행동들을 바인드하세요.
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnDead OnDead;

protected:
	//사망했을 떄, 무엇인가 하려면 덮어쓰세요.
	UFUNCTION()
	virtual void OnDeadEvent(AActor* Who, AActor* DeadBy);

	//사망시 디졸브를 위한 인스턴스
	UPROPERTY(Transient)
	TArray<class UMaterialInstanceDynamic*> BodyMaterialInstance;

	virtual void CreateBodyMaterialInstance();
	
public:
	//이 캐릭터가 사망상태인지 확인합니다.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsDead() { return CharacterState == ECharacterState::DEAD; }

protected:
	/**************************낙하 피해 및 사망*************************/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldCheckFallDeath = true;

	//이 거리 아래로 떨어지면 사망합니다.
	UPROPERTY(EditAnywhere)
	float FallDeathDistance = 3000.f;
	//낙하피해를 주는 이팩트
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UAbilityEffect> FallDamageEffectObject;


	UPROPERTY(Transient)
	bool bStartFallDamageProcess;
	UPROPERTY(Transient)
	FVector FallingStartLocation;


	/**************************상태정보*************************/
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Transient)
	ECharacterState CharacterState;

public:
	ECharacterState GetCharacterState() const { return CharacterState; }
	virtual void SetCharacterState(ECharacterState NewState);


	/**************************무브먼트*************************/
protected:
	//이동속도 변경을 위한 변수입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient)
	EMovementState MovementType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RunSpeedMultiplier = 1.3;

public:
	//이 함수로 무브먼트타입을 변경하면, 이동속도가 변경됩니다.
	UFUNCTION(BlueprintCallable)
	virtual void ChangeMovementState(EMovementState Type, float Multiplier = 1);

protected:
	/**************************콜리전*************************/

	//메시에 설정된 모든 피직스 에셋의 콜리전 반응 설정을 변경합니다.
	UFUNCTION(BlueprintCallable)
	void ChangeBodyCollisionEnabled(ECollisionEnabled::Type NewCollisionEnabled);


	/**************************입력 처리*************************/

public:
	FOnTriggerIgnoreMoveInput OnTriggerIgnoreMoveInput;

protected:
	UPROPERTY(VisibleAnywhere, Transient)
	TMap<FIgnoreInputHandler, uint8> IgnoreMoveInputStacks;
	UPROPERTY(VisibleAnywhere, Transient)
	TMap<FIgnoreInputHandler, uint8> IgnoreLookInputStacks;

public:
	/**
	 * 움직임 입력을 막으려면 사용하세요.
	 * @param bIgnore 참이면 막기 스택을 쌓습니다.
	 * @param AccruedBy 누구에 의해 막혔는지
	 * @param AccruedTag 어떤 태그에 의해 막혔는지
	 */
	UFUNCTION(BlueprintCallable)
	void SetIgnoreMoveInput(bool bIgnore, AActor* AccruedBy, FGameplayTag AccruedTag);
	/**
	 * 특정 대상과 태그를 만족하는 막힌 움직임을 전부 지웁니다.
	 * @param AccruedBy 누구에 의해 막혔는지
	 * @param AccruedTag 어떤 태그에 의해 막혔는지
	 */
	UFUNCTION(BlueprintCallable)
	void ClearMoveInputForGameplayTag(AActor* AccruedBy, FGameplayTag AccruedTag);


	/**
	 * 사용자의 카메라 회전을 막습니다.
	 * @param bIgnore 참이면 막기 스택을 쌓습니다.
	 * @param AccruedBy 누구에 의해 막혔는지
	 * @param AccruedTag 어떤 태그에 의해 막혔는지
	 */
	UFUNCTION(BlueprintCallable)
	void SetIgnoreLookInput(bool bIgnore, AActor* AccruedBy, FGameplayTag AccruedTag);
	/**
	 * 특정 대상과 태그를 만족하는 카메라 회전 금지 스텍을 제거합니다.
	 * @param AccruedBy 누구에 의해 막혔는지
	 * @param AccruedTag 어떤 태그에 의해 막혔는지
	 */
	UFUNCTION(BlueprintCallable)
	void ClearLookInputForGameplayTag(AActor* AccruedBy, FGameplayTag AccruedTag);

protected:

	/**************************상태이상*************************/
	UPROPERTY(EditAnywhere)
	TMap<EStatusEffect, UMaterialInterface*> StatusEffectMaterialMap;
	UPROPERTY()
	TArray<class UMaterialInterface*> OriginalMaterials;

public:
	//상태이상에 맞는 머터리얼로 변경합니다.
	UFUNCTION(BlueprintCallable)
	virtual void ChangeStatusEffectMaterial(EStatusEffect EffectType);
	//상태이상으로 변경된 머터리얼을 되돌립니다.
	UFUNCTION(BlueprintCallable)
	virtual void RestoreStatusEffectMaterial();
	
	UFUNCTION()
	void DeactivateMightyAbility();

	//모든 대상이 사망처리되었을때, 딱한번 재생되는 사운드 큐
	UPROPERTY(EditAnywhere)
	class USoundBase* DeadCue;
};
