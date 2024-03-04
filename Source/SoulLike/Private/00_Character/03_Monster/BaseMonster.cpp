// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/BaseMonster.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "00_Character/03_Monster/00_Controller/MonsterAIController.h"
#include "00_Character/03_Monster/01_Component/ItemDropComponent.h"
#include "01_GameMode/SoulLikeGameMode.h"
#include "02_Ability/AbilityEffect.h"
#include "03_Widget/02_Monster/HealthBarWidget.h"
#include "97_Interface/BossMonsterInterface.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Logging/StructuredLog.h"
#include "Perception/AISense_Damage.h"

DEFINE_LOG_CATEGORY(LogMonster)

ABaseMonster::ABaseMonster()
{
	GetCapsuleComponent()->SetCollisionProfileName("Pawn");
	GetMesh()->SetCollisionProfileName("MonsterMesh");
	GetMesh()->SetBodyNotifyRigidBodyCollision(true);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickMontagesWhenNotRendered;

	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComponent->SetupAttachment(GetMesh(), "HealthBar");

	HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidgetComponent->SetDrawSize(FVector2D(125, 5));

	static ConstructorHelpers::FClassFinder<UUserWidget> widget(TEXT(
		"/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/02_Widget/UMG_HealthBar.UMG_HealthBar_C'"));
	if (widget.Succeeded())
	{
		HealthBarWidgetComponent->SetWidgetClass(widget.Class);
	}


	ItemDropComponent = CreateDefaultSubobject<UItemDropComponent>(TEXT("ItemDropComponent"));

	//이 두개의 옵션으로 AI가 MoveTo 함수 등으로 네비메시에서 이동할 때, 보간없이 휙휙 도는 현상이 제거됩니다.
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	AIControllerClass = AMonsterAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	/*GetArrowComponent()->ArrowColor = FColor::Blue;
	GetArrowComponent()->ArrowSize = 5.f;*/
}

void ABaseMonster::K2_SpawnAlly_Implementation(int32 SpawnCount)
{
	OnFinishSpawnAllyEvent.Broadcast(Cast<UBehaviorTreeComponent>(GetController<AAIController>()->GetBrainComponent()),
	                                 EBTNodeResult::Succeeded);
}

void ABaseMonster::SpawnAlly(int32 SpawnCount, const FOnFinishSpawnAlly& OnFinishSpawnAlly)
{
	OnFinishSpawnAllyEvent = OnFinishSpawnAlly;
	K2_SpawnAlly(SpawnCount);
	/*OnFinishSpawnAlly.Broadcast(Cast<UBehaviorTreeComponent>(GetController<AAIController>()->GetBrainComponent()),EBTNodeResult::Succeeded);*/
}

void ABaseMonster::ResetMonster(const FTransform& DefaultTr)
{
	//블랙보드 초기화
	if (auto aiCon = Cast<AMonsterAIController>(GetController()))
	{
		if (auto brain = aiCon->GetBrainComponent())
		{
			brain->StopLogic("Reset");
			aiCon->ResetBlackboard();
		}

		if (MonsterDataAsset->bStartBehaviorTreeImmediately)
		{
			aiCon->StartBehavior();
		}
	}

	//사망인 경우 사망 해제
	if (IsDead())
	{
		CharacterState = ECharacterState::NORMAL;
		GetMesh()->SetSimulatePhysics(false);
		GetCapsuleComponent()->SetCollisionProfileName("Pawn");
		GetMesh()->SetCollisionProfileName("MonsterMesh");
		GetWorldTimerManager().ClearTimer(DeadTimerHandle);
	}

	//어트리뷰트 초기화
	if (AttributeComponent)
	{
		AttributeComponent->InitAttributes();
	}

	UE_LOGFMT(LogTemp, Log, "{0}의 트렌스폼을 {1}로 되돌립니다.", GetActorNameOrLabel(), DefaultTr.ToString());
	//위치 복구
	SetActorTransform(DefaultTr);
}

void ABaseMonster::BeginPlay()
{
	Super::BeginPlay();

	//휴식시 되살리기 위해 저장합니다.
	if (MonsterDataAsset->bRespawnWhenPlayerRest)
	{
		if (auto gameMode = Cast<ASoulLikeGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			gameMode->AddMonster(this, FMonsterRespawnInfo(this, GetNameSafe(this), GetClass(), GetActorTransform()));
		}
	}
}

