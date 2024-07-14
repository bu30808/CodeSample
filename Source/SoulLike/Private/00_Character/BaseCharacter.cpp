// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/BaseCharacter.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/00_Player/00_Controller/00_Component/InputHandlerComponent.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/01_Component/AnimationHelperComponent.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "00_Character/01_Component/FootStepComponent.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "02_Ability/AbilityEffect.h"
#include "96_Library/AbilityHelperLibrary.h"
#include "96_Library/MathHelperLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/PhysicsAsset.h"
DEFINE_LOG_CATEGORY(LogCharacter)

// Sets default values
ABaseCharacter::ABaseCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.TickInterval = 0.1f;

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));
	AbilityComponent = CreateDefaultSubobject<UAbilityComponent>(TEXT("AbilityComponent"));
	FootStepComponent = CreateDefaultSubobject<UFootStepComponent>(TEXT("FootStepComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	AnimationHelperComponent = CreateDefaultSubobject<UAnimationHelperComponent>(TEXT("AnimationHelperComponent"));
	/*NavigationInvokerComponent = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavigationInvokerComponent"));*/

	DeadDissolveTimeLineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("DeadDissolveTimeLine"));
	AbilityTalentComponent = CreateDefaultSubobject<UAbilityTalentComponent>(TEXT("AbilityTalentComponent"));


	{
		static ConstructorHelpers::FClassFinder<UAbilityEffect> fallDamageClass(TEXT(
			"/Script/Engine.Blueprint'/Game/Blueprints/00_Character/02_CommonAbility/AE_FallDamage.AE_FallDamage_C'"));
		if (fallDamageClass.Succeeded())
		{
			FallDamageEffectObject = fallDamageClass.Class;
		}
	}

	{
		static ConstructorHelpers::FObjectFinder<USoundBase> deadCue(TEXT(
			"/Script/Engine.SoundCue'/Game/CommonResource/Sound/ondead_cue.ondead_cue'"));
		if (deadCue.Succeeded())
		{
			DeadCue = deadCue.Object;
		}
	}
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	CheckTerrainIsLoaded();

	//속성 초기화
	AttributeComponent->InitAttributes();
	//피격 이벤트 바인드
	AttributeComponent->OnRemoveAttributeEffectAdditionalInformation.AddUniqueDynamic(
		this, &ABaseCharacter::OnRemoveAttributeEffectAdditionalInformationEvent);

	GiveDefaultAbility();
	ActivateDefaultAbility();

	if (!IsA<APlayerCharacter>())
	{
		GiveDefaultItem();
	}
}

void ABaseCharacter::CheckFallDeath()
{
	if (!bShouldCheckFallDeath)
	{
		return;
	}

	if (GetCharacterMovement()->IsFalling() && !IsDead())
	{
		FHitResult hit;
		if (!UKismetSystemLibrary::CapsuleTraceSingle(this, GetActorLocation(),
		                                              GetActorLocation() + GetActorUpVector() * (-1) *
		                                              FallDeathDistance,
		                                              GetCapsuleComponent()->GetScaledCapsuleRadius(),
		                                              GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),
		                                              UEngineTypes::ConvertToTraceType(ECC_WorldStatic), false,
		                                              TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, hit, true))
		{
			UE_LOGFMT(LogCharacter, Warning, "아래 착지할 곳이 없습니다!! 사망처리 합니다.");
			AttributeComponent->SetHP(0);
			OnDead.Broadcast(this, this,EDeadReason::DiedFromFalling);
		}
		else
		{
		
			if (bStartFallDamageProcess == false)
			{
				bStartFallDamageProcess = true;
				//떨어지기 시작한 위치를 기록합니다.
				FallingStartLocation = GetActorLocation();
				UE_LOGFMT(LogCharacter, Log, "낙하 시작 기록");
			}
		}
	}
	else
	{
		if (bStartFallDamageProcess)
		{
			if (GetCharacterMovement()->IsMovingOnGround())
			{
				//착지하면  착지한 위치와의 Z축 거리를 계산합니다.
				FVector landLocation = GetActorLocation();

				landLocation.X = 0;
				landLocation.Y = 0;

				FallingStartLocation.X = 0;
				FallingStartLocation.Y = 0;

				const float& distance = (landLocation - FallingStartLocation).Length();
				//거리가 일정 이상일 때, 거리와 비례해서 체력을 감소시킵니다.
				//떨어져 죽을수 있는 거리의 반이상의 높이에서 떨어져 착지한 경우
				const float fallDamageStartLength = FallDeathDistance * 0.5f;
				UE_LOGFMT(LogCharacter, Log, "낙하 거리 : {0}, 낙하 피해 가능 최소 거리 : {1}", distance, fallDamageStartLength);
				if (distance >= fallDamageStartLength)
				{
					float gapPercent = (distance - fallDamageStartLength) / fallDamageStartLength;
					UE_LOGFMT(LogCharacter, Log, "낙하 피해 비율 : {0}", gapPercent);

					if (FallDamageEffectObject == nullptr)
					{
						ensure(FallDamageEffectObject);
						return;
					}

					if (const auto fallDamageObject = NewObject<UFallingDamageData>())
					{
						fallDamageObject->FallingDamageRatio = gapPercent;
						AbilityComponent->K2_ApplyEffect(FallDamageEffectObject, this, FOnEffectExpired(),
						                                 fallDamageObject);
					}
				}


				bStartFallDamageProcess = false;
			}
		}
	}
}

