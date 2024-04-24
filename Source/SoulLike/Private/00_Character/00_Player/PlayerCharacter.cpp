// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/00_Player/PlayerCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blueprint/UserWidget.h"
#include "Logging/StructuredLog.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "KismetAnimationLibrary.h"
#include "00_Character/00_Player/SoulTomb.h"

#include "Components/CapsuleComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/WidgetComponent.h"
#include "Camera/CameraComponent.h"

#include "00_Character/00_Player/00_Controller/UserController.h"
#include "00_Character/00_Player/00_Controller/00_Component/InputHandlerComponent.h"
#include "00_Character/00_Player/00_Controller/00_Component/WidgetManagerComponent.h"
#include "00_Character/00_Player/01_Component/AbilityTalentComponent.h"
#include "00_Character/00_Player/01_Component/LockOnComponent.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "00_Character/01_Component/InventoryComponent.h"

#include "92_Tools/Ladder.h"

#include "03_Widget/MainWidget.h"
#include "03_Widget/KeyWidget.h"

#include "04_Item/01_Equipment/00_Spirit/EquipmentItemActor_Spirit.h"
#include "04_Item/ItemActor.h"
#include "04_Item/01_Equipment/03_Orb/EquipmentItemActor_OrbCore.h"
#include "96_Library/AbilityHelperLibrary.h"
#include "96_Library/MathHelperLibrary.h"
#include "97_Interface/01_Monster/ExecutionInterface.h"
#include "98_GameInstance/SoulLikeInstance.h"


#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "FurComponent.h"
#include "MoviePlayer.h"
#include "00_Character/00_Player/OrbBackgroundActor.h"
#include "00_Character/00_Player/01_Component/JumpMovementComponent.h"
#include "00_Character/00_Player/01_Component/LadderMovementComponent.h"
#include "00_Character/00_Player/01_Component/TeleportBonfireComponent.h"
#include "91_Sky/DynamicSkyActor.h"
#include "92_Tools/WorldStreamingSourceActor.h"
#include "93_SaveGame/SoulLikeSaveGame.h"
#include "96_Library/DataLayerHelperLibrary.h"
#include "96_Library/SaveGameHelperLibrary.h"
#include "99_Subsystem/WidgetInteractionSubsystem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/TextureRenderTarget2D.h"



class UWidgetInteractionSubsystem;

#define ECC_INTERACTION ECC_GameTraceChannel7

#define LOCTEXT_NAMESPACE "PlayerCharacter"

#define INVINCIBILITY_TAG FGameplayTag::RequestGameplayTag("Common.Passive.Invincibility.Effect")

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetMesh()->SetCollisionProfileName("PlayerMesh");

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;


	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraRotationLagSpeed = 30.f;
	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	SceneCaptureCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SceneCaptureCameraBoom"));
	SceneCaptureCameraBoom->SetupAttachment(RootComponent);


	SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent"));
	SceneCaptureComponent2D->ProjectionType = ECameraProjectionMode::Type::Orthographic;
	SceneCaptureComponent2D->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	SceneCaptureComponent2D->SetupAttachment(SceneCaptureCameraBoom);

	SceneCaptureComponent2D->SetAutoActivate(false);

	static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> textureTarget(TEXT(
		"/Script/Engine.TextureRenderTarget2D'/Game/Blueprints/02_Widget/Menu/Render/RT_Character.RT_Character'"));
	if (textureTarget.Succeeded())
	{
		RenderTextureTarget = textureTarget.Object;
	}


	FurComponent = CreateDefaultSubobject<UGFurComponent>(TEXT("FurComponent"));
	FurComponent->SetupAttachment(GetMesh());

	ExecutionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ExecutionWidgetComponent"));
	ExecutionWidgetComponent->SetupAttachment(RootComponent);


	LockOnComponent = CreateDefaultSubobject<ULockOnComponent>(TEXT("LockOnComponent"));
	AbilityTalentComponent = CreateDefaultSubobject<UAbilityTalentComponent>(TEXT("AbilityTalentComponent"));

	LadderMovementComponent = CreateDefaultSubobject<ULadderMovementComponent>(TEXT("LadderMovementComponent"));
	JumpMovementComponent = CreateDefaultSubobject<UJumpMovementComponent>(TEXT("JumpMovementComponent"));
	TeleportBonfireComponent = CreateDefaultSubobject<UTeleportBonfireComponent>(TEXT("TeleportBonfireComponent"));
}

void APlayerCharacter::OnActorBeginOverlapEvent(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor != nullptr)
	{
		if (OtherActor->IsA<ASoulTomb>())
		{
			OverlappedOtherActor = OtherActor;
			const FText restoreText = NSLOCTEXT("PlayerCharacter", "ResotreSoulText", "복구");
			ShowInteractionWidget(OverlappedOtherActor.Get(), InputDataAsset->PickUpAction, restoreText);
			return;
		}


		if (OtherActor->IsA<AItemActor>())
		{
			OverlappedOtherActor = OtherActor;
			const FText getItemText = NSLOCTEXT("PlayerCharacter", "GetItemText", "획득");
			ShowInteractionWidget(OverlappedOtherActor.Get(), InputDataAsset->PickUpAction, getItemText);
		}
	}
}

void APlayerCharacter::OnActorEndOverlapEvent(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor != nullptr)
	{
		if (OverlappedOtherActor.IsValid())
		{
			if (OverlappedOtherActor.Get() == OtherActor)
			{
				if (InteractionWidgetComponent->IsValidLowLevel() && InteractionWidgetComponent->IsVisible())
				{
					InteractionWidgetComponent->SetVisibility(false);
				}

				OverlappedOtherActor = nullptr;
			}
		}
	}
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SetActorTickEnabled(false);

	OnActorBeginOverlap.AddUniqueDynamic(this, &APlayerCharacter::OnActorBeginOverlapEvent);
	OnActorEndOverlap.AddUniqueDynamic(this, &APlayerCharacter::OnActorEndOverlapEvent);

	OnEndPlay.AddUniqueDynamic(this, &APlayerCharacter::OnEndPlayEvent);


	AttributeComponent->OnChangedMoveSpeedAttribute.AddUniqueDynamic(
		this, &APlayerCharacter::OnChangedMoveSpeedAttributeEvent);

	SceneCaptureBoomTr = SceneCaptureCameraBoom->GetRelativeTransform();
	SceneCaptureTr = SceneCaptureComponent2D->GetRelativeTransform();

	SceneCaptureComponent2D->ShowOnlyComponent(GetMesh());
	SceneCaptureComponent2D->ShowOnlyComponent(FurComponent);
	SceneCaptureComponent2D->SetActive(false);

	OriginalFurMaterial = FurComponent->GetMaterial(0);

	CreateBodyMaterialInstance();

	CameraBoom->bEnableCameraLag = false;
	CameraBoom->bEnableCameraRotationLag = false;
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//인풋 바인딩
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(NewController))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputDataAsset->DefaultMappingContext, 0);
		}
	}
}

