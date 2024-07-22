// Fill out your copyright notice in the Description page of Project Settings.


#include "92_Tools/PortalActor.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "91_Sky/DynamicSkyActor.h"
#include "99_Subsystem/TransitionPortalSubsystem.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SphereComponent.h"
#include "Components/WorldPartitionStreamingSourceComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Logging/StructuredLog.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"

// Sets default values
APortalActor::APortalActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
	RootComponent = SceneComponent;

	PortalMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMeshComponent"));
	PortalMeshComponent->SetupAttachment(RootComponent);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(PortalMeshComponent);

	PortalCameraComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("PortalCamera"));
	PortalCameraComponent->SetupAttachment(RootComponent);

	/*StreamingSourceComponent = CreateDefaultSubobject<UWorldPartitionStreamingSourceComponent>(TEXT("StreamingSourceComponent"));
	StreamingSourceComponent->SetAutoActivate(false);*/
}

void APortalActor::SetTextureTarget(APortalActor* PortalActor)
{
	if (PortalActor)
	{
		SetPortalRenderTarget();
		PortalActor->PortalCameraComponent->TextureTarget = PortalRenderTarget;
		//PortalCameraComponent->TextureTarget = PortalActor->PortalRenderTarget;
	}
}

// Called when the game starts or when spawned
void APortalActor::BeginPlay()
{
	Super::BeginPlay();

	MaterialSetting();
	
	SetTickGroup(TG_PostUpdateWork);
	Sky = Cast<ADynamicSkyActor>(UGameplayStatics::GetActorOfClass(this, ADynamicSkyActor::StaticClass()));

	SetTextureTarget(LinkedPortal);
}

void APortalActor::SetPortalRenderTarget()
{
	if(PortalRenderTarget == nullptr)
	{
		const auto& viewportSize = UWidgetLayoutLibrary::GetViewportSize(this);
		PortalRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, viewportSize.X, viewportSize.Y);
	}
	
	if (PortalRenderTarget)
	{
		if(PortalMaterialInstance)
		{
			PortalMaterialInstance->SetTextureParameterValue("LocationTexture", PortalRenderTarget);
		}else
		{
			UE_LOGFMT(LogTemp,Error,"PortalMaterialInstance 널입니다.");
		}
	}else
	{
		UE_LOGFMT(LogTemp,Error,"PortalRenderTarget이 널입니다.");
	}
}

void APortalActor::MaterialSetting()
{
	if (PortalMaterial)
	{
		PortalMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, PortalMaterial);
		if (PortalMaterialInstance)
		{
			PortalMeshComponent->SetMaterial(0, PortalMaterialInstance);
			SetPortalRenderTarget();
		}else
		{
			UE_LOGFMT(LogTemp,Error,"PortalMaterialInstance 널입니다.");
		}
	}else
	{
		UE_LOGFMT(LogTemp,Error,"PortalMaterial 널입니다.");
	}
}

void APortalActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	MaterialSetting();
}

void APortalActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

// Called every frame
void APortalActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateSceneCapture(LinkedPortal);
	CheckViewportSize();
}

