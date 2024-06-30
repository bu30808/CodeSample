// Fill out your copyright notice in the Description page of Project Settings.


#include "02_Ability/AbilityCue.h"

#include "NiagaraComponent.h"
#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Logging/StructuredLog.h"
#include "NiagaraDataInterfaceRigidMeshCollisionQuery.h"

DEFINE_LOG_CATEGORY(LogCue)

// Sets default values
AAbilityCue::AAbilityCue()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = SceneComponent;

	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("CasecadeParticleComponent"));
	ParticleSystemComponent->SetupAttachment(RootComponent);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ParticleComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);

	ParticleSystemComponent->LDMaxDrawDistance = 3000.f;
	NiagaraComponent->LDMaxDrawDistance = 3000.f;
}

void AAbilityCue::OnNiagaraFinished(UNiagaraComponent* PSystem)
{
	NiagaraComponent->Deactivate();
	if (bReusable == false)
	{
		bParticleEffectFinish = true;
		if (AudioComponent->Sound != nullptr)
		{
			if (bAudioFinish)
			{
				K2_DestroyActor();
			}
		}
		else
		{
			K2_DestroyActor();
		}
	}
	else
	{
		DeactivateCue();
	}
}

void AAbilityCue::OnAudioFinished()
{
	AudioComponent->Stop();
	AudioComponent->Deactivate();

	if (bReusable == false)
	{
		bAudioFinish = true;
		if (ParticleSystemComponent->Template != nullptr || NiagaraComponent->GetFXSystemAsset() != nullptr)
		{
			if (bParticleEffectFinish)
			{
				K2_DestroyActor();
			}
		}
		else
		{
			K2_DestroyActor();
		}
	}
	else
	{
		DeactivateCue();
	}
}

void AAbilityCue::OnParticleFinished(UParticleSystemComponent* PSystem)
{
	ParticleSystemComponent->Deactivate();
	if (bReusable == false)
	{
		bParticleEffectFinish = true;

		if (AudioComponent->Sound != nullptr)
		{
			if (bAudioFinish)
			{
				K2_DestroyActor();
			}
		}
		else
		{
			K2_DestroyActor();
		}
	}
	else
	{
		DeactivateCue();
	}
}

void AAbilityCue::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	NiagaraComponent->OnSystemFinished.AddUniqueDynamic(this, &AAbilityCue::OnNiagaraFinished);
	AudioComponent->OnAudioFinished.AddUniqueDynamic(this, &AAbilityCue::OnAudioFinished);
	ParticleSystemComponent->OnSystemFinished.AddUniqueDynamic(this, &AAbilityCue::OnParticleFinished);
	OnDestroyed.AddUniqueDynamic(this, &AAbilityCue::OnDestroyedEvent);
}

void AAbilityCue::OnOwnerDeadEvent(AActor* DeadBy)
{
	UE_LOGFMT(LogCue, Log, "오너 사망으로 큐 제거됨 : {0},{1}", GetActorNameOrLabel(), CueTag.ToString());
	Destroy();
}

// Called when the game starts or when spawned
void AAbilityCue::BeginPlay()
{
	Super::BeginPlay();

	UE_LOGFMT(LogCue, Log, "큐 시작됨 : {0},{1}", GetActorNameOrLabel(), CueTag.ToString());
	
	ParticleSystemComponent->Deactivate();
	NiagaraComponent->Deactivate();
	AudioComponent->Deactivate();
}

void AAbilityCue::PlayCue_Implementation(const FAbilityCueInformation& CueInformation)
{
	DeactivateCue();

	UE_LOGFMT(LogTemp, Log, "{0} {1}", __FUNCTION__, __LINE__);
	bReusable = CueInformation.bReusable;
	CueTag = CueInformation.CueTag;

	switch (CueInformation.AbilityCueType)
	{
	case EAbilityCueType::AtLocation:
		UE_LOGFMT(LogCue, Log, "좌표 : {0}", CueInformation.SpawnLocation.ToString());
		SetActorLocation(CueInformation.SpawnLocation);
		/*AttachToActor(CueInformation.AttachTarget.Get(),
		              FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));*/
		break;
	case EAbilityCueType::AttachToTarget:
		if (CueInformation.AttachTarget.IsValid())
		{
			UE_LOGFMT(LogTemp, Log, "{0} {1}", __FUNCTION__, __LINE__);
			AttachToActor(CueInformation.AttachTarget.Get(),
			              FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld,
			                                        EAttachmentRule::KeepWorld, true));
		}
		else
		{
			UE_LOGFMT(LogTemp, Error, "{0} {1}", __FUNCTION__, __LINE__);
		}
		break;
	case EAbilityCueType::AttachToSocket:
		if (CueInformation.AttachTarget.IsValid())
		{
			UE_LOGFMT(LogTemp, Log, "큐 붙임 : {0} {1}", __FUNCTION__, __LINE__);
			AttachToComponent(Cast<ABaseCharacter>(CueInformation.AttachTarget.Get())->GetMesh(),
			                  FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			                  CueInformation.SocketName);

			/*UE_LOGFMT(LogTemp, Log, "{0} {1}", __FUNCTION__, __LINE__);
			NiagaraComponent->AttachToComponent(Cast<ABaseCharacter>(CueInformation.AttachTarget.Get())->GetMesh(),
			                                    FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			                                    CueInformation.SocketName);

			ParticleSystemComponent->AttachToComponent(
				Cast<ABaseCharacter>(CueInformation.AttachTarget.Get())->GetMesh(),
				FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				CueInformation.SocketName);*/
		}
		else
		{
			UE_LOGFMT(LogTemp, Error, "{0} {1}", __FUNCTION__, __LINE__);
		}
		break;
	case EAbilityCueType::AtOwnerLocation:
		if (GetOwner())
		{
			SetActorLocation(GetOwner()->GetActorLocation());
			/*AttachToActor(CueInformation.AttachTarget.Get(),
			              FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));*/
		}
		break;
	default: ;
	}

	ActivateCue();
}

void AAbilityCue::DeactivateCue() const
{
	ParticleSystemComponent->Deactivate();
	NiagaraComponent->Deactivate();
	AudioComponent->Stop();
	AudioComponent->Deactivate();
}

void AAbilityCue::ActivateCue()
{
	ParticleSystemComponent->Activate(true);
	NiagaraComponent->Activate(true);
	AudioComponent->Activate(true);
	AudioComponent->Play(0);
}

void AAbilityCue::SetNiagaraSourceActor()
{
	if(auto character = GetOwner<ABaseCharacter>())
	{
		NiagaraComponent->AttachToComponent(character->GetMesh(),FAttachmentTransformRules(EAttachmentRule::SnapToTarget,true));
		TArray<AActor*> actors;
		actors.Emplace(character);
		UNiagaraDIRigidMeshCollisionFunctionLibrary::SetSourceActors(NiagaraComponent,NAME_None,actors);
	}
}


void AAbilityCue::OnDestroyedEvent(AActor* DestroyedActor)
{
	UE_LOGFMT(LogTemp, Warning, "큐 제거됨 : {0}", CueTag.ToString());
	if (auto character = GetOwner<ABaseCharacter>())
	{
		character->GetAbilityComponent()->OnRemoveCue.Broadcast(CueTag);
	}
}

void AAbilityCue::Destroyed()
{
	DeactivateCue();

	Super::Destroyed();
}

void AAbilityCue_SourceActorBased::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAbilityCue_SourceActorBased::ActivateCue()
{
	SetNiagaraSourceActor();
	NiagaraComponent->ResetSystem();
	Super::ActivateCue();
};
