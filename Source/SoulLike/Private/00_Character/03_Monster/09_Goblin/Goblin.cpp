// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/09_Goblin/Goblin.h"

#include "NavigationInvokerComponent.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "00_Character/01_Component/FootStepComponent.h"
#include "00_Character/01_Component/UROComponent.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "00_Character/03_Monster/01_Component/ItemDropComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"


AGoblin::AGoblin()
{
	HelmetComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HelmetComponent"));
	HelmetComponent->SetupAttachment(GetMesh());

	SkirtComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkirtComponent"));
	SkirtComponent->SetupAttachment(GetMesh());

	LegComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegComponent"));
	LegComponent->SetupAttachment(GetMesh());

	BracerComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BracerComponent"));
	BracerComponent->SetupAttachment(GetMesh());

	BodyComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyComponent"));
	BodyComponent->SetupAttachment(GetMesh());

	WeaponComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponComponent"));
	WeaponComponent->SetupAttachment(GetMesh());


	HelmetComponent->SetLeaderPoseComponent(GetMesh());
	SkirtComponent->SetLeaderPoseComponent(GetMesh());
	LegComponent->SetLeaderPoseComponent(GetMesh());
	BracerComponent->SetLeaderPoseComponent(GetMesh());
	BodyComponent->SetLeaderPoseComponent(GetMesh());
}

AGoblinRogue::AGoblinRogue()
{
	if (WeaponComponent)
	{
		WeaponComponent->SetupAttachment(GetMesh(), "Sword");
	}
}

AGoblinWarrior::AGoblinWarrior()
{
	if (WeaponComponent)
	{
		WeaponComponent->SetupAttachment(GetMesh(), "Sword");
	}

	ShieldComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldComponent"));
	ShieldComponent->SetupAttachment(GetMesh(), "Shield");
}

AGoblinMage::AGoblinMage()
{
	if (WeaponComponent)
	{
		WeaponComponent->SetupAttachment(GetMesh(), "Staff");
	}
}

AGoblinCrystallized::AGoblinCrystallized()
{
	CrystallizedArmComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CrystallizedArmComponent"));
	CrystallizedArmComponent->SetupAttachment(GetMesh());

	CrystallizedBodyComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CrystallizedBodyComponent"));
	CrystallizedBodyComponent->SetupAttachment(GetMesh());

	CrystallizedLegBottomComponent = CreateDefaultSubobject<USkeletalMeshComponent>(
		TEXT("CrystallizedLegBottomComponent"));
	CrystallizedLegBottomComponent->SetupAttachment(GetMesh());

	CrystallizedLegTopComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CrystallizedLegTopComponent"));
	CrystallizedLegTopComponent->SetupAttachment(GetMesh());

	CrystallizedHeadComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CrystallizedHeadComponent"));
	CrystallizedHeadComponent->SetupAttachment(GetMesh());

	CrystallizedShoulderComponent = CreateDefaultSubobject<USkeletalMeshComponent>(
		TEXT("CrystallizedShoulderComponent"));
	CrystallizedShoulderComponent->SetupAttachment(GetMesh());

	CrystallizedArmComponent->SetLeaderPoseComponent(GetMesh());
	CrystallizedBodyComponent->SetLeaderPoseComponent(GetMesh());
	CrystallizedLegBottomComponent->SetLeaderPoseComponent(GetMesh());
	CrystallizedLegTopComponent->SetLeaderPoseComponent(GetMesh());
	CrystallizedHeadComponent->SetLeaderPoseComponent(GetMesh());
	CrystallizedShoulderComponent->SetLeaderPoseComponent(GetMesh());
}

AGoblinBattleMage::AGoblinBattleMage()
{
	if (WeaponComponent)
	{
		WeaponComponent->SetupAttachment(GetMesh(), "Sword");
	}

	StaffComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaffComponent"));
	StaffComponent->SetupAttachment(GetMesh(), "StaffMirror");
}

AGoblinSwarm::AGoblinSwarm()
{
	SpriteComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	RootComponent = SpriteComponent;

	if(HealthBarWidgetComponent)
	{
		HealthBarWidgetComponent->DestroyComponent();
	}
	
	if(GetMesh())
	{
		GetMesh()->DestroyComponent();
	}

	if(FootStepComponent)
	{
		FootStepComponent->DestroyComponent();
	}

	if(NavigationInvokerComponent)
	{
		NavigationInvokerComponent->DestroyComponent();
	}

	if(UROComponent)
	{
		UROComponent->DestroyComponent();
	}
}