void APlayerCharacter::CreateBodyMaterialInstance()
{
	BodyMaterialInstance.Add(FurComponent->CreateDynamicMaterialInstance(0));
	const auto& index = GetMesh()->GetMaterials().Num();
	for (int32 i = 0; i < 4; i++)
	{
		BodyMaterialInstance.Add(GetMesh()->CreateDynamicMaterialInstance(i));
	}
}

void APlayerCharacter::KneelStart()
{
	if(USaveGameHelperLibrary::IsUseSave(this))
	{
		PlayAnimMontage(StartKneelMontage);
	}
}

void APlayerCharacter::LoadStartLayer()
{
	
	if (auto instance = Cast<USoulLikeInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		instance->LoadLayers(StartLayerRowNames);
		//시작지점 스트리밍
		if (const auto streamingSource = UDataLayerHelperLibrary::SpawnWorldStreamingSourceActor(this))
		{
			streamingSource->OnAfterStreamingComplete.AddUniqueDynamic(this,&APlayerCharacter::OnAfterLoadStartLayerEvent);
			streamingSource->StreamingStart(GetActorLocation());
		}
		
	}
}

void APlayerCharacter::CreateSoulTomb()
{
	if (auto instance = Cast<USoulLikeInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		if (instance->IsUseGameSave())
		{
			if (instance->ExistSaveFile())
			{
				instance->CreateSoulTomb(this);
			}
		}
	}
}

void APlayerCharacter::OnAfterLoadStartLayerEvent()
{
	if (auto instance = Cast<USoulLikeInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		//기본 코어 장착
		EquipDefaultCore();
		//기본 무기 장착
		EquipDefaultWeapon();
		//기본 아이템 제공
		GiveDefaultItem();
		//스카이 타임 저장
		instance->SaveSky(this);
		instance->SaveDataLayer(this);
		KneelStart();
				
		AttributeComponent->SetHP(AttributeComponent->GetMaxHP());
		AttributeComponent->SetMP(AttributeComponent->GetMaxMP());
		AttributeComponent->BroadcastHPEvent();
		AttributeComponent->BroadcastMPEvent();

		FTimerHandle timerHandle;
		GetWorldTimerManager().SetTimer(timerHandle, this, &APlayerCharacter::OnFinishLoadGame, 1.f);
	}
}

void APlayerCharacter::LoadGame()
{
	if (auto pc = GetController<AUserController>())
	{
		//초기화가 끝날때까지 입력 막음
		DisableInput(pc);
		
		if (auto instance = Cast<USoulLikeInstance>(UGameplayStatics::GetGameInstance(this)))
		{

			instance->SetPlayer(this);
			if (instance->IsUseGameSave())
			{
				if (instance->ExistSaveFile() == false)
				{
					UKismetSystemLibrary::PrintString(this,TEXT("세이브 파일이 없는 상태입니다."));
					//첫 스타트 위치를 로드합니다.
					LoadStartLayer();
				}
				else
				{
			
					instance->OnFinishLoadGame.AddUniqueDynamic(this, &APlayerCharacter::OnFinishLoadGame);
					instance->LoadGame();
				}
			}
			else
			{
				//첫 스타트 위치를 로드합니다.
				LoadStartLayer();
			}
		}
	}
}

void APlayerCharacter::OnFinishLoadGame()
{
	if (auto pc = GetController<AUserController>())
	{
		UE_LOGFMT(LogCharacter,Log,"로드 완료");
		GetCapsuleComponent()->SetEnableGravity(true);
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc);

		//초기화가 끝날때까지 입력 막음
		EnableInput(pc);
		CameraBoom->bEnableCameraLag = true;
		CameraBoom->bEnableCameraRotationLag = true;

		auto cameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
		cameraManager->StartCameraFade(1,0,3.f,FColor::Black,true);

		MainWidget->SetVisibility(ESlateVisibility::Visible);
		SetActorTickEnabled(true);

		//오브 배경 스폰
		if (OrbBackgroundActorObject)
		{
			if(OrbBackgroundActor==nullptr)
			{
				OrbBackgroundActor = GetWorld()->SpawnActor<AOrbBackgroundActor>(OrbBackgroundActorObject);
				OrbBackgroundActor->ShowRender(false);
				SceneCaptureComponent2D->SetActive(false);
			}
		}

		//서브시스템은 게임이 종료될때까지 유지되는 것이므로, 레벨을 새로 로드 한 후에 초기화를 해 줘야 합니다.
		pc->GetLocalPlayer()->GetSubsystem<UWidgetInteractionSubsystem>()->InitSubsystem();
		
		//n초마다 마지막으로 밟은 땅의 좌표를 기억합니다. 이 좌표에다 영혼무덤을 생성할 것입니다.
		LastGroundedLocation = GetActorLocation();
		if(GetWorldTimerManager().IsTimerActive(GroundLocationSaveTimerHandle)==false)
		{
			GetWorldTimerManager().SetTimer(GroundLocationSaveTimerHandle, this,
											&APlayerCharacter::SaveLastGroundedLocation, 1.f, true);
		}
		pc->GetWidgetManagerComponent()->RemoveWidget(FGameplayTag::RequestGameplayTag("Widget.Loading"));
		GetMoviePlayer()->StopMovie();

		//딜레이를 안 주면 중간에 사라지기 때문에 어쩔수 없다.
		FTimerHandle tombTimerHandle;
		GetWorldTimerManager().SetTimer(tombTimerHandle,this,&APlayerCharacter::CreateSoulTomb,3.f,false);
	}
}