void ABaseCharacter::CheckTerrainIsLoaded()
{
	GetCapsuleComponent()->SetEnableGravity(false);

	// 지형의 콜리전 상태를 확인합니다.
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), GetActorUpVector() * -1000.f,
	                                                 ECC_Visibility, QueryParams);

	if (bHit && HitResult.GetActor()->IsValidLowLevel())
	{
		GetCapsuleComponent()->SetEnableGravity(true);
	}
	else
	{
		// 1초 후에 다시 시도합니다.
		GetWorldTimerManager().SetTimerForNextTick(this, &ABaseCharacter::CheckTerrainIsLoaded);
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OnDead.AddUniqueDynamic(this, &ABaseCharacter::OnDeadEvent);
	if (AnimationHelperComponent)
	{
		AnimationHelperComponent->OnTriggerHitAnimationEnter.AddUniqueDynamic(
			this, &ABaseCharacter::OnTriggerHitAnimationEnterEvent);
		AnimationHelperComponent->OnTriggerHitAnimationExit.AddUniqueDynamic(
			this, &ABaseCharacter::OnTriggerHitAnimationExitEvent);
	}

	if (GetMesh() != nullptr)
	{
		OriginalMaterials = GetMesh()->GetMaterials();
	}
}


void ABaseCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (AttributeComponent)
	{
		AttributeComponent->InitAttributes();
	}
}

void ABaseCharacter::ShowGotHitCue(FVector HitLocation)
{
	if (AbilityComponent)
	{
		for (auto& c : GotHitCues)
		{
			c.SpawnLocation = HitLocation;
		}

		AbilityComponent->ApplyCues(GotHitCues);
	}
}

void ABaseCharacter::GiveDefaultAbility()
{
	for (auto ab : DefaultAbility)
	{
		AbilityComponent->GiveAbility(ab);
	}
}

void ABaseCharacter::ActivateDefaultAbility()
{
	UE_LOGFMT(LogCharacter, Log, "{0} {1} : 기본 어빌리티 사용", __FUNCTION__, __LINE__);
	for (auto tag : DefaultUseAbilityTags)
	{
		AbilityComponent->ActivateAbility(tag, this);
	}
}

void ABaseCharacter::GiveDefaultItem()
{
	for (auto item : DefaultItem)
	{
		InventoryComponent->SpawnAndAddItem(item);
	}
}