void AGoblinSwarm::BeginPlay()
{
	Super::BeginPlay();
	SpawnPoints = GetComponentsByTag(USceneComponent::StaticClass(),TEXT("SpawnPoint"));
}

void AGoblinSwarm::SpawnGoblin(TArray<TSubclassOf<AGoblin>> SpawnClass, int32 SpawnCount)
{
	for (auto i = 0; i < SpawnCount; i++)
	{
		const auto randomPoint = Cast<USceneComponent>(SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)]);

		const auto& index = FMath::RandRange(0, SpawnClass.Num() - 1);
		const auto rand = SpawnClass[index];

		FActorSpawnParameters spawnParam;
		spawnParam.Instigator = this;
		spawnParam.Owner = this;
		spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		if(auto goblin = GetWorld()->SpawnActor<AGoblin>(rand, randomPoint->GetComponentLocation(), FRotator::ZeroRotator, spawnParam))
		{
			Goblins.Add(goblin);
			goblin->OnDead.AddUniqueDynamic(this,&AGoblinSwarm::OnGoblinDeadEvent);
			goblin->GetItemDropComponent()->SetIgnoreDropExp();
			goblin->GetItemDropComponent()->SetIgnoreDropItem();
			
			
			if(auto aiCon = Cast<AMonsterAIController>(goblin->GetController())){

				aiCon->StartBehavior();
				
				if(auto bbComp = aiCon->GetBlackboardComponent())
				{
					bbComp->SetValueAsObject("Target",UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
				}
			}
		}
	}

	if(auto bbComp = Cast<AAIController>(GetController())->GetBlackboardComponent())
	{
		bbComp->SetValueAsInt("CurrentWave",bbComp->GetValueAsInt("CurrentWave") + 1);
	}
}

void AGoblinSwarm::SpawnGoblinElite()
{
	FActorSpawnParameters spawnParam;
	spawnParam.Instigator = this;
	spawnParam.Owner = this;
	spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	
	if(auto goblin = GetWorld()->SpawnActor<AGoblin>(GoblinEliteClass,GetActorLocation(),FRotator::ZeroRotator,spawnParam))
	{
		goblin->OnDead.AddUniqueDynamic(this,&AGoblinSwarm::OnGoblinDeadEvent);
		goblin->GetItemDropComponent()->SetIgnoreDropExp();
		goblin->GetItemDropComponent()->SetIgnoreDropItem();
		
		if(auto aiCon = Cast<AMonsterAIController>(goblin->GetController()))
		{
			aiCon->StartBehavior();
			aiCon->GetBlackboardComponent()->SetValueAsObject("Target",UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
		}
	}
	
}

void AGoblinSwarm::OnGoblinDeadEvent(AActor* Who, AActor* DeadBy)
{
	Goblins.Remove(Who);

	FTimerHandle destroyTimerHandle;
	FTimerDelegate destroyTimerDel = FTimerDelegate::CreateLambda([Who]()
	{
		Who->Destroy();
	});
	GetWorldTimerManager().SetTimer(destroyTimerHandle,destroyTimerDel,5.f,false);
	
	if(Goblins.IsEmpty())
	{
		//피해를 입습니다.
		AbilityComponent->K2_ApplyEffect(DamageEffect,DeadBy,FOnEffectExpired());

		if(AttributeComponent->GetHP()<=0)
		{
			OnDead.Broadcast(this,DeadBy);
		}
	}
}

void AGoblinSwarm::OnDeadEvent(AActor* Who, AActor* DeadBy)
{
	UE_LOGFMT(LogTemp, Log, "{0} 사망 이벤트 호출", GetActorNameOrLabel());
	if (CharacterState != ECharacterState::DEAD)
	{
		UGameplayStatics::PlaySoundAtLocation(this,DeadCue,Who->GetActorLocation());
		UE_LOGFMT(LogTemp, Log, "{0} 사망 이벤트 / 정보 설정", GetActorNameOrLabel());
		SetCharacterState(ECharacterState::DEAD);
		if (AbilityComponent)
		{
			AbilityComponent->ClearReusableCue();
		}

	}
}