void APlayerCharacter::SaveLastGroundedLocation()
{
	if (IsDead() == false)
	{
		if (GetCharacterMovement()->IsMovingOnGround())
		{
			LastGroundedLocation = GetActorLocation();
		}
	}
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	if (auto pc = GetController<AUserController>())
	{
		GetCapsuleComponent()->SetEnableGravity(false);
		pc->GetWidgetManagerComponent()->AddWidget(FGameplayTag::RequestGameplayTag("Widget.Loading"));
		
		auto cameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
		cameraManager->SetManualCameraFade(1, FColor::Black, true);

		DynamicSkyActor = Cast<ADynamicSkyActor>(
			UGameplayStatics::GetActorOfClass(this, ADynamicSkyActor::StaticClass()));

		//기본 위젯 생성, 반드시 게임을 불러오는 과정보다 앞에 있어야 합니다.
		pc->GetWidgetManagerComponent()->AddWidget(FGameplayTag::RequestGameplayTag("widget.main"));
		//메인메뉴 포인터 저장
		SetMainWidget();
		MainWidget->SetVisibility(ESlateVisibility::Collapsed);

		Super::BeginPlay();
		/*LoadGame();*/
		
		/*
		//일단 플레이어의 주변이 스트리밍이 끝난 후에, 로드에 필요한 행동을 합니다.
		if (const auto streamingSource = UDataLayerHelperLibrary::SpawnWorldStreamingSourceActor(this))
		{
			streamingSource->OnAfterStreamingComplete.AddUniqueDynamic(this,&APlayerCharacter::LoadGame);
			streamingSource->StreamingStart(GetActorLocation());
			
		}*/
		
	}
}

void APlayerCharacter::OnEndPlayEvent(AActor* Actor, EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::Destroyed)
	{
		if (IsDead())
		{
			OnFinishDissolveTimeLine();
		}
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckFallDeath();

	FindInteractActor();

	//처형 가능한 몬스터를 찾습니다.
	CheckExecutionMonster();

	//캐릭터 후방 표시
	DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), (GetActorForwardVector() * -100) + GetActorLocation(),
	                          50.f, FColor::Red, false, -1, 0, 3.f);
	//입력 후방 표시
	DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(),
	                          (UKismetMathLibrary::GetForwardVector(GetController()->GetControlRotation()) * -100) +
	                          GetActorLocation(), 50.f, FColor::Yellow, false, -1, 0, 3.f);

	//락온중이 아닐 때, 입력방향을 표시합니다.
	{
		auto inputDirVector = FRotator(0, GetControlRotation().Yaw, 0).Vector();
		auto end = GetActorLocation() - (inputDirVector.ForwardVector * MovementInputVector.Y +
			inputDirVector.RightVector * MovementInputVector.X).GetSafeNormal() * -200.f;

		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), end, 50.f, FColor::Orange, false, -1, 0, 3.f);
	}

	//락온중일 때, 입력방향을 표시합니다.
	{
		const auto& dirVector = UMathHelperLibrary::GetControllerInputDir(GetControlRotation(), MovementInputVector) *
			200.f;
		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + dirVector, 50.f, FColor::Cyan,
		                          false, -1, 0, 5.f);
	}
}

void APlayerCharacter::PressMove()
{
	//UE_LOGFMT(LogTemp,Error,"PRESS");
	bPressMove = true;
}

void APlayerCharacter::ReleaseMove()
{
	//UE_LOGFMT(LogTemp,Error,"RELEASE");
	bPressMove = false;
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//움직임
		EnhancedInputComponent->BindAction(InputDataAsset->MoveAction, ETriggerEvent::Triggered, this,
		                                   &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(InputDataAsset->MoveAction, ETriggerEvent::Started, this,
		                                   &APlayerCharacter::PressMove);
		EnhancedInputComponent->BindAction(InputDataAsset->MoveAction, ETriggerEvent::Completed, this,
		                                   &APlayerCharacter::ReleaseMove);

		//시점 변환
		EnhancedInputComponent->BindAction(InputDataAsset->LookAction, ETriggerEvent::Triggered, this,
		                                   &APlayerCharacter::Look);

		//약공격
		EnhancedInputComponent->BindAction(InputDataAsset->NormalAttackAction, ETriggerEvent::Triggered, this,
		                                   &APlayerCharacter::NormalAttack);
		//강공격
		EnhancedInputComponent->BindAction(InputDataAsset->StrongAttackAction, ETriggerEvent::Triggered, this,
		                                   &APlayerCharacter::StrongAttack);

		EnhancedInputComponent->BindAction(InputDataAsset->SprintAction, ETriggerEvent::Started, this,
		                                   &APlayerCharacter::PressSprint);
		EnhancedInputComponent->BindAction(InputDataAsset->SprintAction, ETriggerEvent::Completed, this,
		                                   &APlayerCharacter::ReleaseSprint);


		//락온
		EnhancedInputComponent->BindAction(InputDataAsset->LockOnAction, ETriggerEvent::Triggered, this,
		                                   &APlayerCharacter::LockOn);
		EnhancedInputComponent->BindAction(InputDataAsset->ChangeNextLockOnTargetAction, ETriggerEvent::Triggered, this,
		                                   &APlayerCharacter::NextLockOnTarget);
		EnhancedInputComponent->BindAction(InputDataAsset->ChangePreLockOnTargetAction, ETriggerEvent::Triggered, this,
		                                   &APlayerCharacter::PreLockOnTarget);


		//메뉴 및 팝업처리
		EnhancedInputComponent->BindAction(InputDataAsset->MenuAction, ETriggerEvent::Triggered, this,
		                                   &APlayerCharacter::OpenNavigationOrClosePopUp);

		//마우스 커서 
		EnhancedInputComponent->BindAction(InputDataAsset->ShowCursorAction, ETriggerEvent::Triggered, this,
		                                   &APlayerCharacter::ShowMouseCursor);

		//픽업
		EnhancedInputComponent->BindAction(InputDataAsset->PickUpAction, ETriggerEvent::Triggered, this,
		                                   &APlayerCharacter::PickUp);

		//퀵슬롯
		EnhancedInputComponent->BindAction(InputDataAsset->ConsumeQuickSlotChangeAction, ETriggerEvent::Triggered, this,
		                                   &APlayerCharacter::ChangeConsumeQuickSlot);

		EnhancedInputComponent->BindAction(InputDataAsset->AbilityQuickSlotChangeAction, ETriggerEvent::Triggered, this,
		                                   &APlayerCharacter::ChangeAbilityQuickSlot);

		EnhancedInputComponent->BindAction(InputDataAsset->ConsumeQuickSlotAction, ETriggerEvent::Triggered,
		                                   InventoryComponent,
		                                   &UInventoryComponent::UseConsumeQuickSlot);
		EnhancedInputComponent->BindAction(InputDataAsset->AbilityQuickSlotAction, ETriggerEvent::Triggered,
		                                   InventoryComponent,
		                                   &UInventoryComponent::UseAbilityQuickSlot);


		//상호작용
		EnhancedInputComponent->BindAction(InputDataAsset->InteractAction, ETriggerEvent::Triggered, this,
		                                   &APlayerCharacter::Interaction);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SetupPlayerInputComponent"));
	}
}