void ABaseCharacter::OnRemoveAttributeEffectAdditionalInformationEvent_Implementation(const FAttributeEffect& Effect,
	UAbilityEffectAdditionalInformation*
	AdditionalInformation, float DeltaTime)
{
	if (AdditionalInformation != nullptr && IsValidLowLevel())
	{
		if (Effect.Attribute == EAttributeType::HP)
		{
			if (AdditionalInformation->HitBy != nullptr)
			{
				UE_LOGFMT(LogCharacter, Log, "ABaseCharacter {0}이(가) {1}에게 입은 피해량 : {2} 111111111111",
				          GetActorNameOrLabel(),
				          AdditionalInformation->HitBy->GetActorNameOrLabel(), Effect.ApplyValue * DeltaTime);
			}

			//사망에 이르렀니?
			if (AttributeComponent->GetHP() <= 0)
			{
				if (IsDead() == false)
				{
					OnDead.Broadcast(this, AdditionalInformation->HitBy.Get(),EDeadReason::DiedFromHPDepletion);
					/*CharacterState = ECharacterState::DEAD;*/
				}
				UE_LOGFMT(LogTemp, Log, "체력이 0 이하라 다음 코드를 스킵합니다.");
				return;
			}

			//슈퍼아머 상태가 아닐경우, 애니메이션 관련 설정을 합니다.
			if (IsMighty() == false)
			{
				if (AdditionalInformation->bTriggerHitAnimation)
				{
					//TODO 강인도 수치를 조사하여 만약 몽타주가 재생중이라면 멈춰버려야 합니다.
					//강인도 수치가 피해량*비율 보다 큰지 확인합니다. 작다면 피격 애니메이션을 재생할 설정을 합니다.
					if (ShouldSkipHitAnimation(Effect.ApplyValue, 0.7f) == false)
					{
						UE_LOGFMT(LogTemp, Log, "최대체력대비 피해 비율 : {0}",
						          Effect.ApplyValue / AttributeComponent->GetMaxHP());
						//70%이상의 큰 피해를 받은 경우
						if (Effect.ApplyValue / AttributeComponent->GetMaxHP() > 0.7f)
						{
							//넘어지든가 하면 됨.
							AbilityComponent->ActivateAbility(
								FGameplayTag::RequestGameplayTag("Common.Passive.Knockdown"),
								this,
								/*nullptr, */nullptr);
							return;
						}

						TriggerHitAnimation(AdditionalInformation);
					}
					else
					{
						UE_LOGFMT(LogTemp, Warning, "히트 애니메이션 스킵, 강인도 수치가 높습니다.",);
					}
				}
			}
		}
	}
}

bool ABaseCharacter::IsMighty()
{
	const bool& result = AbilityComponent->HasEffectTag(MIGHTY_EFFECT_TAG);
	if (result)
	{
		UE_LOGFMT(LogCharacter, Warning, "이 캐릭터는 슈퍼아머 상태입니다 : {0}", GetNameSafe(this));
	}
	return result;
}

bool ABaseCharacter::ShouldSkipHitAnimation(float Damage, float SkipThreshold)
{
	UE_LOGFMT(LogTemp, Log, "강인도 체크 : {0}, 기준값 : {1},강인도 수치와 기준값의 비교 결과 : {2}", AttributeComponent->GetEndurance(),
	          Damage*SkipThreshold, AttributeComponent->GetEndurance() > Damage * SkipThreshold);
	return AttributeComponent->GetEndurance() > Damage * SkipThreshold;
}

void ABaseCharacter::OnUpdateDeadDissolveTimeLine(float Value)
{
	const FName& param = "Percent";
	for (auto iter : BodyMaterialInstance)
	{
		iter->SetScalarParameterValue(param, Value);
	}
}

void ABaseCharacter::OnFinishDissolveTimeLine()
{
	
}

void ABaseCharacter::StopDeadDissolve()
{
	UE_LOGFMT(LogCharacter,Log,"사망 디졸브 제거");
	const FName& param = "Percent";
	for (auto iter : BodyMaterialInstance)
	{
		iter->SetScalarParameterValue(param, 1);
	}

	AnimationHelperComponent->DestroyDissolveParticle();
}

void ABaseCharacter::PlayDeadAnimationSequence()
{
	AnimationHelperComponent->PlayDeadAnimationSequence();
}