void APortalActor::UpdateSceneCapture(APortalActor* Target) const
{
	if (Target == nullptr)
	{
		return;
	}

	const auto& tr = GetActorTransform();
	const auto& scale = tr.GetScale3D();

	const auto& newTr = UKismetMathLibrary::MakeTransform(tr.GetLocation(), tr.Rotator(),
	                                                      FVector(scale.X * -1.f, scale.Y * -1.f, scale.Z));
	const auto playerCameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);

	const auto& inverseTrLoc = UKismetMathLibrary::InverseTransformLocation(
		newTr, playerCameraManager->GetTransformComponent()->GetComponentLocation());
	const auto& newLoc = UKismetMathLibrary::TransformLocation(Target->GetActorTransform(), inverseTrLoc);


	const auto& cameraRotation = playerCameraManager->GetTransformComponent()->GetComponentRotation();
	FVector x;
	FVector y;
	FVector z;
	UKismetMathLibrary::BreakRotIntoAxes(cameraRotation, x, y, z);

	const auto& x_inverseTrDir = UKismetMathLibrary::InverseTransformDirection(tr, x);
	const auto& x_mirrorX = UKismetMathLibrary::MirrorVectorByNormal(x_inverseTrDir, FVector(1, 0, 0));
	const auto& x_mirrorY = UKismetMathLibrary::MirrorVectorByNormal(x_mirrorX, FVector(0, 1, 0));

	const auto& fwd = UKismetMathLibrary::TransformDirection(Target->GetActorTransform(), x_mirrorY);

	const auto& y_inverseTrDir = UKismetMathLibrary::InverseTransformDirection(tr, y);
	const auto& y_mirrorX = UKismetMathLibrary::MirrorVectorByNormal(y_inverseTrDir, FVector(1, 0, 0));
	const auto& y_mirrorY = UKismetMathLibrary::MirrorVectorByNormal(y_mirrorX, FVector(0, 1, 0));

	const auto& right = UKismetMathLibrary::TransformDirection(Target->GetActorTransform(), y_mirrorY);


	const auto& z_inverseTrDir = UKismetMathLibrary::InverseTransformDirection(tr, z);
	const auto& z_mirrorX = UKismetMathLibrary::MirrorVectorByNormal(z_inverseTrDir, FVector(1, 0, 0));
	const auto& z_mirrorY = UKismetMathLibrary::MirrorVectorByNormal(z_mirrorX, FVector(0, 1, 0));

	const auto& up = UKismetMathLibrary::TransformDirection(Target->GetActorTransform(), z_mirrorY);


	const auto& newRot = UKismetMathLibrary::MakeRotationFromAxes(fwd, right, up);

	Target->PortalCameraComponent->SetWorldLocationAndRotation(newLoc, newRot);
}

void APortalActor::CheckViewportSize()
{
	if (PortalRenderTarget)
	{
		const auto& viewportSize = UWidgetLayoutLibrary::GetViewportSize(this);

		if (!(PortalRenderTarget->SizeX == viewportSize.X && PortalRenderTarget->SizeY == viewportSize.Y))
		{
			UKismetRenderingLibrary::ResizeRenderTarget2D(PortalRenderTarget, viewportSize.X, viewportSize.Y);
		}
	}
}

void APortalActor::TeleportPlayer(AActor* OtherActor, APortalActor* Target)
{
	if (auto player = Cast<APlayerCharacter>(OtherActor))
	{
		const auto& tr = GetActorTransform();
		const auto& scale = tr.GetScale3D();

		if (Target)
		{
			const auto& teleportLocation = UKismetMathLibrary::TransformLocation(
				Target->GetActorTransform(),
				UKismetMathLibrary::InverseTransformLocation(
					FTransform(tr.GetRotation(), tr.GetLocation(), FVector(scale.X * -1, scale.Y * -1, scale.Z)),
					player->GetActorLocation())) + Target->GetActorForwardVector() * 500.f;

			player->SetActorLocation(teleportLocation);


			if (auto pc = player->GetController<APlayerController>())
			{
				pc->SetControlRotation(Target->GetActorRotation());
			}
		}
	}
}

void APortalActor::SetLinkedPortal(APortalActor* OtherPortal)
{
	UE_LOGFMT(LogTemp,Log,"{0}포탈과 {1}포탈을 링크합니다.",OtherPortal->GetName(),GetName());
	LinkedPortal = OtherPortal;
	SetTextureTarget(OtherPortal);
}

void APortalActor::SetSkyTimeAndWeather()
{
	if (Sky == nullptr)
	{
		Sky = Cast<ADynamicSkyActor>(UGameplayStatics::GetActorOfClass(this, ADynamicSkyActor::StaticClass()));
	}
	
	Sky->TimeOfDay = SkyTime;
	Sky->SetWeather(Weather);
	Sky->bAnimateTimeOfDay = bUseDayNightCycle;
	
}