void APlayerCharacter::NormalAttack()
{
	if (ExecutableMonster.IsValid())
	{
		//TODO 뒤잡기 실행.
		HideExecutionIcon();
		DoExecute(ExecutableMonster.Get());
	}
	else
	{
		//(TagName="Active.WhiteTiger.Attack.Normal")
		const auto& normalAttackTag = UAbilityHelperLibrary::GetNormalAttackTag(InventoryComponent);
		AbilityComponent->ActivateAbility(normalAttackTag, this,
		                                  InputDataAsset->NormalAttackAction);
	}
}

void APlayerCharacter::StrongAttack()
{
	//TODO 장착한 무기의 태그를 가져와서 사용하도록 해야 합니다.
	const auto& strongAttackTag = UAbilityHelperLibrary::GetStrongAttackTag(InventoryComponent);
	AbilityComponent->ActivateAbility(strongAttackTag,
	                                  this, InputDataAsset->StrongAttackAction);
}

void APlayerCharacter::Dodge()
{
	const auto& dodgeTag = UAbilityHelperLibrary::GetDodgeTag(InventoryComponent);
	AbilityComponent->ActivateAbility(dodgeTag, this, InputDataAsset->SprintAction);
}

void APlayerCharacter::LockOn()
{
	if (AbilityComponent->IsAlreadyActivated(LockOnTag))
	{
		AbilityComponent->ForceEndAbility(LockOnTag);
	}
	else
	{
		AbilityComponent->ActivateAbility(LockOnTag, this, InputDataAsset->LockOnAction);
	}
}

void APlayerCharacter::Run()
{
	if (AbilityComponent)
	{
		if (!AbilityComponent->IsAlreadyActivated(RunTag))
		{
			if (GetCharacterMovement()->Velocity.Length() > 0)
			{
				UE_LOGFMT(LogCharacter, Warning, "RUN!!");
				AbilityComponent->ActivateAbility(RunTag, this, InputDataAsset->SprintAction);
			}
		}
	}
}

void APlayerCharacter::StopRun()
{
	if (AbilityComponent)
	{
		AbilityComponent->ForceEndAbility(RunTag);
	}
}

void APlayerCharacter::PreLockOnTarget()
{
	LockOnComponent->OnLockOnTargetChangePre.Broadcast();
}

void APlayerCharacter::NextLockOnTarget()
{
	LockOnComponent->OnLockOnTargetChangeNext.Broadcast();
}

void APlayerCharacter::OpenNavigationOrClosePopUp()
{
	if (auto pc = Cast<AUserController>(GetController()))
	{
		if (pc->IsThereAnyPopUp())
		{
			for (auto iter : pc->GetPopUpStack())
			{
				UKismetSystemLibrary::PrintString(this, iter->GetName());
			}
			pc->ClosePopUp();
		}
		else
		{
			if (MainWidget.IsValid())
			{
				MainWidget->OpenNavigation();
			}
		}
	}
}

void APlayerCharacter::PickUp()
{
	if (OverlappedOtherActor.IsValid())
	{
		if (OverlappedOtherActor->IsA<AItemActor>())
		{
			InventoryComponent->AddItem(Cast<AItemActor>(OverlappedOtherActor.Get()));
			OverlappedOtherActor = nullptr;
			return;
		}

		if (OverlappedOtherActor->IsA<ASoulTomb>())
		{
			Cast<ASoulTomb>(OverlappedOtherActor)->Absorption(this);
			OverlappedOtherActor = nullptr;
		}
	}
}

void APlayerCharacter::ChangeConsumeQuickSlot()
{
	InventoryComponent->NextConsumeQuickSlot();
}

void APlayerCharacter::ChangeAbilityQuickSlot()
{
	InventoryComponent->NextAbilityQuickSlot();
}

void APlayerCharacter::Interaction()
{
	if (LadderMovementComponent->GetUseLadderMovement())
	{
		return;
	}

	if (InteractableActor.IsValid())
	{
		IInteractionInterface::Execute_Interaction(InteractableActor.Get(), this);
		return;
	}

	const float& halfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float& radius = GetCapsuleComponent()->GetScaledCapsuleRadius();

	auto forwardLoc = GetActorLocation() + (GetActorForwardVector() * radius * 2);
	auto targetLoc = forwardLoc;


	TArray<AActor*> ignoreActors;
	ignoreActors.Add(this);

	TArray<FHitResult> hits;
	if (UKismetSystemLibrary::CapsuleTraceMulti(this, targetLoc, targetLoc, radius, halfHeight,
	                                            UEngineTypes::ConvertToTraceType(ECC_INTERACTION), false,
	                                            ignoreActors, EDrawDebugTrace::ForOneFrame, hits, true))
	{
		for (auto h : hits)
		{
			if (UKismetSystemLibrary::DoesImplementInterface(h.GetActor(), UInteractionInterface::StaticClass()))
			{
				IInteractionInterface::Execute_Interaction(h.GetActor(), this);
				break;
			}
		}
	}
}

void APlayerCharacter::ShowMouseCursor()
{
	if (auto pc = GetController<AUserController>())
	{
		pc->SetShowMouseCursor(!pc->bShowMouseCursor);

		if (!pc->bShowMouseCursor)
		{
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc);
		}

		if (pc->bShowMouseCursor == false)
		{
			if (MainWidget.IsValid())
			{
				MainWidget->OpenNavigation(true);
			}
		}
	}
}

void APlayerCharacter::DoExecute(ABaseMonster* ExecuteTarget)
{
	if (AbilityComponent)
	{
		AbilityComponent->ActivateAbility(ExecuteTag, ExecuteTarget);
	}
}

void APlayerCharacter::ClosePopUp()
{
	if (auto pc = Cast<AUserController>(GetController()))
	{
		pc->ClosePopUp();
	}
}

void APlayerCharacter::PressSprint()
{
	//UE_LOGFMT(LogTemp,Error,"PressSprint");
	bPressSprint = true;
	SprintTimeKeyDown = GetWorld()->GetTimeSeconds();
	//때지 않고 0.25초 이상 지났다면, Run 실행
	GetWorldTimerManager().SetTimer(SprintTimerHandle, this, &APlayerCharacter::Run, 0.35f, false);
}

void APlayerCharacter::ReleaseSprint()
{
	//UE_LOGFMT(LogTemp,Error,"ReleaseSprint");
	bPressSprint = false;
	float SprintKeyUpTime = GetWorld()->GetTimeSeconds();
	float DeltaTime = SprintKeyUpTime - SprintTimeKeyDown;

	StopRun();

	//0.25초 안에 키를 떘을 경우,
	if (DeltaTime < 0.35f)
	{
		GetWorldTimerManager().ClearTimer(SprintTimerHandle);
		Dodge();
	}
}