void ABaseCharacter::OnDeadEvent(AActor* Who, AActor* DeadBy, EDeadReason DeadReason)
{
	UE_LOGFMT(LogTemp, Log, "{0} 사망 이벤트 호출", GetActorNameOrLabel());
	if (CharacterState != ECharacterState::DEAD)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeadCue, Who->GetActorLocation());
		SetCharacterState(ECharacterState::DEAD);
		GetCapsuleComponent()->SetCollisionProfileName("Spectator");
		GetMesh()->SetCollisionProfileName("Spectator");

		UE_LOGFMT(LogTemp, Log, "{0} 사망 이벤트 / 정보 설정 / 사망 원인 {1}", GetActorNameOrLabel(),StaticEnum<EDeadReason>()->GetValueAsString(DeadReason));
		AnimationHelperComponent->PlayDeadAnimationByMode(DeadReason);
		AnimationHelperComponent->StartDeadDissolve();
	}
}

void ABaseCharacter::SetCharacterState(ECharacterState NewState)
{
	CharacterState = NewState;
	UE_LOGFMT(LogCharacter, Log, "{0}, 캐릭터 상태 업데이트: {1}", GetNameSafe(this),
	          StaticEnum<ECharacterState>()->GetValueAsString(NewState));

	switch (CharacterState)
	{
	case ECharacterState::DEAD:
	case ECharacterState::HIT:
	case ECharacterState::STUN:
	case ECharacterState::EXCUTE:
		GetController()->StopMovement();
		break;
	case ECharacterState::NORMAL:
		break;
	case ECharacterState::MAX:
		break;
	default: ;
	}
}

void ABaseCharacter::ChangeMovementState(EMovementState Type, float Multiplier)
{
	MovementType = Type;

	switch (Type)
	{
	case EMovementState::Walk:
		GetCharacterMovement()->MaxWalkSpeed = AttributeComponent->GetMoveSpeed() * Multiplier;
		UE_LOGFMT(LogCharacter, Log, "걷기로 변경 : {0}", GetCharacterMovement()->MaxWalkSpeed);
		break;
	case EMovementState::Run:
		GetCharacterMovement()->MaxWalkSpeed = AttributeComponent->GetMoveSpeed() * Multiplier;
		UE_LOGFMT(LogCharacter, Log, "뛰기로 변경 : {0} {1}", GetCharacterMovement()->MaxWalkSpeed,GetName());
		break;
	default: ;
	}
}

void ABaseCharacter::ChangeBodyCollisionEnabled(ECollisionEnabled::Type NewCollisionEnabled)
{
	GetMesh()->SetCollisionEnabled(NewCollisionEnabled);

	UE_LOGFMT(LogTemp, Warning, "콜리전 반응 변경 : {0}",
	          StaticEnum<ECollisionEnabled::Type>()->GetValueAsString(NewCollisionEnabled));

	if (auto body = GetMesh()->GetBodySetup())
	{
		for (auto i = 0; i < GetMesh()->GetPhysicsAsset()->SkeletalBodySetups.Num(); ++i)
		{
			UE_LOGFMT(LogTemp, Log, "{0} : {1}", GetMesh()->GetPhysicsAsset()->SkeletalBodySetups[i]->BoneName,
			          GetMesh()->GetPhysicsAsset()->SkeletalBodySetups[i]->AggGeom.SphylElems.Num());

			for (auto& e : GetMesh()->GetPhysicsAsset()->SkeletalBodySetups[i]->AggGeom.SphylElems)
			{
				e.SetCollisionEnabled(NewCollisionEnabled);
			}

			body->InvalidatePhysicsData();
			body->CreatePhysicsMeshes();

			for (auto& e : GetMesh()->GetPhysicsAsset()->SkeletalBodySetups[i]->AggGeom.SphylElems)
			{
				UE_LOGFMT(LogTemp, Log, "{0}",
				          StaticEnum<ECollisionEnabled::Type>()->GetValueAsString(e.GetCollisionEnabled()));
			}

			UE_LOGFMT(LogTemp, Log, "------------------");
		}
	}
}

