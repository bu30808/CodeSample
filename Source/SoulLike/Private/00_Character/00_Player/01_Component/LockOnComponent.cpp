// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/00_Player/01_Component/LockOnComponent.h"

#include "ComponentReregisterContext.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/01_Component/AnimationHelperComponent.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "97_Interface/LockOnInterface.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogLockOnComponent)

// Sets default values for this component's properties
ULockOnComponent::ULockOnComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	// ...
}


// Called when the game starts
void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Owner = Cast<APlayerCharacter>(GetOwner());
	if (Owner.IsValid())
	{
		Camera = Owner->GetFollowCamera();
		SpringArm = Owner->GetCameraBoom();
		SpringArmOriginalLocation = SpringArm->GetRelativeLocation();
		OriginalArmLength = SpringArm->TargetArmLength;
	}
}


// Called every frame
void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	CreateTrace();
	//틱마다 정리하지 말고, 락온 타겟을 바꿀 때, 바뀔 대상이 멀면 제거하는 식으로 하자.
	//RemoveDistantTargetsFromLockOnList();
	LookAtTarget(DeltaTime);
	SetLockOnWidgetPosToLookTarget();
	//RemoveHideTargetsFromLockOnList();
	//RemoveCannotLockOnTargetOnList();
}

void ULockOnComponent::Start()
{
	if (Owner.IsValid())

	{
		CurIndex = 0;
		HitActors.Empty();

		Owner->GetAnimationHelperComponent()->bModifySkeletonTransform = true;
		Owner->GetController()->SetIgnoreLookInput(true);
		SpringArm->bEnableCameraLag = false;
		SpringArm->bEnableCameraRotationLag = false;
		SpringArm->SetRelativeLocation(FVector(0, 0, 150.f));

		SetComponentTickEnabled(true);
	}
}

void ULockOnComponent::End()
{
	//UE_LOGFMT(LogActorComponent, Warning, "{0} {1}", __FUNCTION__, __LINE__);
	if (Owner.IsValid())
	{
		SetComponentTickEnabled(false);
		CurIndex = 0;

		HitActors.Empty();
		PreLookTargetStack.Empty();
		LookTarget = nullptr;

		Owner->GetAnimationHelperComponent()->bModifySkeletonTransform = false;
		Owner->GetController()->SetIgnoreLookInput(false);
		SpringArm->bEnableCameraLag = true;
		SpringArm->bEnableCameraRotationLag = true;
		SpringArm->TargetArmLength = OriginalArmLength;
		SpringArm->SetRelativeLocation(SpringArmOriginalLocation);
		Camera->SetRelativeLocation(FVector::ZeroVector);
		Camera->SetRelativeRotation(FRotator::ZeroRotator);

		if (LockOnWidgetComponent!=nullptr)
		{
			LockOnWidgetComponent->SetVisibility(false);
			LockOnWidgetComponent->SetWorldLocation(Owner->GetActorLocation());
			LockOnWidgetComponent->AttachToComponent(Owner->GetRootComponent(),FAttachmentTransformRules(EAttachmentRule::SnapToTarget,true));
			UE_LOGFMT(LogTemp,Log,"락온 위젯 안 보이게 설정됨.");
		}else
		{
			UE_LOGFMT(LogTemp,Error,"락온 위젯이 널포인터라 안보이게 설정할 수 없음.");
		}

		UnBindChangeTargetEvent();

		OnLockOnEnd.Broadcast();

		//다른 경로를 통해서 End가 호출된 경우 강제종료시킵니다.
		if (const auto abComp = Owner->GetAbilityComponent())
		{
			if (abComp->HasAnyActivateAbilityTags(
				FGameplayTagContainer(FGameplayTag::RequestGameplayTag("Common.Active.LockOn"))))
			{
				abComp->ForceEndAbility(FGameplayTag::RequestGameplayTag("Common.Active.LockOn"));
			}
		}
		Owner->GetAnimationHelperComponent()->SpineRotation = FRotator::ZeroRotator;
	}
	else
	{
		UE_LOGFMT(LogLockOnComponent, Error, "{0} {1}", __FUNCTION__, __LINE__);
	}
}