void APlayerCharacter::EquipDefaultWeapon()
{
	UE_LOGFMT(LogCharacter, Log, "기본 무기 지급");
	if (DefaultSpirit)
	{
		const auto& id = InventoryComponent->SpawnAndAddItem(DefaultSpirit);
		InventoryComponent->UseItem(id);
		if (MainWidget.IsValid())
		{
			//장비창 강제 업데이트
			MainWidget->ForceUpdateSpirit(InventoryComponent->GetInventoryItem(id));
		}
	}
}

void APlayerCharacter::EquipDefaultCore()
{
	UE_LOGFMT(LogCharacter, Log, "기본 코어 지급");
	if (InventoryComponent && DefaultCore)
	{
		const auto& id = InventoryComponent->SpawnAndAddItem(DefaultCore);
		InventoryComponent->UseItem(id);
	}
}

void APlayerCharacter::SetMainWidget()
{
	if (!MainWidget.IsValid())
	{
		if (auto pc = GetController<AUserController>())
		{
			if (auto mainWiget = pc->GetWidgetManagerComponent()->GetWidget(
				FGameplayTag::RequestGameplayTag("Widget.Main")))
			{
				MainWidget = Cast<UMainWidget>(mainWiget);
			}
		}
	}
}

void APlayerCharacter::FindInteractActor()
{
	//평화상태가 아니면 찾을 필요가 없습니다.
	if (IsPeaceState() == false)
	{
		return;
	}

	const float& halfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float& radius = GetCapsuleComponent()->GetScaledCapsuleRadius();

	const auto forwardLoc = GetActorLocation() + (GetActorForwardVector() * radius * 2);
	const auto targetLoc = forwardLoc;


	TArray<FHitResult> hits;
	TArray<AActor*> ignoreActors;
	ignoreActors.Emplace(this);

	if (UKismetSystemLibrary::CapsuleTraceMulti(GetWorld(), targetLoc, targetLoc, radius, halfHeight,
	                                            UEngineTypes::ConvertToTraceType(ECC_INTERACTION),
	                                            false, ignoreActors, EDrawDebugTrace::ForOneFrame, hits,
	                                            true))
	{
		for (const auto& hit : hits)
		{
			if (hit.GetActor()->IsValidLowLevel() && UKismetSystemLibrary::DoesImplementInterface(
				hit.GetActor(), UInteractionInterface::StaticClass()))
			{
				InteractableActor = hit.GetActor();
				if (IInteractionInterface::Execute_ShowInteractionWidgetDirectly(InteractableActor.Get()))
				{
					ShowInteractionWidget(hit.GetActor(), InputDataAsset->InteractAction,
					                      IInteractionInterface::Execute_GetActionName(InteractableActor.Get()));
				}
				break;
			}
		}
	}
	else
	{
		//찾은게 없다면 발 아래 사다리가 있는지 찾습니다.
		if (!FindLadder())
		{
			if (InteractableActor.IsValid())
			{
				HideInteractionWidget();
				InteractableActor = nullptr;
			}
		}
	}
}

bool APlayerCharacter::FindLadder()
{
	const FVector& start = GetActorLocation() - FVector(
		0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 1.5f) + GetActorForwardVector() * 10.f;
	const FVector& end = start + GetActorForwardVector() * -10;

	TArray<AActor*> ignoreActors;
	ignoreActors.Emplace(this);

	TArray<FHitResult> hits;
	if (UKismetSystemLibrary::SphereTraceMulti(this, start, end, 35.f,
	                                           UEngineTypes::ConvertToTraceType(ECC_INTERACTION), false, ignoreActors,
	                                           EDrawDebugTrace::ForOneFrame, hits, true))
	{
		for (const auto& hit : hits)
		{
			if (hit.GetActor()->IsValidLowLevel() && UKismetSystemLibrary::DoesImplementInterface(
				hit.GetActor(), UInteractionInterface::StaticClass()))
			{
				if (hit.GetActor()->IsA<ALadder>())
				{
					InteractableActor = hit.GetActor();
					return true;
				}
			}
		}
	}
	return false;
}

UWidgetComponent* APlayerCharacter::ShowInteractionWidget(const AActor* Target, const UInputAction* Action,
                                                          const FText& ActionName)
{
	if (ActionName.IsEmpty())
	{
		UE_LOGFMT(LogTemp, Error, "인터렉션 위젯의 액션이름이 비었습니다.");
		return nullptr;
	}

	auto keys = GetLocalInputSystem()->QueryKeysMappedToAction(Action);
	if (keys.IsValidIndex(0))
	{
		if (keys[0].GetFName().IsEqual(NAME_None))
		{
			UE_LOGFMT(LogTemp, Error, "인터렉션 위젯의 키 이름이 비정상입니다.");
			return nullptr;
		}

		if (auto widget = GetPressKeyWidget(keys[0].GetFName(), ActionName))
		{
			widget->AttachToComponent(
				Target->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));

			return widget;
		}
	}

	return nullptr;
}

UWidgetComponent* APlayerCharacter::GetInteractionWidget(const AActor* Target, const UInputAction* Action,
                                                         const FText& ActionName)
{
	if (ActionName.IsEmpty())
	{
		UE_LOGFMT(LogTemp, Error, "인터렉션 위젯의 액션이름이 비었습니다.");
		return nullptr;
	}

	auto keys = GetLocalInputSystem()->QueryKeysMappedToAction(Action);
	if (keys.IsValidIndex(0))
	{
		if (keys[0].GetFName().IsEqual(NAME_None))
		{
			UE_LOGFMT(LogTemp, Error, "인터렉션 위젯의 키 이름이 비정상입니다.");
			return nullptr;
		}

		if (auto widget = GetPressKeyWidget(keys[0].GetFName(), ActionName))
		{
			widget->SetVisibility(false);

			widget->AttachToComponent(
				Target->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));

			return widget;
		}
	}

	return nullptr;
}

void APlayerCharacter::HideInteractionWidget()
{
	if (auto widget = GetPressKeyWidget("", FText::GetEmpty()))
	{
		widget->SetVisibility(false);
	}
}