void ABaseMonster::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (MonsterDataAsset == nullptr)
	{
		UE_LOGFMT(LogMonster, Error, "몬스터 클래스 내부 변수인 MonsterDataAsset가 비었습니다. 실행하면 큰 문제가 생길 것입니다.");
	}

	if (HealthBarWidgetComponent)
	{
		HealthBarWidgetComponent->InitWidget();
		if (HealthBarWidgetComponent->GetUserWidgetObject())
		{
			HealthBarWidgetComponent->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Collapsed);
		}

		HealthBarWidgetComponent->AttachToComponent(
			GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), "HealthBar");
	}
	AttributeComponent->OnChangeHPValue.AddUniqueDynamic(this, &ABaseMonster::UpdateHealthBar);

	if (auto aiCon = Cast<AMonsterAIController>(GetController()))
	{
		aiCon->OverrideSightConfig(MonsterDataAsset);
	}

	if (UKismetSystemLibrary::DoesImplementInterface(this, UBossMonsterInterface::StaticClass()))
	{
		OnDead.AddUniqueDynamic(this, &ABaseMonster::OnDeadBossEvent);
	}

	OnDestroyed.AddUniqueDynamic(this,&ABaseMonster::OnDestroyedEvent);
}

void ABaseMonster::OnDestroyedEvent(AActor* DestroyedActor)
{
	if (UKismetSystemLibrary::DoesImplementInterface(this, UBossMonsterInterface::StaticClass()))
	{
		IBossMonsterInterface::Execute_RemoveBossWidget(this, this, UGameplayStatics::GetPlayerCharacter(this,0));
	}
}


void ABaseMonster::UpdateHealthBar(float Value, float MaxValue)
{
	if (HealthBarWidgetComponent->IsValidLowLevel() == false)
	{
		return;
	}

	if (IsDead())
	{
		return;
	}

	if (HealthBarWidgetComponent->GetUserWidgetObject())
	{
		if (auto widget = Cast<UHealthBarWidget>(HealthBarWidgetComponent->GetUserWidgetObject()))
		{
			widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			FTimerDelegate hiddenTimerDel = FTimerDelegate::CreateUObject(
				widget, &UUserWidget::SetVisibility, ESlateVisibility::Collapsed);
			GetWorldTimerManager().SetTimer(HealthBarVisibleTimerHandle, hiddenTimerDel, 3.f, false);

			widget->UpdateProgress(Value, MaxValue);
		}
		else
		{
			UE_LOGFMT(LogTemp, Error, "{0} {1}", __FUNCTION__, __LINE__);
		}
	}
	else
	{
		UE_LOGFMT(LogTemp, Error, "{0} {1}", __FUNCTION__, __LINE__);
	}
}

void ABaseMonster::OnDeadEvent(AActor* Who, AActor* DeadBy)
{
	Super::OnDeadEvent(Who, DeadBy);
	
	if (ItemDropComponent)
	{
		if (DeadBy != nullptr)
		{
			if (GetController<AAIController>()->GetBrainComponent() != nullptr)
			{
				GetController<AAIController>()->GetBrainComponent()->StopLogic("Dead");
			}

			GetCharacterMovement()->StopMovementImmediately();

			GetCapsuleComponent()->SetCollisionProfileName("Spectator");
			GetMesh()->SetCollisionProfileName("Spectator");


			if (DeadBy->IsA<ABaseCharacter>())
			{
				if (!UKismetSystemLibrary::DoesImplementInterface(this, UBossMonsterInterface::StaticClass()))
				{
					ItemDropComponent->DropItem(Cast<ABaseCharacter>(DeadBy));
				}

				ItemDropComponent->GiveExp(Cast<ABaseCharacter>(DeadBy));
			}

			//캐릭터 특성 적용
			if (DeadBy->IsA<APlayerCharacter>())
			{
				Cast<APlayerCharacter>(DeadBy)->GetAbilityTalentComponent()->BroadcastOnKillMonster(
					Cast<ABaseCharacter>(DeadBy), Cast<ABaseCharacter>(Who));
			}

			auto randIndex = FMath::RandRange(0,DeadMontages.Num()-1);
			if(DeadMontages.IsValidIndex(randIndex))
			{
				DeadMontage = DeadMontages[randIndex];
			}
			
			if (DeadMontage != nullptr && GetMesh()->GetAnimInstance() != nullptr)
			{
				if(MonsterDataAsset->RagdollPhysics)
				{
					GetMesh()->SetPhysicsAsset(MonsterDataAsset->RagdollPhysics);
				}
				
				GetMesh()->GetAnimInstance()->OnMontageEnded.AddUniqueDynamic(
					this, &ABaseMonster::OnDeadMontageEndedEvent);
				PlayAnimMontage(DeadMontage);
			}
			else
			{
				GetMesh()->SetSimulatePhysics(true);

				FTimerDelegate deadTimerDel = FTimerDelegate::CreateUObject(this, &ABaseMonster::K2_DestroyActor);
				GetWorldTimerManager().SetTimer(DeadTimerHandle, deadTimerDel, 60.f, false);
			}
		}
	}
}

