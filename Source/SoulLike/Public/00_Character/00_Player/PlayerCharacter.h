// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "00_Character/BaseCharacter.h"
#include "02_Ability/AbilityCue.h"
#include "03_Widget/MainWidget.h"
#include "GameFramework/SpringArmComponent.h"
#include "PlayerCharacter.generated.h"

#define ECC_Monster ECC_GameTraceChannel1
#define LockOnTag FGameplayTag::RequestGameplayTag("Common.Active.LockOn")
#define RunTag FGameplayTag::RequestGameplayTag("Common.Active.Run")
#define ExecuteTag FGameplayTag::RequestGameplayTag("Common.Active.Execute")

enum class EOrbMatrixSlotType : uint8;

UCLASS(BlueprintType)
class SOULLIKE_API UInputDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PickUpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LockOnAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeNextLockOnTargetAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangePreLockOnTargetAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* NormalAttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* StrongAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MenuAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShowCursorAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ConsumeQuickSlotChangeAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AbilityQuickSlotChangeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ConsumeQuickSlotAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AbilityQuickSlotAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CharacterInfoAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* EquipmentAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InventoryAction;

};

UENUM(BlueprintType)
enum class EPlayerCharacterState : uint8
{
	Peaceful,
	Combat,
	// ... 다른 상태들 추가
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUseQuickSlot, class UInputAction*, InputAction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangePlayerState, EPlayerCharacterState, State);
UCLASS()
class SOULLIKE_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
		
	/**********************************************기본 상속 함수*********************************************************/
public:
	// Sets default values for this character's properties
	APlayerCharacter();