UWidgetComponent* APlayerCharacter::GetPressKeyWidget(FName KeyName, const FText& ActionName)
{
	if (InteractionWidgetComponent == nullptr)
	{
		if (KeyWidgetObject == nullptr)
		{
			UE_LOGFMT(LogTemp, Error, "KeyWidgetObject가 비었습니다. {0} {1}", __FUNCTION__, __LINE__);
			return nullptr;
		}

		if (UActorComponent* newComp = NewObject<
			UActorComponent>(this, UWidgetComponent::StaticClass()))
		{
			//UE_LOGFMT(LogTemp, Warning, "{0} {1}", __FUNCTION__, __LINE__);
			newComp->RegisterComponent();
			InteractionWidgetComponent = Cast<UWidgetComponent>(newComp);
			InteractionWidgetComponent->SetWidgetClass(KeyWidgetObject);
			InteractionWidgetComponent->SetDrawSize(FVector2d(64, 64));
			InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
			InteractionWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			InteractionWidgetComponent->InitWidget();
		}
	}

	if (InteractionWidgetComponent->GetUserWidgetObject() != nullptr)
	{
		Cast<UKeyWidget>(InteractionWidgetComponent->GetUserWidgetObject())->SetKeyAndAction(KeyName, ActionName);
	}
	InteractionWidgetComponent->SetVisibility(true);
	return InteractionWidgetComponent;
}

void APlayerCharacter::ChangeMovementState(EMovementState Type, float Multiplier)
{
	//이동속도 관련 특성 적용

	MovementType = Type;

	const auto calculatedSpeed = AbilityTalentComponent->CalculateModifiedMoveSpeedWithTraits(
		AttributeComponent->GetMoveSpeed());

	switch (Type)
	{
	case EMovementState::Walk:
		GetCharacterMovement()->MaxWalkSpeed = calculatedSpeed * Multiplier;
		break;
	case EMovementState::Run:
		GetCharacterMovement()->MaxWalkSpeed = calculatedSpeed * RunSpeedMultiplier * Multiplier;
		break;
	default: ;
	}
}

EDirection APlayerCharacter::GetPlayerMoveDirection(bool bControllerDirection)
{
	if (!bPressMove)
	{
		return EDirection::Back;
	}

	float dir = 0;
	if (bControllerDirection)
	{
		const auto& point = UMathHelperLibrary::GetControllerInputDir(GetControlRotation(), MovementInputVector) * 200.f
			+ GetActorLocation();
		dir = UMathHelperLibrary::CalculateDegree(this, point);
	}
	else
	{
		dir = UKismetAnimationLibrary::CalculateDirection(GetCharacterMovement()->Velocity, GetActorRotation());
	}

	//각 방향별로 45도씩 할당했습니다.

	//전
	if (-22.5f < dir && dir <= 22.5f)
	{
		return EDirection::Front;
	}

	//전&우
	if (22.5f < dir && dir <= 67.5f)
	{
		return EDirection::FrontRight;
	}

	//우
	if (67.5f < dir && dir <= 112.5f)
	{
		return EDirection::Right;
	}

	//후&우
	if (112.5f < dir && dir <= 157.5f)
	{
		return EDirection::BackRight;
	}

	//후
	if (157.5f < dir && dir <= 180 || -157.5f >= dir && dir > 112.5f)
	{
		return EDirection::Back;
	}

	//후 & 좌
	if (-112.5f >= dir && dir > -157.5f)
	{
		return EDirection::BackLeft;
	}

	//좌
	if (-67.5f >= dir && dir > -112.5f)
	{
		return EDirection::Left;
	}

	//전 & 좌
	if (-22.5f >= dir && dir > -67.5f)
	{
		return EDirection::FrontLeft;
	}


	return EDirection::Back;
}

void APlayerCharacter::OnWaitKnockDownDodgeKeyPress(const FKeyPressedInfo& ActionInfo)
{
	AbilityComponent->ForceEndAbility(FGameplayTag::RequestGameplayTag("Common.Passive.Knockdown"));
	Dodge();
}

void APlayerCharacter::CheckExecutionMonster()
{
	FVector hitLocation;
	if (auto monster = GetPlayerForwardMonster(hitLocation))
	{
		if (monster->IsDead())
		{
			return;
		}

		if (IsExecutable(monster, hitLocation))
		{
			ExecutableMonster = monster;
			ShowExecutionIcon(monster);
			return;
		}
	}

	if (!AbilityComponent->IsAlreadyActivated(ExecuteTag))
	{
		ExecutableMonster = nullptr;
		HideExecutionIcon();
	}
}

ABaseMonster* APlayerCharacter::GetPlayerForwardMonster(FVector& HitLocation)
{
	const auto& start = GetActorLocation();
	FVector end;

	if (AbilityComponent->IsAlreadyActivated(LockOnTag))
	{
		const auto& forwardVector = UKismetMathLibrary::GetForwardVector(
			FRotator(0, GetCapsuleComponent()->GetComponentRotation().Yaw, 0));
		end = forwardVector * DistanceToExecution + start;
	}
	else
	{
		end = (GetActorForwardVector() * DistanceToExecution) + start;
	}

	FHitResult hit;
	TArray<AActor*> ignoreActors;
	ignoreActors.Emplace(this);

	if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), start, end, 10.f,
	                                            UEngineTypes::ConvertToTraceType(ECC_Visibility),
	                                            false, ignoreActors, EDrawDebugTrace::ForOneFrame, hit, true))
	{
		if (hit.GetActor() != nullptr && hit.GetActor()->IsA<ABaseMonster>())
		{
			HitLocation = hit.ImpactPoint;
			return Cast<ABaseMonster>(hit.GetActor());
		}
	}

	return nullptr;
}

bool APlayerCharacter::IsExecutable(ABaseMonster* Monster, const FVector& HitLocation)
{
	if (Monster)
	{
		if (UKismetSystemLibrary::DoesImplementInterface(Monster, UExecutionInterface::StaticClass()))
		{
			//처형 가능한 몬스터인 경우,
			if (IExecutionInterface::Execute_GetIsExecutionable(Monster) && !Monster->IsMighty())
			{
				//플레이어가 몬스터의 후방에 위치하는지 확인합니다.
				if (UMathHelperLibrary::PointToDirection(Monster, HitLocation, 90, 75, 75) == EDirection::Back)
				{
					//Z축을 검사합니다. Z축이 일정 이상 차이나면 불가능합니다.
					//UE_LOGFMT(LogTemp,Log,"내 Z축 : {0} / 몬스터 Z축 : {1}",GetMesh()->GetComponentLocation().Z,Monster->GetMesh()->GetComponentLocation().Z);
					return FMath::IsNearlyEqual(Monster->GetMesh()->GetComponentLocation().Z,
					                            GetMesh()->GetComponentLocation().Z, 10.f);
				}
			}
		}
	}

	return false;
}

void APlayerCharacter::ShowExecutionIcon(ABaseMonster* Monster)
{
	if (Monster)
	{
		if (AbilityComponent->IsAlreadyActivated(ExecuteTag))
		{
			return;
		}

		if (ExecutionWidgetComponent)
		{
			if (!ExecutionWidgetComponent->IsVisible())
			{
				ExecutionWidgetComponent->SetVisibility(true);
			}
			ExecutionWidgetComponent->SetWorldLocation(Monster->GetActorLocation());

			//ExecutionWidgetComponent->AttachToComponent(Monster->GetRootComponent(),FAttachmentTransformRules(EAttachmentRule::SnapToTarget,true));
		}
	}
}