ATransition_PortalActor::ATransition_PortalActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	PlayerCheckComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("PlayerCheckComponent"));
	PlayerCheckComponent->SetupAttachment(RootComponent);

	StreamingSourceComponent = CreateDefaultSubobject<UWorldPartitionStreamingSourceComponent>(
		TEXT("StreamingSourceComponent"));
	StreamingSourceComponent->SetAutoActivate(false);

	PostProcessOverlapComponent = CreateDefaultSubobject<USphereComponent>(TEXT("PostProcessOverlapComponent"));
	PostProcessOverlapComponent->SetupAttachment(RootComponent);

	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
	PostProcessComponent->SetupAttachment(PostProcessOverlapComponent);
	
	bIsSpatiallyLoaded = false;
}

void ATransition_PortalActor::BeginPlay()
{
	Super::BeginPlay();
	
	StreamingSourceComponent->SetActive(bShouldActivateStreaming);
	
	switch(TransitionPortalType)
	{
	case ETransitionPortalType::ENTRANCE_TO_TRANSITION_ZONE:
	case ETransitionPortalType::ENTRANCE_FROM_ANOTHER_ZONE:
		PostProcessComponent->DestroyComponent();
		PostProcessOverlapComponent->DestroyComponent();
		break;
	default: ;
	}
	
	if (auto system = UDataLayerManager::GetDataLayerManager(this))
	{
		system->OnDataLayerInstanceRuntimeStateChanged.AddUniqueDynamic(this,&ATransition_PortalActor::OnDatalayerInstanceRuntimeStateChangedEvent);
	}
}

void ATransition_PortalActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (PlayerCheckComponent)
	{
		PlayerCheckComponent->OnComponentBeginOverlap.AddUniqueDynamic(
			this, &ATransition_PortalActor::OnPlayerCheckedEvent);
	}
	if (BoxComponent)
	{
		BoxComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ATransition_PortalActor::TeleportEvent);
	}
}

void ATransition_PortalActor::TeleportEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                            const FHitResult& SweepResult)
{
	
	if (OtherActor->IsA<APlayerCharacter>())
	{
		SetActorEnableCollision(false);
		if(PortalMaterialInstance)
		{
			PortalMaterialInstance->SetVectorParameterValue("Color",FLinearColor::Red);
		}
		
		FTimerDelegate resetTimerDel = FTimerDelegate::CreateUObject(this,&ATransition_PortalActor::ReActivatePortal);
		GetWorldTimerManager().SetTimer(ReactivateTimerHandle,resetTimerDel,3.f,false);
		
		Player = OtherActor;

		switch (TransitionPortalType)
		{
		case ETransitionPortalType::ENTRANCE_TO_TRANSITION_ZONE:
		case ETransitionPortalType::ENTRANCE_FROM_ANOTHER_ZONE:
		case ETransitionPortalType::EXIT_TO_ORIGINAL_ZONE:
			SetSkyTimeAndWeather();
			break;
		}
		
		DisableCameraLag();
		TeleportToZone(OtherActor);
	}
}

void ATransition_PortalActor::OnPlayerCheckedEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                   bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<APlayerCharacter>())
	{
		if (auto system = UTransitionPortalSubsystem::GetSubsystem(this))
		{
			system->UpdateLink(this);
		}

		switch (TransitionPortalType)
		{
		case ETransitionPortalType::ENTRANCE_TO_TRANSITION_ZONE:
		case ETransitionPortalType::ENTRANCE_FROM_ANOTHER_ZONE:
			DataLayerSetting();
			break;
		}
	}
}