AActor* ULockOnComponent::GetNonCurrentTarget()
{
	AActor* target = nullptr;

	if (HitActors.Num() > 0)
	{
		if (!HitActors.IsValidIndex(CurIndex))
		{
			CurIndex = 0;
		}

		if (HitActors[CurIndex].GetActor() == LookTarget)
		{
			CurIndex++;
		}

		if (!HitActors.IsValidIndex(CurIndex))
		{
			CurIndex = 0;
		}

		//UE_LOGFMT(LogLockOnComponent,Log,"{0}번 인덱스 대상을 가져옵니다 : {1}",CurIndex,HitActors[CurIndex].GetActor()->GetActorNameOrLabel());
		target = HitActors[CurIndex].GetActor();

		if (target->GetDistanceTo(Owner.Get()) > RemoveDistance)
		{
			//UE_LOGFMT(LogLockOnComponent,Log,"너무 멀어서 제거하고 다른 대상을 가져옵니다 : {0}",target->GetActorNameOrLabel());
			const auto index = HitActors.IndexOfByPredicate([target](const FHitResult& hit)
			{
				return hit.GetActor() == target;
			});

			HitActors.RemoveAt(index);

			return GetNonCurrentTarget();
		}
	}

	/*if(target!=nullptr)
	{
		UE_LOGFMT(LogLockOnComponent,Warning,"최종 대상 : {0}",target->GetActorNameOrLabel());
	}*/

	return target;
}

AActor* ULockOnComponent::GetPreTargetNotOverRemoveDistance()
{
	AActor* target = nullptr;

	if (PreLookTargetStack.IsEmpty() == false)
	{
		auto pop = PreLookTargetStack.Pop();
		if (pop.IsValid())
		{
			if (pop->GetDistanceTo(Owner.Get()) <= RemoveDistance)
			{
				target = pop.Get();
				PreLookTargetStack.Insert(target, 0);
				return target;
			}

			return GetPreTargetNotOverRemoveDistance();
		}
	}

	return target;
}

// 4 3 2 1
// 2 3 4
void ULockOnComponent::OnChangedNextTargetEvent()
{
	RemoveCannotLockOnTargetOnList();

	if (HitActors.Num() > 0)
	{
		//처음 락온을 시작한 경우에는 가장 가까운 대상을 타겟으로 해줍니다.
		if (LookTarget == nullptr)
		{
			//정렬
			LookTarget = GetNearestTarget();
			return;
		}

		//그 이외의 경우에는 정렬하지 않고 그냥 배열 순서대로 가져옵니다.

		//현재 락온중이지 않은 타겟을 가져옵니다.
		if (auto target = GetNonCurrentTarget())
		{
			//이전에 타겟중이던 대상이 있다면 저장합니다.
			if (LookTarget != nullptr)
			{
				if (!PreLookTargetStack.Contains(LookTarget))
				{
					UE_LOGFMT(LogLockOnComponent, Log, "이전 타겟을 스텍에 푸시합니다 : {0}", LookTarget->GetActorNameOrLabel());
					PreLookTargetStack.Push(LookTarget);
				}
			}

			LookTarget = target;
		}
	}
	else
	{
		End();
	}
}

void ULockOnComponent::OnChangedPreTargetEvent()
{
	if (HitActors.Num() > 0)
	{
		//이전 타겟이 잡힌게 아무것도 없다면, 그냥 다음타겟을 잡습니다.
		if (PreLookTargetStack.Num() <= 0)
		{
			OnChangedNextTargetEvent();
			return;
		}

		//이전 타겟을 가져오되, 거리를 벗어나지 않은 대상을 가져옵니다.
		if (auto preTarget = GetPreTargetNotOverRemoveDistance())
		{
			LookTarget = preTarget;
		}
		else
		{
			OnChangedNextTargetEvent();
		}
	}
	else
	{
		End();
	}
}

void ULockOnComponent::BindChangeTargetEvent()
{
	OnLockOnTargetChangeNext.AddUniqueDynamic(this, &ULockOnComponent::OnChangedNextTargetEvent);
	OnLockOnTargetChangePre.AddUniqueDynamic(this, &ULockOnComponent::OnChangedPreTargetEvent);
}