void APlayerCharacter::HideExecutionIcon()
{
	if (ExecutionWidgetComponent)
	{
		ExecutionWidgetComponent->SetVisibility(false);
	}
}

ABaseCharacter* APlayerCharacter::GetExecuteMonster()
{
	return ExecutableMonster.Get();
}

void APlayerCharacter::CheckAroundExistMonster()
{
	UE_LOGFMT(LogCharacter, Log, "CheckAroundExistMonster");
	FHitResult hit;
	bool bHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), GetActorLocation(), GetActorLocation() + 1,
	                                                    CombatCheckTraceRadius,
	                                                    UEngineTypes::ConvertToTraceType(ECC_Monster), false,
	                                                    TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, hit, true);
	if (!bHit)
	{
		UE_LOGFMT(LogCharacter, Log, "평화상태로 강제전환을 시도합니다.");
		ChangeCombatStateFrom.Empty();
		SetPlayerState(EPlayerCharacterState::Peaceful);
	}
}

void APlayerCharacter::SetPlayerState(EPlayerCharacterState NewState)
{
	CurrentState = NewState;
	/*if (CurrentState == EPlayerCharacterState::Combat)
	{
		if (GetWorldTimerManager().TimerExists(PeaceTimerHandle))
		{
			GetWorldTimerManager().ClearTimer(PeaceTimerHandle);
		}
		//주변에 적이 있는가 계속 검사합니다.
		//없다면 평화로 전환합니다.
		GetWorldTimerManager().SetTimer(PeaceTimerHandle, this, &APlayerCharacter::CheckAroundExistMonster,
		                                PeaceCheckTime, true);
	}
	else
	{
		if (GetWorldTimerManager().TimerExists(PeaceTimerHandle))
		{
			GetWorldTimerManager().ClearTimer(PeaceTimerHandle);
		}
	}*/

	OnChangePlayerState.Broadcast(CurrentState);
}

void APlayerCharacter::SetPlayerStateBy(EPlayerCharacterState NewState, AActor* By)
{
	if(!IsValidLowLevel())
	{
		return;
	}
	
	if(IsDead())
	{
		return;
	}
	
	if (NewState == EPlayerCharacterState::Combat)
	{
		CurrentState = NewState;
		ChangeCombatStateFrom.Emplace(By);
		OnChangePlayerState.Broadcast(CurrentState);

		if (GetWorldTimerManager().TimerExists(PeaceTimerHandle))
		{
			GetWorldTimerManager().ClearTimer(PeaceTimerHandle);
		}
		//주변에 적이 있는가 계속 검사합니다.
		//없다면 평화로 전환합니다.
		GetWorldTimerManager().SetTimer(PeaceTimerHandle, this, &APlayerCharacter::CheckAroundExistMonster,
		                                PeaceCheckTime, true);
	}
	else
	{
		if (By == nullptr)
		{
			CurrentState = NewState;
			OnChangePlayerState.Broadcast(CurrentState);
			return;
		}

		UE_LOGFMT(LogCharacter, Log, "평화상태로 전환 시도합니다.");


		if (ChangeCombatStateFrom.Num() > 0)
		{
			ChangeCombatStateFrom.Remove(By);
			UE_LOGFMT(LogCharacter, Log, "스텍에서 다음 대상을 제거합니다 : {0}", GetNameSafe(By));
			if (ChangeCombatStateFrom.Num() <= 0)
			{
				if (GetWorldTimerManager().TimerExists(PeaceTimerHandle))
				{
					GetWorldTimerManager().ClearTimer(PeaceTimerHandle);
				}

				CurrentState = NewState;
				OnChangePlayerState.Broadcast(CurrentState);
			}
		}
		else
		{
			CurrentState = NewState;
			OnChangePlayerState.Broadcast(CurrentState);
		}
	}
}

bool APlayerCharacter::IsPeaceState()
{
	if (ChangeCombatStateFrom.Num() != 0)
	{
		const auto& arr = ChangeCombatStateFrom.Array();
		for (auto i = 0; i < arr.Num(); i++)
		{
			if (arr[i] == nullptr)
			{
				ChangeCombatStateFrom.Remove(arr[i]);
			}
		}

		if (ChangeCombatStateFrom.Num() == 0)
		{
			CurrentState = EPlayerCharacterState::Peaceful;
			OnChangePlayerState.Broadcast(CurrentState);
		}
	}


	return CurrentState == EPlayerCharacterState::Peaceful;
}

void APlayerCharacter::OnChangedMoveSpeedAttributeEvent()
{
	GetCharacterMovement()->MaxWalkSpeed = AttributeComponent->GetMoveSpeed();
	//이미 달리고 있는 경우,
	if (AbilityComponent->IsAlreadyActivated(RunTag))
	{
		ChangeMovementState(EMovementState::Run, 1.25f);
	}
	else
	{
		ChangeMovementState(EMovementState::Walk, 1.f);
	}
}

void APlayerCharacter::OnDeadEvent(AActor* Who, AActor* DeadBy)
{
	if (CharacterState != ECharacterState::DEAD)
	{
		Super::OnDeadEvent(Who, DeadBy);

		DisableInput(GetController<APlayerController>());
		LockOnComponent->End();
		//사망상태 저장
		if (auto instance = GetGameInstance<USoulLikeInstance>())
		{
			if (instance->IsUseGameSave())
			{
				instance->OnDeadPlayer(this);
			}
		}
	}
}

void APlayerCharacter::ChangeStatusEffectMaterial(EStatusEffect EffectType)
{
	Super::ChangeStatusEffectMaterial(EffectType);

	if (StatusEffectMaterialMap.Contains(EffectType))
	{
		FurComponent->SetMaterial(0, StatusEffectMaterialMap[EffectType]);
	}
}

void APlayerCharacter::RestoreStatusEffectMaterial()
{
	Super::RestoreStatusEffectMaterial();

	FurComponent->SetMaterial(0, OriginalFurMaterial);
}


void APlayerCharacter::ForceEndDodge()
{
	const auto& dodgeTag = UAbilityHelperLibrary::GetDodgeTag(InventoryComponent);
	AbilityComponent->ForceEndAbility(dodgeTag);

	//추가회피 어빌리티가 있다면, 그것도 강제종료 시킵니다.
	const auto& dodgeContTag = UAbilityHelperLibrary::GetDodgeContTag(InventoryComponent);
	AbilityComponent->ForceEndAbility(dodgeContTag);
}