void ATransition_PortalActor::TeleportToZone(AActor* OtherActor)
{
	if (OtherActor->IsA<APlayerCharacter>())
	{
		if(auto player = Cast<APlayerCharacter>(Player))
		{
			player->GetCharacterMovement()->StopMovementImmediately();
			player->SetIgnoreMoveInput(true,this,FGameplayTag::RequestGameplayTag("Common.PlayerInput.Ignore.Move.Teleport"));

			UE_LOGFMT(LogTemp,Log,"이 포탈로 텔레포트 합니다 : {0}",LinkedPortal->GetName());

			//이 액터를 스트리밍 소스로 사용합니다.
			//플레이어가 트렌지션 구역으로 이동한 뒤에 다시 돌아올수도 있기 때문입니다.
			StreamingSourceComponent->SetActive(true);

			const auto& tr = GetActorTransform();
			const auto& scale = tr.GetScale3D();
			const auto& teleportLocation = UKismetMathLibrary::TransformLocation(
				LinkedPortal->GetActorTransform(),
				UKismetMathLibrary::InverseTransformLocation(
					FTransform(tr.GetRotation(), tr.GetLocation(), FVector(scale.X * -1, scale.Y * -1, scale.Z)),
					Player->GetActorLocation())) + LinkedPortal->GetActorForwardVector() * 500.f;

			UKismetSystemLibrary::DrawDebugPoint(this,teleportLocation,30.f,FLinearColor::Yellow,1000.f);
		
			Player->SetActorLocation(teleportLocation);

			EnableCameraLag();
			
			if (auto pc = player->GetController<APlayerController>())
			{
				Player->SetActorRotation(LinkedPortal->GetActorRotation());
				pc->SetControlRotation(LinkedPortal->GetActorRotation());
			}
		
			player->SetIgnoreMoveInput(false,this,FGameplayTag::RequestGameplayTag("Common.PlayerInput.Ignore.Move.Teleport"));
			Player = nullptr;
			
			//필요하다면 다른 지역의 데이터 레이어의 상태를 변경합니다.
			ActivateDataLayerAssetToActive();

			switch (TransitionPortalType) {
			case ETransitionPortalType::ENTRANCE_TO_TRANSITION_ZONE:
			case ETransitionPortalType::ENTRANCE_FROM_ANOTHER_ZONE:
				
				break;
			case ETransitionPortalType::EXIT_TO_ORIGINAL_ZONE:
			case ETransitionPortalType::EXIT_TO_ANOTHER_ZONE:
				
				if(auto portalSystem = UTransitionPortalSubsystem::GetSubsystem(this))
				{
					portalSystem->GetOutPortal()->ReActivatePortal();
					portalSystem->GetInPortal()->ReActivatePortal();
					//반대진영의 포탈을 가져와야 합니다.
					portalSystem->GetOppositePortal(TransitionPortalType);
					//반대진영 포탈의 언로드 레이어정보를 읽어와 언로드합니다.
				}
				
				break;
	
			}
			
			
		}
	}
}

void ATransition_PortalActor::OnDatalayerInstanceRuntimeStateChangedEvent(const UDataLayerInstance* DataLayer,
	EDataLayerRuntimeState State)
{
	if(auto portalSystem = UTransitionPortalSubsystem::GetSubsystem(this))
	{
		if (auto layerSystem = UDataLayerManager::GetDataLayerManager(this))
		{
			if(auto instance = layerSystem->GetDataLayerInstanceFromAsset(DataLayerAssetToActive))
			{
				if(instance == DataLayer && State == EDataLayerRuntimeState::Activated)
				{
					UE_LOGFMT(LogTemp,Log,"다음 구역 로드가 완료되었습니다. : {0}",DataLayerAssetToActive->GetName());
					portalSystem->GetOutPortal()->ReActivatePortal();
					
					/*//이전 구역으로 돌아가지 못하게 막습니다.
					portalSystem->GetInPortal()->DeactivatePortal();
					//레이어를 끕니다.
					for(auto iter : DataLayerAssetsToUnloaded){
						layerSystem->SetDataLayerRuntimeState(iter,EDataLayerRuntimeState::Unloaded);
					}*/
				}
			}
		
		}
	}
}