void ULockOnComponent::UnBindChangeTargetEvent()
{
	OnLockOnTargetChangeNext.RemoveAll(this);
	OnLockOnTargetChangePre.RemoveAll(this);
}

FVector ULockOnComponent::GetPlayerMoveInputDirection()
{
	if (Owner.IsValid())
	{
		if (auto pc = Owner->GetController())
		{
			auto inputDirVector = Owner->MovementInputVector;
			auto conRot = pc->GetControlRotation();

			auto fwd = UKismetMathLibrary::GetForwardVector(conRot) * inputDirVector.Y;
			auto right = UKismetMathLibrary::GetRightVector(conRot) * inputDirVector.X;

			return fwd + right;
		}
	}

	return FVector::ZeroVector;
}

FRotator ULockOnComponent::GetPlayerInputDirRotation()
{
	if (Owner.IsValid())
	{
		return UKismetMathLibrary::FindLookAtRotation(Owner->GetActorLocation(),
		                                              Owner->GetActorLocation() + GetPlayerMoveInputDirection());
	}
	return FRotator::ZeroRotator;
}

void ULockOnComponent::CheckTraceHitActorIsHide(TArray<FHitResult>& Hits)
{
	const auto startLocation = Camera->GetComponentLocation();

	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	//몬스터가 사용하는 트레이스 채널
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	for (auto i = Hits.Num() - 1; i >= 0; i--)
	{
		if (Hits[i].GetActor()->IsA<ACharacter>())
		{
			if (auto character = Cast<ACharacter>(Hits[i].GetActor()))
			{
				TArray<FName> boneNames;
				character->GetMesh()->GetBoneNames(boneNames);
				bool bHide = true;

				AActor* obstarcle = nullptr;
				//모든 본에 대해서 트레이스를 쏩니다.
				for (auto name : boneNames)
				{
					FVector endLocation = character->GetMesh()->GetBoneLocation(name);
					FHitResult hit;
					//한번이라도 타겟의 본에 걸렸을 경우엔, 숨지 않은것으로 판단합니다.
					if (UKismetSystemLibrary::LineTraceSingleForObjects(Owner.Get(), startLocation, endLocation,
					                                                    objectTypes, bTraceComplex, TArray<AActor*>(),
					                                                    EDrawDebugTrace::None, hit, true))
					{
						if (hit.GetActor() == character)
						{
							bHide = false;
							break;
						}
						obstarcle = hit.GetActor();
					}
				}

				//숨어있는 경우에는 제거합니다.
				if (bHide)
				{
					if (obstarcle)
					{
						UE_LOGFMT(LogLockOnComponent, Warning, "이 대상({0})이 장애물 ({1}) 뒤에 숨어있는 관계로 리스트에서 제거합니다.",
						          character->GetActorNameOrLabel(), obstarcle->GetActorNameOrLabel());
					}
					Hits.RemoveAt(i);
				}
			}
		}
	}
}

void ULockOnComponent::CreateTrace()
{
	if (Owner.IsValid() && Camera.IsValid())
	{
		const auto cameraForward = Camera->GetForwardVector();

		const auto startLocation = Camera->GetComponentLocation();
		const auto endLocation = (cameraForward * Distance) + Camera->GetComponentLocation();

		TArray<AActor*> ignoreActors;
		TArray<FHitResult> outHits;

		//트레이스를 그려 대상들을 감지합니다.
		UKismetSystemLibrary::BoxTraceMultiForObjects(Owner.Get(), startLocation, endLocation, BoxHalfSize,
		                                              FRotator::ZeroRotator, ObjectTypes, bTraceComplex, ignoreActors,
		                                              DrawDebugType, outHits, true, TraceColor, FLinearColor::Green,
		                                              DrawTime);


		//감지된 대상이 장애물 뒤에 숨어있는지 확인합니다.
		//CheckTraceHitActorIsHide(outHits);

		//감지된 대상을 배열에 저장합니다.
		AddTraceHitActors(outHits);

		//배열에 저장된게 아무것도 없다면, 아무것도 안 걸린 상태니까 종료합니다.
		if (HitActors.Num() <= 0)
		{
			UE_LOGFMT(LogLockOnComponent, Warning, "걸린 대상이 없으므로 종료합니다. {0} {1}", __FUNCTION__, __LINE__);
			End();
		}
	}
}