void ABaseMonster::OnDeadBossEvent(AActor* Who, AActor* DeadBy)
{
	if (UKismetSystemLibrary::DoesImplementInterface(this, UBossMonsterInterface::StaticClass()))
	{
		IBossMonsterInterface::Execute_RemoveBossWidget(this, this, DeadBy);
		ItemDropComponent->BossDropItem(Cast<ABaseCharacter>(DeadBy));
	}
}

void ABaseMonster::OnDeadMontageEndedEvent(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == DeadMontage)
	{
		if (auto interface = Cast<IBossMonsterInterface>(this))
		{
			Destroy();
			return;
		}
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionProfileName("Ragdoll");
		GetMesh()->SetSimulatePhysics(true);
	}
}

void ABaseMonster::SetMonsterState(EMonsterState NewState)
{
	MonsterState = NewState;
	//블랙보드값 업데이트
	if(auto aiCon = Cast<AAIController>(GetController()))
	{
		if(auto bbComp = UAIBlueprintHelperLibrary::GetBlackboard(this))
		{
			bbComp->SetValueAsEnum("MonsterState",static_cast<uint8>(MonsterState));
		}
	}
}

bool ABaseMonster::ShouldForceCombatState()
{
	if(MonsterDataAsset)
	{
		return MonsterDataAsset->bForceCombatStateWhenPerceptedTarget;
	}
	return false;
}

void ABaseMonster::IncreaseStunIntensity(const FAttributeEffect& Effect,
                                         UAbilityEffectAdditionalInformation* AdditionalInformation)
{
	if (!MonsterDataAsset->bUseStun)
	{
		return;
	}

	if (auto damagedBy = Cast<ABaseCharacter>(AdditionalInformation->HitBy))
	{
		//경직치를 누적시킵니다. 피해를 준 대상의 의지에 따라 추가증가시킵니다.
		StunIntensity += Effect.ApplyValue * 0.1f * (damagedBy->GetAttributeComponent()->GetWillpower());
		if (StunIntensity >= MonsterDataAsset->MaxStunIntensity)
		{
			StunIntensity = 0;
			if (auto aiCon = GetController<AMonsterAIController>())
			{
				UE_LOGFMT(LogAICon,Warning,"{0}가 스턴됨!!!",GetActorNameOrLabel());
				CharacterState = ECharacterState::STUN;

				FTimerHandle aiResumeTimerHandle;
				FTimerDelegate aiResumeTimerDel = FTimerDelegate::CreateLambda([this]()
				{
					CharacterState = ECharacterState::NORMAL;
				});

				const auto& time = GetMesh()->GetAnimInstance()->Montage_Play(
					MonsterDataAsset->StunMontage, 1.f, EMontagePlayReturnType::Duration);
				GetWorldTimerManager().SetTimer(aiResumeTimerHandle, aiResumeTimerDel, time, false);
			}
		}
	}
}


void ABaseMonster::OnRemoveAttributeEffectAdditionalInformationEvent_Implementation(const FAttributeEffect& Effect,
	UAbilityEffectAdditionalInformation*
	AdditionalInformation, float DeltaTime)
{
	if (AdditionalInformation != nullptr)
	{
		UE_LOGFMT(LogMonster, Log, "{0}이(가) {1}에게 입은 피해량 : {2}", GetActorNameOrLabel(),
		          AdditionalInformation->HitBy->GetActorNameOrLabel(), Effect.ApplyValue * DeltaTime);
	
		if (Effect.Attribute == EAttributeType::HP)
		{
			//사망에 이르렀니?
			if (AttributeComponent->GetHP() <= 0)
			{
				if (IsDead() == false)
				{
					/*CharacterState = ECharacterState::DEAD;*/
					OnDead.Broadcast(this, AdditionalInformation->HitBy.Get());
				}
				return;
			}


			if (auto aiCon = GetController<AMonsterAIController>())
			{
				UAISense_Damage::ReportDamageEvent(this, this, AdditionalInformation->HitBy.Get(), Effect.ApplyValue,
				                                   GetActorLocation(), AdditionalInformation->Hit.Location);
			}

			IncreaseStunIntensity(Effect, AdditionalInformation);
			TriggerHitAnimation(AdditionalInformation);
		}
	}
}

void ABaseMonster::SetRandomRotationYaw()
{
	AddActorLocalRotation(FRotator(0, FMath::RandRange(-360, 360), 0));
}

void ABaseMonster::TriggerHitAnimation_Implementation(UAbilityEffectAdditionalInformation* AdditionalInformation)
{
	Super::TriggerHitAnimation_Implementation(AdditionalInformation);
	
	if (IsMighty() == false)
	{
		if (auto aicon = Cast<AMonsterAIController>(GetController()))
		{
			UE_LOGFMT(LogTemp,Warning,"트리거 히트 애니메이션 : {0}",GetActorNameOrLabel());
			if (aicon->IsBehaviorTreeRunning())
			{
				Cast<UBehaviorTreeComponent>(aicon->GetBrainComponent())->StopTree();
			}
		}
	}
}