protected:
	
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	/**********************************************컴포넌트*********************************************************/
protected:
	UPROPERTY(VisibleAnywhere, Category=Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	UPROPERTY(VisibleAnywhere, Category=Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SceneCaptureCameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta = (AllowPrivateAccess = "true"))
	class USceneCaptureComponent2D* SceneCaptureComponent2D;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class ULockOnComponent* LockOnComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAbilityTalentComponent* AbilityTalentComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* ExecutionWidgetComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class ULadderMovementComponent* LadderMovementComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UJumpMovementComponent* JumpMovementComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UGFurComponent* FurComponent;
	UPROPERTY()
	class UMaterialInterface* OriginalFurMaterial;

	//씬캡쳐2d 랜더용
	UPROPERTY()
	TObjectPtr<class UTextureRenderTarget2D> RenderTextureTarget;
	

public:
	FORCEINLINE class UJumpMovementComponent* GetJumpMovementComponent() const {return JumpMovementComponent;};
	FORCEINLINE class ULadderMovementComponent* GetLadderMovementComponent() const {return LadderMovementComponent;};
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class ULockOnComponent* GetLockOnComponent() const { return LockOnComponent; }
	FORCEINLINE class USpringArmComponent* GetSceneCaptureCameraBoom() const { return SceneCaptureCameraBoom; }
	FORCEINLINE class USceneCaptureComponent2D* GetSceneCaptureComponent2DComponent() const
	{
		return SceneCaptureComponent2D;
	}

	FORCEINLINE class UAbilityTalentComponent* GetAbilityTalentComponent() const { return AbilityTalentComponent; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	class UInputHandlerComponent* GetInputHandlerComponent();

	UPROPERTY()
	FTransform SceneCaptureBoomTr;
	UPROPERTY()
	FTransform SceneCaptureTr;
	
	void ResetSceneCaptureCamera() const;
	void ShowRender(bool bIsVisible);
	/**********************************************입력*********************************************************/
protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UInputDataAsset* InputDataAsset;

	//이동키를 누르면 참입니다.
	UPROPERTY(Transient,BlueprintReadOnly)
	bool bPressMove;
public:
	UFUNCTION(BlueprintCallable)
	void SetDefaultMappingContext();

	class UEnhancedInputLocalPlayerSubsystem* GetLocalInputSystem() const;
	
	bool GetPressMove() const {return bPressMove;}
	/**********************************************움직임 입력*********************************************************/
protected:
	void PressMove();
	void ReleaseMove();
	
	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	/**********************************************시점 입력*********************************************************/
	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	virtual void AddControllerYawInput(float Val) override;
	virtual void AddControllerPitchInput(float Val) override;
public:
	//좌 우 입력에 대한 입력값을 방향백터로 변경한 변수입니다.
	UPROPERTY(BlueprintReadOnly)
	FVector MovementInputVector;
	/**********************************************액션 입력 처리*********************************************************/
protected:
	//질주키를 눌렀는가?
	UPROPERTY(Transient)
	bool bPressSprint;
	//질주키를 누른 시간을 저장할 변수
	UPROPERTY(Transient)
	float SprintTimeKeyDown;

	UPROPERTY()
	FTimerHandle SprintTimerHandle;
	
	void PressSprint();
	void ReleaseSprint();
	
	void NormalAttack();
	void StrongAttack();
	void Dodge();
	void LockOn();
	void Run();
	void StopRun();
	void PreLockOnTarget();
	void NextLockOnTarget();
	void OpenNavigationOrClosePopUp();
	void PickUp();

	void ChangeConsumeQuickSlot();
	void ChangeAbilityQuickSlot();
	
	void Interaction();
	void ShowMouseCursor();

	void DoExecute(class ABaseMonster* ExecuteTarget);

	void ClosePopUp();

	/*void OpenCharacterInfo();
	void OpenEquipment();
	void OpenInventory();*/
	
	/**********************************************오버렙 처리*********************************************************/
private:
	UPROPERTY()
	TWeakObjectPtr<AActor> OverlappedOtherActor;

protected:
	UFUNCTION()
	void OnActorBeginOverlapEvent(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void OnActorEndOverlapEvent(AActor* OverlappedActor, AActor* OtherActor);
	


	/**********************************************퀵슬롯 이벤트*********************************************************/
public:
	/*UPROPERTY()
	FOnUseQuickSlot OnUseConsumeQuickSlot1;
	UPROPERTY()
	FOnUseQuickSlot OnUseConsumeQuickSlot2;
	UPROPERTY()
	FOnUseQuickSlot OnUseConsumeQuickSlot3;
	UPROPERTY()
	FOnUseQuickSlot OnUseConsumeQuickSlot4;
	UPROPERTY()
	FOnUseQuickSlot OnUseConsumeQuickSlot5;

	TArray<FOnUseQuickSlot*> ConsumeQuickSlotEvents;


	UPROPERTY()
	FOnUseQuickSlot OnUseAbilityQuickSlot1;
	UPROPERTY()
	FOnUseQuickSlot OnUseAbilityQuickSlot2;
	UPROPERTY()
	FOnUseQuickSlot OnUseAbilityQuickSlot3;
	UPROPERTY()
	FOnUseQuickSlot OnUseAbilityQuickSlot4;
	UPROPERTY()
	FOnUseQuickSlot OnUseAbilityQuickSlot5;

	TArray<FOnUseQuickSlot*> AbilityQuickSlotEvents;*/
	/**********************************************장비*********************************************************/
protected:
	/*
	 * 시작하면 장착시킬 기본무기
	 */
	UPROPERTY(EditAnywhere, Category=Default)
	TSubclassOf<class AEquipmentItemActor_Spirit> DefaultSpirit;
	UPROPERTY(EditAnywhere, Category=Default)
	TSubclassOf<class AEquipmentItemActor_OrbCore> DefaultCore;
	/*
		 * 신력구슬 텝에서 사용할 렌더타겟을 그리는 액터입니다. 이 액터가 레벨에 없으면 뒷배경에 아무것도 안보임
		 */
	UPROPERTY(EditAnywhere, Category=Default)
	TSubclassOf<class AOrbBackgroundActor> OrbBackgroundActorObject;
public:
	UPROPERTY()
	AOrbBackgroundActor* OrbBackgroundActor;
	
	/**********************************************레벨업*********************************************************/
protected:
	/**
	 * 레벨업하면 사용할 큐 정보
	 */
	UPROPERTY(EditAnywhere, Category=Default)
	FAbilityCueInformation LevelUpCueInfo;

	UFUNCTION()
	void OnLevelUpEvent();

	/**********************************************애니메이션*********************************************************/
protected:
	//아무 무기도 장착하지 않을 때 상용할 애니메이션 인스턴스입니다. 사용될일이 없는게 정상입니다.
	UPROPERTY(EditAnywhere, Category=Default)
	TSubclassOf<class UAnimInstance> DefaultAnimationInstance;

	UPROPERTY(EditAnywhere, Category=Default)
	class UAnimMontage* TeleportMontage;
	
	
	virtual void OnUpdateDeadDissolveTimeLine(float Value) override;
	virtual void OnFinishDissolveTimeLine() override;
public:
	TSubclassOf<class UAnimInstance> GetDefaultAnimInstance() const { return DefaultAnimationInstance; }
protected:
	virtual void OnTriggerHitAnimationEnterEvent(class ABaseCharacter* DamagedCharacter, AActor* HitBy) override;
	virtual void OnTriggerHitAnimationExitEvent(class ABaseCharacter* DamagedCharacter, AActor* HitBy) override;
	/**********************************************어빌리티*********************************************************/
protected:
	/**
	 * 기본 무기를 장착합니다
	 */
	void EquipDefaultWeapon();
	/**
	 * 기본 코어를 장착합니다.
	 */
	void EquipDefaultCore();

public:
	//다운당했을때, 특성이 부여되어 있으면, 회피로 연계합니다.
	UFUNCTION(BlueprintCallable)
	void OnWaitKnockDownDodgeKeyPress(const FKeyPressedInfo& ActionInfo);
protected:
	//회피 어빌리티를 강제종료합니다.
	void ForceEndDodge();
	
	/**********************************************위젯*********************************************************/
protected:
	UPROPERTY()
	TWeakObjectPtr<class UMainWidget> MainWidget;

	void SetMainWidget();
public:
	UMainWidget* GetMainWidget() const {return MainWidget.Get();}
	/**********************************************인터렉션*********************************************************/
protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UKeyWidget> KeyWidgetObject;

	//아이템 획득 표시용 위젯 컴포넌트, 이 컴포넌트가 필요하다면 GetGetItemWidget 함수로 가져오세요. 
	UPROPERTY()
	class UWidgetComponent* InteractionWidgetComponent;

	UPROPERTY()
	TWeakObjectPtr<AActor> InteractableActor;

	/**
	 * 상호작용 가능한 NPC를 틱마다 찾습니다.
	 */
	void FindInteractableNPC();
	bool FindLadder();
public:
	/**
	 * 상호작용에 필요한 키와 액션을 표시해줍니다.
	 * @param Target 상호작용할 대상
	 * @param Action 상호작용할 액션 정보
	 * @param ActionName 키 이름
	 */
	UFUNCTION(BlueprintCallable)
	class UWidgetComponent* ShowInteractionWidget(const AActor* Target, const UInputAction* Action, const FString& ActionName);
	UFUNCTION(BlueprintCallable)
	//보여주진 않고 그냥 가져만 옵니다. Visiblity를 켜야 보입니다.
	class UWidgetComponent* GetInteractionWidget(const AActor* Target, const UInputAction* Action, const FString& ActionName);
	UFUNCTION(BlueprintCallable)
	void HideInteractionWidget();

	class UInputDataAsset* GetInputDataAsset() const {return InputDataAsset;}
protected:
	//키를 표시해주는 위젯 컴포넌트를 가져옵니다.
	class UWidgetComponent* GetPressKeyWidget(FName KeyName=NAME_None, const FString& ActionName= "");

	virtual void ChangeMovementState(EMovementState Type, float Multiplier) override;
	/**********************************************입력방향*********************************************************/
public:
	/**
	 * 플레이어의 움직인 방향을 구합니다.
	 * @param bControllerDirection 이 값이 참이면, 컨트롤러의 입력 방향만 고려합니다.
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EDirection GetPlayerMoveDirection(bool bControllerDirection = true);

	/**********************************************처형*********************************************************/
protected:
	//처형을 시도할 수 있는 최대거리입니다.
	UPROPERTY(EditAnywhere)
	float DistanceToExecution = 100.f;

	//일반공격 키를 누르면 처형할 대상입니다.
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<class ABaseMonster> ExecutableMonster;

	//플레이어 전방에 몬스터가 있고, 해당 몬스터가 처형 가능한 몬스터인지 확인합니다.
	void CheckExecutionMonster();
	//플레이어 전방의 처형 가능한 몬스터를 찾아 리턴합니다.
	class ABaseMonster* GetPlayerForwardMonster(FVector& HitLocation);
	//해당 몬스터가 처형 가능한 상태인지 확인합니다.
	bool IsExecutable(class ABaseMonster* Monster, const FVector& HitLocation);
	//처형 아이콘을 해당 몬스터에게 표시합니다.
	void ShowExecutionIcon(class ABaseMonster* Monster);
	void HideExecutionIcon();

public:
	ABaseCharacter* GetExecuteMonster();

protected:
	UPROPERTY(EditAnywhere)
	FGameplayTag CharacterTag;
public:
	const FGameplayTag& GetCharacterTag() const {return CharacterTag;}

	/**********************************************플레이어 상태 정의*********************************************************/
private:
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (AllowPrivateAccess = "true"),Transient)
	EPlayerCharacterState CurrentState = EPlayerCharacterState::Peaceful;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float PeaceCheckTime = 15.f;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float CombatCheckTraceRadius = 1500.f;

	UPROPERTY()
	FTimerHandle PeaceTimerHandle;
public:

	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FOnChangePlayerState OnChangePlayerState;
	
	//주기적으로 주변에 몬스터가 있는지 확인해서 평화상태로 되돌리는 기능을 할 함수입니다.
	void CheckAroundExistMonster();
	UFUNCTION(BlueprintCallable)
	void SetPlayerState(EPlayerCharacterState NewState);

	UFUNCTION(BlueprintCallable,BlueprintPure)
	bool IsPeaceState() const {return CurrentState == EPlayerCharacterState::Peaceful;}

	//이동속도 어트리뷰트가 변경되면 호출됩니다.
	UFUNCTION()
	void OnChangedMoveSpeedAttributeEvent();

	virtual void OnDeadEvent(AActor* Who, AActor* DeadBy) override;
private:
	//이동할 다른 화톳불이 있는 레벨 이름입니다.
	UPROPERTY()
	FName OtherBonfireLevelName;
public:
	//다른 화톳불로 이동하는 함수입니다.
	void TeleportToOtherBonfire(FName LevelName);
	UFUNCTION()
	void OnEndPlayTeleportMontage(UAnimMontage* Montage, bool bInterrupted);


	virtual void ChangeStatusEffectMaterial(EStatusEffect EffectType) override;
	virtual void RestoreStatusEffectMaterial() override;
};