void ULockOnComponent::AddTraceHitActors(const TArray<FHitResult>& Hits)
{
	for (const auto& Hit : Hits)
	{
		if (!HitActors.ContainsByPredicate([&](const FHitResult& Inner) { return Inner.GetActor() == Hit.GetActor(); }))
		{
			//UE_LOGFMT(LogActorComponent, Log, "ADD NEW HIT ACTOR : {0}", Hit.GetActor()->GetName());
			if (UKismetSystemLibrary::DoesImplementInterface(Hit.GetActor(), ULockOnInterface::StaticClass()))
			{
				if (ILockOnInterface::Execute_IsLockOnAble(Hit.GetActor()))
				{
					HitActors.Add(Hit);
				}
			}
			else
			{
				HitActors.Add(Hit);
			}
		}
	}
}

void ULockOnComponent::LookAtTarget(float DeltaTime)
{
	if (HitActors.Num() > 0)
	{
		if (LookTarget.IsValid() == false)
		{
			LookTarget = GetNearestTarget();
		}

		//찾아온 가장 가까운 액터가 비 정상적인 경우
		if (LookTarget.IsValid() == false)
		{
			//다음 타겟을 찾습니다.
			OnChangedNextTargetEvent();
			return;
		}

		if (LookTarget->IsA<ABaseCharacter>())
		{
			if (Cast<ABaseCharacter>(LookTarget)->IsDead())
			{
				for (auto i = HitActors.Num() - 1; i >= 0; i--)
				{
					if (HitActors[i].GetActor() == LookTarget)
					{
						HitActors.RemoveAt(i);
						break;
					}
				}

				OnChangedNextTargetEvent();
				return;
			}
		}


		{
			if (LockOnWidgetComponent!=nullptr && LockOnWidgetComponent->IsVisible() == false)
			{
				LockOnWidgetComponent->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld,true));
				LockOnWidgetComponent->SetVisibility(true);
				UE_LOGFMT(LogTemp,Log,"락온 위젯 보이게 설정됨.");
			}

			auto distanceToTarget = (LookTarget->GetActorLocation() - Owner->GetActorLocation()).Size();

			float newZ = FMath::Clamp(distanceToTarget * SpringArmZFactor, SpringArmOriginalLocation.Z,
			                          SpringArmOriginalLocation.Z * 2.f);
			SpringArm->SetRelativeLocation(FVector(0, 0, newZ));

			float newArmLength = FMath::Clamp(distanceToTarget * SpringArmLengthFactor + OriginalArmLength,
			                                  OriginalArmLength / 4, OriginalArmLength * 1.5f);
			SpringArm->TargetArmLength = newArmLength;

			//플레이어가 타겟을 바라봄
			if (bEnableLockOnRotation)
			{
				//정상적으로 가져온 경우
				FRotator characterLookAtRot = UKismetMathLibrary::FindLookAtRotation(Owner->GetActorLocation(),
					LookTarget->GetActorLocation());

				FRotator characterRot = Owner->GetActorRotation();
				FRotator characterInterpRot = FMath::RInterpTo(characterRot, characterLookAtRot, DeltaTime,
				                                               CharaterRotationLerpSpeed);
				Owner->SetActorRotation(FRotator(0, characterInterpRot.Yaw, 0));
				Owner->GetAnimationHelperComponent()->SpineRotation = (LookTarget->GetActorLocation() - Owner->GetActorLocation()).Rotation();
			}

			FRotator cameraLookAtRot = UKismetMathLibrary::FindLookAtRotation(Camera->GetComponentLocation(),
			                                                                  LookTarget->GetActorLocation());
			FRotator controllRot = Owner->GetControlRotation();
			FRotator cameraInterpRot = FMath::RInterpTo(controllRot, cameraLookAtRot, DeltaTime,
			                                            CameraRotationLerpSpeed);
			Owner->GetController()->SetControlRotation(cameraInterpRot);
		}
	}
}