void ABaseCharacter::OnTriggerHitAnimationEnterEvent(ABaseCharacter* DamagedCharacter, AActor* HitBy)
{
	UE_LOGFMT(LogTemp, Log, "히트 몽타주 : {0} {1}", __FUNCTION__, __LINE__);
	AnimationHelperComponent->SetIsTriggeredHitAnimationExitEvent(false);
	//재생중인 몽타주를 멈춥니다.
	if (GetMesh()->GetAnimInstance() != nullptr && GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		GetMesh()->GetAnimInstance()->StopAllMontages(0.f);
	}
}

void ABaseCharacter::OnTriggerHitAnimationExitEvent(ABaseCharacter* DamagedCharacter, AActor* HitBy)
{
	if (CharacterState != ECharacterState::DEAD)
	{
		UE_LOGFMT(LogCharacter, Log, "OnTriggerHitAnimationExitEvent : 캐릭터 상태를 노말로 되돌립니다.");
		AnimationHelperComponent->SetIsTriggeredHitAnimationExitEvent(true);
		SetCharacterState(ECharacterState::NORMAL);
	}
}

void ABaseCharacter::TriggerHitAnimation_Implementation(UAbilityEffectAdditionalInformation* AdditionalInformation)
{
	if (IsMighty() == false)
	{
		UE_LOGFMT(LogTemp, Log, "히트 몽타주 : {0} {1}", __FUNCTION__, __LINE__);
		if (AdditionalInformation->bTriggerHitAnimation)
		{
			UE_LOGFMT(LogTemp, Log, "히트 몽타주 : {0} {1}", __FUNCTION__, __LINE__);
			AnimationHelperComponent->OnTriggerHitAnimationEnter.Broadcast(this, AdditionalInformation->HitBy.Get());
			//피격 애니메이션 블랜드를 위한 각도를 저장합니다.
			AnimationHelperComponent->HitDegree = UMathHelperLibrary::CalculateDegree(
				this, AdditionalInformation->Hit.Location);
			SetCharacterState(ECharacterState::HIT);
		}
	}
}

void ABaseCharacter::SetIgnoreMoveInput(bool bIgnore, AActor* AccruedBy, FGameplayTag AccruedTag)
//강인함 효과가 적용되어있는가 확인합니다.
{
	if (GetController() == nullptr)
	{
		return;
	}

	if (!AccruedTag.IsValid())
	{
		UE_LOGFMT(LogCharacter, Error, "입력무시를 시도했지만, 유효하지 않은 태그정보입니다!!");
		return;
	}

	FIgnoreInputHandler ignoreMoveInputHandler(AccruedBy, AccruedTag);
	if (bIgnore)
	{
		if (IgnoreMoveInputStacks.Contains(ignoreMoveInputHandler))
		{
			IgnoreMoveInputStacks[ignoreMoveInputHandler]++;
		}
		else
		{
			IgnoreMoveInputStacks.Emplace(ignoreMoveInputHandler, 1);
		}

		UE_LOGFMT(LogCharacter, Log, "입력 무시 추가됨!! {0}로부터 {1}태그, {2}누적", AccruedBy->GetActorNameOrLabel(),
		          AccruedTag.ToString(), IgnoreMoveInputStacks[ignoreMoveInputHandler]);

		GetController()->SetIgnoreMoveInput(true);
	}
	else
	{
		if (IgnoreMoveInputStacks.Contains(ignoreMoveInputHandler))
		{
			auto& count = IgnoreMoveInputStacks[ignoreMoveInputHandler];
			count = count - 1;

			UE_LOGFMT(LogCharacter, Log, "입력 무시 줄어듦!! {0}로부터 {1}태그, {2}누적", AccruedBy->GetActorNameOrLabel(),
			          AccruedTag.ToString(), IgnoreMoveInputStacks[ignoreMoveInputHandler]);


			if (count <= 0)
			{
				IgnoreMoveInputStacks.Remove(ignoreMoveInputHandler);
			}


			GetController()->SetIgnoreMoveInput(false);
		}
	}

	OnTriggerIgnoreMoveInput.Broadcast(IgnoreMoveInputStacks);
}