void APlayerCharacter::OnUpdateDeadDissolveTimeLine(float Value)
{
	FName param = "Percent";
	for (auto iter : BodyMaterialInstance)
	{
		iter->SetScalarParameterValue(param, Value);
	}
}

void APlayerCharacter::OnFinishDissolveTimeLine()
{
	UE_LOGFMT(LogCharacter, Warning, "사망 디졸브가 끝났습니다. 레벨을 다시 로드합니다.");
	UGameplayStatics::OpenLevel(this, FName(UGameplayStatics::GetCurrentLevelName(this)));
}

void APlayerCharacter::OnTriggerHitAnimationEnterEvent(ABaseCharacter* DamagedCharacter, AActor* HitBy)
{
	Super::OnTriggerHitAnimationEnterEvent(DamagedCharacter, HitBy);

	UE_LOGFMT(LogCharacter, Log, "애니메이션 트리거 엔터");
	SetIgnoreMoveInput(
		true, this, FGameplayTag::RequestGameplayTag("Common.Passive.IgnoreMoveInput.Animation.TriggerHit"));
	//GetController<AUserController>()->SetIgnoreMoveInput(true);
}

void APlayerCharacter::OnTriggerHitAnimationExitEvent(ABaseCharacter* DamagedCharacter, AActor* HitBy)
{
	Super::OnTriggerHitAnimationExitEvent(DamagedCharacter, HitBy);

	UE_LOGFMT(LogCharacter, Log, "애니메이션 트리거 엑시트");
	SetIgnoreMoveInput(false, this,
	                   FGameplayTag::RequestGameplayTag("Common.Passive.IgnoreMoveInput.Animation.TriggerHit"));
	GetController<AUserController>()->ResetIgnoreMoveInput();
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		if (LadderMovementComponent->GetUseLadderMovement())
		{
			LadderMovementComponent->AddLadderMovementInput(MovementVector.Y);
		}
		else
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			// get right vector 
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);


			// add movement 
			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);

			MovementInputVector = FVector(MovementVector.X, MovementVector.Y, 0);
		}
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		auto pc = Cast<AUserController>(Controller);
		if (pc->bShowMouseCursor == false)
		{
			// add yaw and pitch input to controller
			AddControllerYawInput(LookAxisVector.X * pc->GetMouseSensitivity());
			AddControllerPitchInput(LookAxisVector.Y * pc->GetMouseSensitivity());
		}
	}
}

void APlayerCharacter::AddControllerYawInput(float Val)
{
	Super::AddControllerYawInput(Val);
}

void APlayerCharacter::AddControllerPitchInput(float Val)
{
	Super::AddControllerPitchInput(Val);
}


UInputHandlerComponent* APlayerCharacter::GetInputHandlerComponent()
{
	if (auto pc = GetController<AUserController>())
	{
		return pc->GetInputHandlerComponent();
	}

	return nullptr;
}

void APlayerCharacter::ResetSceneCaptureCamera() const
{
	SceneCaptureCameraBoom->SetRelativeTransform(SceneCaptureBoomTr);
	SceneCaptureComponent2D->SetRelativeTransform(SceneCaptureTr);
}

void APlayerCharacter::ShowRender(bool bIsVisible)
{
	if (bIsVisible)
	{
		SceneCaptureComponent2D->TextureTarget = RenderTextureTarget;
	}
	else
	{
		SceneCaptureComponent2D->TextureTarget = nullptr;
	}
	SceneCaptureComponent2D->SetActive(bIsVisible);
}

void APlayerCharacter::SetDefaultMappingContext()
{
	UE_LOGFMT(LogCharacter, Log, "입력메핑 스택4");
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		UE_LOGFMT(LogCharacter, Log, "기본 입력 매핑을 적용합니다.");
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(InputDataAsset->DefaultMappingContext, 0);
		}
	}
	else
	{
		UE_LOGFMT(LogCharacter, Log, "컨트롤러를 찾을 수 없어, 기본입력매핑으로 돌릴 수 없습니다.");
	}
}

UEnhancedInputLocalPlayerSubsystem* APlayerCharacter::GetLocalInputSystem() const
{
	if (auto pc = GetController<AUserController>())
	{
		if (pc->PlayerInput->IsA<UEnhancedPlayerInput>())
		{
			return ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		}
	}

	return nullptr;
}

void APlayerCharacter::OnLevelUpEvent()
{
	AbilityComponent->ApplyCue(LevelUpCueInfo);
}

#if WITH_EDITOR
void APlayerCharacter::CheatPower(bool bActive)
{
	if(bActive)
	{
		OriginalPhysicPower = AttributeComponent->GetPhysicalAttack();
		OriginalMagicPower = AttributeComponent->GetMagicalAttack();
		
		AttributeComponent->SetPhysicalAttack(static_cast<float>(INT_MAX));
		AttributeComponent->SetMagicalAttack(static_cast<float>(INT_MAX));
	}else
	{
		AttributeComponent->SetPhysicalAttack(OriginalPhysicPower);
		AttributeComponent->SetMagicalAttack(OriginalMagicPower);
	}
	
	AttributeComponent->OnCharacterInformationUpdate.Broadcast();
}

void APlayerCharacter::CheatMoney()
{
	AttributeComponent->SetEXP(static_cast<float>(INT_MAX));
	AttributeComponent->OnCharacterInformationUpdate.Broadcast();
}

void APlayerCharacter::CheatInvincible(bool bActive)
{
	if(bActive)
	{
		AbilityComponent->K2_ApplyEffect(InvincibleEffectObject,this,FOnEffectExpired(),nullptr);
	}else
	{
		AbilityComponent->EndEffectByTag(INVINCIBILITY_TAG);
	}
}

void APlayerCharacter::CheatSP(bool bActive)
{
	if(bActive)
	{
		OriginalSP = AttributeComponent->GetMaxSP();
	
		AttributeComponent->SetMaxSP(static_cast<float>(INT_MAX));
		AttributeComponent->SetSP(static_cast<float>(INT_MAX));

		UE_LOGFMT(LogCharacter,Log,"플레이어의 SP 치트 설정됨 : {0}",AttributeComponent->GetMaxSP());
	}else
	{
		AttributeComponent->SetMaxSP(OriginalSP);
		AttributeComponent->SetSP(AttributeComponent->GetMaxSP());
	}
	AttributeComponent->OnCharacterInformationUpdate.Broadcast();
}

void APlayerCharacter::CheatWalkSpeed(float NewSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}
#endif

#undef LOCTEXT_NAMESPACE