void ULockOnComponent::SetLockOnWidgetPosToLookTarget()
{
	if (LookTarget.IsValid())
	{
		if (LockOnWidgetComponent == nullptr)
		{
			//UE_LOGFMT(LogTemp, Warning, "{0} {1}", __FUNCTION__, __LINE__);
			if (UActorComponent* newComp = NewObject<
				UActorComponent>(Owner.Get(), UWidgetComponent::StaticClass()))
			{
				//UE_LOGFMT(LogTemp, Warning, "{0} {1}", __FUNCTION__, __LINE__);
				newComp->RegisterComponent();
				LockOnWidgetComponent = Cast<UWidgetComponent>(newComp);
				LockOnWidgetComponent->SetWidgetClass(LockOnWidgetClass);
				LockOnWidgetComponent->SetDrawSize(FVector2d(32, 32));
				LockOnWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
				LockOnWidgetComponent->InitWidget();
			}
		}

		//UE_LOGFMT(LogTemp, Error, "{0} {1}", __FUNCTION__, __LINE__);
		LockOnWidgetComponent->SetWorldLocation(LookTarget->GetActorLocation());

		//OnUpdateLockOnTargetWidget.Broadcast(LookTarget->GetActorLocation());
	}
}

void ULockOnComponent::SortLockOnListByDistance()
{
	/*
	GPT 피셜 : 
	GetDistanceTo사용은 잠재적으로 비효율적일 수 있습니다. 왜냐하면 GetDistanceTo는 객체 간의 거리를 계산하기 위해 매우 비용이 많이 드는 작업일 수 있기 때문입니다.
	대신 GetDistanceSquaredTo를 사용하여 거리의 제곱을 비교하는 것이 더 효율적입니다. 거리의 제곱을 비교하면 실제 거리를 계산하는 것보다 계산 비용이 적게 듭니다.
	 */
	HitActors.Sort([this](const FHitResult& a, const FHitResult& b)
	{
		float DistanceSquaredA = FVector::DistSquared(GetOwner()->GetActorLocation(), a.GetActor()->GetActorLocation());
		float DistanceSquaredB = FVector::DistSquared(GetOwner()->GetActorLocation(), b.GetActor()->GetActorLocation());
		return DistanceSquaredA < DistanceSquaredB;
	});
}

AActor* ULockOnComponent::GetNearestTarget()
{
	if (Owner.IsValid())
	{
		float MinDistance = MAX_FLT; // 초기 거리 설정
		AActor* closestTarget = nullptr; // 가장 가까운 액터

		for (const auto& hit : HitActors)
		{
			if (hit.GetActor() && hit.GetActor()->IsA<ABaseMonster>())
			{
				const float distance = FVector::Distance(hit.GetActor()->GetActorLocation(), Owner->GetActorLocation());

				if (distance < MinDistance)
				{
					MinDistance = distance;
					closestTarget = hit.GetActor();
				}
			}
		}

		CurIndex = HitActors.IndexOfByPredicate([closestTarget](const FHitResult& Hit)
		{
			return Hit.GetActor() == closestTarget;
		});

		return closestTarget;
	}
	return nullptr;
}

void ULockOnComponent::RemoveDistantTargetsFromLockOnList()
{
	if (Owner.IsValid())
	{
		bool bRemovedLockOnTarget = false;
		for (auto i = HitActors.Num() - 1; i >= 0; i--)
		{
			if (HitActors[i].GetActor()->IsValidLowLevel())
			{
				if (HitActors[i].GetActor()->GetDistanceTo(Owner.Get()) > RemoveDistance)
				{
					UE_LOGFMT(LogLockOnComponent, Warning, "대상이 너무 멀어져 락온 리스트로부터 제외합니다.");
					const auto removeActor = HitActors[i].GetActor();
					HitActors.RemoveAt(i);
					if (removeActor == LookTarget)
					{
						bRemovedLockOnTarget = true;
					}
				}
			}
			else
			{
				HitActors.RemoveAt(i);
			}
		}


		if (bRemovedLockOnTarget)
		{
			UE_LOGFMT(LogLockOnComponent, Warning, "제외된 대상이 락온 타겟이기 때문에 다음타겟으로 넘깁니다.");
			OnChangedNextTargetEvent();
		}
	}
}