void ABaseCharacter::ClearMoveInputForGameplayTag(AActor* AccruedBy, FGameplayTag AccruedTag)
{
	const FIgnoreInputHandler ignoreMoveInputHandler(AccruedBy, AccruedTag);

	if (IgnoreMoveInputStacks.Contains(ignoreMoveInputHandler))
	{
		IgnoreMoveInputStacks.Remove(ignoreMoveInputHandler);
	}

	if (IgnoreMoveInputStacks.Num() <= 0)
	{
		if (GetController())
		{
			GetController()->SetIgnoreMoveInput(false);
		}
	}
}

void ABaseCharacter::SetIgnoreLookInput(bool bIgnore, AActor* AccruedBy, FGameplayTag AccruedTag)
{
	if (!AccruedTag.IsValid())
	{
		UE_LOGFMT(LogCharacter, Error, "입력무시를 시도했지만, 유효하지 않은 태그정보입니다!!");
		return;
	}

	FIgnoreInputHandler ignoreMoveInputHandler(AccruedBy, AccruedTag);
	if (bIgnore)
	{
		if (IgnoreLookInputStacks.Contains(ignoreMoveInputHandler))
		{
			IgnoreLookInputStacks[ignoreMoveInputHandler]++;
		}
		else
		{
			IgnoreLookInputStacks.Emplace(ignoreMoveInputHandler, 1);
		}

		UE_LOGFMT(LogCharacter, Log, "입력 무시 추가됨!! {0}로부터 {1}태그, {2}누적", AccruedBy->GetActorNameOrLabel(),
		          AccruedTag.ToString(), IgnoreLookInputStacks[ignoreMoveInputHandler]);

		GetController()->SetIgnoreLookInput(true);
	}
	else
	{
		if (IgnoreLookInputStacks.Contains(ignoreMoveInputHandler))
		{
			auto& count = IgnoreLookInputStacks[ignoreMoveInputHandler];
			count = count - 1;

			UE_LOGFMT(LogCharacter, Log, "입력 무시 줄어듦!! {0}로부터 {1}태그, {2}누적", AccruedBy->GetActorNameOrLabel(),
			          AccruedTag.ToString(), IgnoreLookInputStacks[ignoreMoveInputHandler]);


			if (count <= 0)
			{
				IgnoreLookInputStacks.Remove(ignoreMoveInputHandler);
			}


			GetController()->SetIgnoreLookInput(false);
		}
	}
}

void ABaseCharacter::ClearLookInputForGameplayTag(AActor* AccruedBy, FGameplayTag AccruedTag)
{
	const FIgnoreInputHandler ignoreMoveInputHandler(AccruedBy, AccruedTag);

	if (IgnoreLookInputStacks.Contains(ignoreMoveInputHandler))
	{
		IgnoreLookInputStacks.Remove(ignoreMoveInputHandler);
	}

	if (IgnoreLookInputStacks.Num() <= 0)
	{
		if (GetController())
		{
			GetController()->SetIgnoreLookInput(false);
		}
	}
}

void ABaseCharacter::ClearLookInput()
{
	IgnoreLookInputStacks.Empty();
	GetController()->SetIgnoreLookInput(false);
}

void ABaseCharacter::ClearMoveInput()
{
	IgnoreMoveInputStacks.Empty();
	GetController()->SetIgnoreMoveInput(false);
}

void ABaseCharacter::ChangeStatusEffectMaterial(EStatusEffect EffectType)
{
	if (StatusEffectMaterialMap.Contains(EffectType))
	{
		for (auto i = 0; i < OriginalMaterials.Num(); i++)
		{
			GetMesh()->SetMaterial(i, StatusEffectMaterialMap[EffectType]);
		}
	}
}

void ABaseCharacter::RestoreStatusEffectMaterial()
{
	for (auto i = 0; i < OriginalMaterials.Num(); i++)
	{
		GetMesh()->SetMaterial(i, OriginalMaterials[i]);
	}
}

void ABaseCharacter::DeactivateMightyAbility()
{
	UAbilityHelperLibrary::DeactivateMightyAbility(this);
}

void ABaseCharacter::CreateBodyMaterialInstance()
{
	const auto& index = GetMesh()->GetMaterials().Num();
	for (int32 i = 0; i < 4; i++)
	{
		BodyMaterialInstance.Add(GetMesh()->CreateDynamicMaterialInstance(i));
	}
}
