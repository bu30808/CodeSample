// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/00_Player/OrbBackgroundActor.h"

#include "NiagaraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"

// Sets default values
AOrbBackgroundActor::AOrbBackgroundActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneComponent;
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Background"));
	NiagaraComponent->SetupAttachment(RootComponent);
	SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent2D"));
	SceneCaptureComponent2D->SetupAttachment(RootComponent);
	SceneCaptureComponent2D->PostProcessBlendWeight = 0.f;
	SceneCaptureComponent2D->bConsiderUnrenderedOpaquePixelAsFullyTranslucent = true;
	SceneCaptureComponent2D->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;


	SceneCaptureComponent2D->SetAutoActivate(false);

	static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> textureTarget(TEXT(
		"/Script/Engine.CanvasRenderTarget2D'/Game/Blueprints/00_Character/04_Orb/RT_OrbBackground.RT_OrbBackground'"));
	if (textureTarget.Succeeded())
	{
		TextureTarget = textureTarget.Object;
	}
}

// Called when the game starts or when spawned
void AOrbBackgroundActor::BeginPlay()
{
	Super::BeginPlay();
}

void AOrbBackgroundActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (SceneCaptureComponent2D)
	{
		SceneCaptureComponent2D->ShowOnlyComponent(NiagaraComponent);
		SceneCaptureComponent2D->SetActive(false);
	}
}

// Called every frame
void AOrbBackgroundActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOrbBackgroundActor::ShowRender(bool bIsVisible)
{
	if (bIsVisible)
	{
		SceneCaptureComponent2D->TextureTarget = TextureTarget;
	}
	else
	{
		SceneCaptureComponent2D->TextureTarget = nullptr;
	}
	SceneCaptureComponent2D->SetActive(bIsVisible);
}