void ATransition_PortalActor::ReActivatePortal()
{
	SetActorEnableCollision(true);
	if(PortalMaterialInstance)
	{
		PortalMaterialInstance->SetVectorParameterValue("Color",OriginalPortalColor);
	}
}

void ATransition_PortalActor::DeactivatePortal()
{
	SetActorEnableCollision(false);
	if(PortalMaterialInstance)
	{
		PortalMaterialInstance->SetVectorParameterValue("Color",FLinearColor::Red);
	}
}

void ATransition_PortalActor::EnableCameraLag()
{
	switch (TransitionPortalType)
	{
	case ETransitionPortalType::EXIT_TO_ORIGINAL_ZONE:
	case ETransitionPortalType::EXIT_TO_ANOTHER_ZONE:
		if (GetWorldTimerManager().TimerExists(EnableCameraLagTimer))
		{
			GetWorldTimerManager().ClearTimer(EnableCameraLagTimer);
		}
		FTimerDelegate timerDel = FTimerDelegate::CreateUObject(this, &ATransition_PortalActor::SetPlayerCameraLag,
		                                                        true);
		GetWorldTimerManager().SetTimer(EnableCameraLagTimer, timerDel, 3.f, false);

		break;
	}
}

void ATransition_PortalActor::DisableCameraLag()
{
	if (auto player = Cast<APlayerCharacter>(Player))
	{
		switch (TransitionPortalType)
		{
		case ETransitionPortalType::ENTRANCE_TO_TRANSITION_ZONE:
		case ETransitionPortalType::ENTRANCE_FROM_ANOTHER_ZONE:
			SetPlayerCameraLag(false);
			break;
		}
	}
}

void ATransition_PortalActor::DataLayerSetting()
{
	if (auto layerSystem = UDataLayerManager::GetDataLayerManager(this))
	{
		for (auto iter : TransitionDataLayerAssetsToUnloaded)
		{
			layerSystem->SetDataLayerRuntimeState(iter, EDataLayerRuntimeState::Unloaded);
		}
		
		layerSystem->SetDataLayerRuntimeState(TransitionDataLayerAssetToActivate, EDataLayerRuntimeState::Activated);
	}
}

void ATransition_PortalActor::SetPlayerCameraLag(bool bEnabled)
{
	if (auto player = Cast<APlayerCharacter>(Player))
	{
		player->GetCameraBoom()->bEnableCameraLag = bEnabled;
	}
}

void ATransition_PortalActor::OverrideSkyAndWeatherCurValue()
{
	if (Sky == nullptr)
	{
		Sky = Cast<ADynamicSkyActor>(UGameplayStatics::GetActorOfClass(this, ADynamicSkyActor::StaticClass()));
	}
	
	SkyTime = Sky->TimeOfDay;
	Weather = Sky->GetWeather();
	bUseDayNightCycle = Sky->bAnimateTimeOfDay;
}

void ATransition_PortalActor::ActivateDataLayerAssetToActive()
{
	
	//로드하는동안 다른 지역으로 나가는 포탈을 비활성화 합니다.
	if(auto portalSystem = UTransitionPortalSubsystem::GetSubsystem(this))
	{
		portalSystem->GetOutPortal()->DeactivatePortal();
	}
	
	if (auto layerSystem = UDataLayerManager::GetDataLayerManager(this))
	{
		if(DataLayerAssetToActive != nullptr){
			UE_LOGFMT(LogTemp,Log,"다음 구역을 로드하는 중입니다 : {0}",DataLayerAssetToActive->GetName());
			layerSystem->SetDataLayerRuntimeState(DataLayerAssetToActive, EDataLayerRuntimeState::Activated);
		}
	}
}