void ULockOnComponent::RemoveHideTargetsFromLockOnList()
{
	const auto startLocation = Camera->GetComponentLocation();

	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	//몬스터가 사용하는 트레이스 채널
	//objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic)); // 월드 스테틱
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic)); // 월드 다이나믹

	for (auto i = HitActors.Num() - 1; i >= 0; i--)
	{
		if (HitActors[i].GetActor()->IsValidLowLevel() && HitActors[i].GetActor()->IsA<ACharacter>())
		{
			if (auto character = Cast<ACharacter>(HitActors[i].GetActor()))
			{
				TArray<FName> boneNames;
				character->GetMesh()->GetBoneNames(boneNames);
				bool bHide = true;


				//모든 본에 대해서 트레이스를 쏩니다.
				for (auto name : boneNames)
				{
					FVector endLocation = character->GetMesh()->GetBoneLocation(name);

					FHitResult hit;
					//한번이라도 타겟의 본에 걸렸을 경우엔, 숨지 않은것으로 판단합니다.
					if (!UKismetSystemLibrary::LineTraceSingleForObjects(Owner.Get(), startLocation, endLocation,
					                                                     objectTypes, true, TArray<AActor*>(),
					                                                     EDrawDebugTrace::None, hit, true))
					{
						//UE_LOGFMT(LogTemp,Warning,"{0}은 시야에 잘 보입니다!",character->GetName());
						//안 걸린적이 있는가 확인합니다. -> 카메라와 몬스터 사이에 장애물이 없으므로 하나의 본이라도 보인다는 뜻
						bHide = false;
						if (RemoveTimerHandles.Contains(character))
						{
							character->GetWorldTimerManager().ClearTimer(RemoveTimerHandles[character]);
						}
						return;
					}
				}

				//숨어있는경우 타이머를 돌려서 일정시간 이상 타이머가 해제되지 않으면 리스트에서 제거합니다.
				if (bHide)
				{
					if (!RemoveTimerHandles.Contains(character))
					{
						FTimerHandle removeTimerHandle;
						FTimerDelegate removeTimerDel = FTimerDelegate::CreateUObject(
							this, &ULockOnComponent::RemoveTargetFromLockOnListByTimer, character);
						character->GetWorldTimerManager().SetTimer(removeTimerHandle, removeTimerDel, 5.f, false);
					}
				}
			}
		}
		else
		{
			HitActors.RemoveAt(i);
		}
	}
}

void ULockOnComponent::RemoveTargetFromLockOnListByTimer(ACharacter* Character)
{
	const auto find = HitActors.FindByPredicate([Character](const FHitResult& Hit)
	{
		return Hit.GetActor() == Character;
	});

	if (find != nullptr)
	{
		RemoveTimerHandles.Remove(find->GetActor());
		for (auto i = HitActors.Num(); i >= 0; i--)
		{
			if (HitActors.IsValidIndex(i) && HitActors[i].GetActor() == Character)
			{
				//UE_LOGFMT(LogTemp,Log,"{0}을 락온 리스트에서 제거합니다 ",Character->GetName());
				HitActors.RemoveAt(i);
				if (LookTarget.IsValid() && Character == LookTarget)
				{
					//UE_LOGFMT(LogTemp,Log,"지운 대상이 락온중이기때문에 다음 타겟으로 넘깁니다.");
					OnChangedNextTargetEvent();
				}


				break;
			}
		}
	}
}

void ULockOnComponent::RemoveCannotLockOnTargetOnList()
{
	if (Owner.IsValid())
	{
		bool bRemovedLockOnTarget = false;
		for (auto i = HitActors.Num() - 1; i >= 0; i--)
		{
			if (UKismetSystemLibrary::DoesImplementInterface(HitActors[0].GetActor(), ULockOnInterface::StaticClass()))
			{
				if (ILockOnInterface::Execute_IsLockOnAble(HitActors[i].GetActor()) == false)
				{
					const auto removeActor = HitActors[i].GetActor();
					HitActors.RemoveAt(i);
					if (removeActor == LookTarget)
					{
						bRemovedLockOnTarget = true;
					}
				}
			}
		}

		if (bRemovedLockOnTarget)
		{
			OnChangedNextTargetEvent();
		}
	}
}
