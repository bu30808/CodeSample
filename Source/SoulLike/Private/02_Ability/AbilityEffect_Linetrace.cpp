// Fill out your copyright notice in the Description page of Project Settings.


#include "02_Ability/AbilityEffect_Linetrace.h"

#include "GenericTeamAgentInterface.h"
#include "00_Character/BaseCharacter.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "02_Ability/01_Task/GameplayTask_LaunchEvent.h"
#include "96_Library/AbilityHelperLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Logging/StructuredLog.h"

UAbilityEffect_Linetrace::UAbilityEffect_Linetrace()
{
	EffectApplyType = EEffectApplyType::InfiniteWithTick;
}

bool UAbilityEffect_Linetrace::IsBoneName(FName SocketName) const
{
	if (MeshComponent->IsA<USkeletalMeshComponent>())
	{
		return Cast<USkeletalMeshComponent>(MeshComponent)->GetSocketBoneName(SocketName).IsEqual(SocketName);
	}

	return false;
}

void UAbilityEffect_Linetrace::UpdateLastSocketLocation()
{
	for (auto Socket : Sockets)
	{
		LastKnownSocketLocation.Add(Socket, MeshComponent->GetSocketLocation(Socket));
	}
}

void UAbilityEffect_Linetrace::OnHitActorEvent_Implementation(const FHitResult& Hit, ABaseCharacter* Target)
{
	if (!CanApplyEffect(Target))
	{
		return;
	}

	if (Hit.GetActor() != nullptr)
	{

		if(bIgnoreSelf)
		{
			if(TickOwner == Hit.GetActor())
			{
				return;
			}
		}
		
		UE_LOGFMT(LogEffect, Warning, "{0}이 히트를 감지했습니다 : {1}, {2}, {3}", Target->GetActorNameOrLabel(),
		          Hit.GetActor()->GetActorNameOrLabel(), Hit.Location.ToString(), Hit.ImpactPoint.ToString());

		if (Hit.GetActor()->IsA<ABaseCharacter>())
		{
			if (auto character = Cast<ABaseCharacter>(Hit.GetActor()))
			{

				//아군 무시 옵션이 켜져있을 경우, 팀을 확인합니다.
				if(bIgnoreAlly)
				{
					//걸린 대상의 팀
					const auto& targetTeam = UAbilityHelperLibrary::GetTeam(character);
					const auto& ownerTeam =  UAbilityHelperLibrary::GetTeam(TickOwner.Get());

					if(targetTeam.GetId() == ownerTeam.GetId())
					{
						return;
					}
				}
				
				UpdateAttributeEffectsAffectedByOwnersAttribute(Target);

				const auto addInfo = CreateAdditionalInfo();
				addInfo->Hit = Hit;
				addInfo->HitBy = Target;
				addInfo->bTriggerHitAnimation = bTriggerHitAnimation;

				bool bSucc =false;

				if(bSplitDamageOverTime)
				{
					bSucc = ApplyInstantEffect(character, addInfo, GetDeltaTime());
				}else
				{
					bSucc = ApplyInstantEffect(character, addInfo);
				}
				
				if (bSucc)
				{
					//피해를 입은 대상에서 impact 큐를 생성해 적용합니다.
					character->ShowGotHitCue(Hit.Location);

					//피해를 입은 대상에게 그 외 다른 큐가 필요하다면 적용합니다.
					UpdateCueLocation(Hit.Location);
					ApplyCueInstance(character);
				}

				Target->OnLineTraceEffectHitActor.Broadcast(Hit,Target);
			}
			
		}
		else
		{
			if (Hit.GetComponent()->IsSimulatingPhysics())
			{
				Hit.GetComponent()->AddForce(Hit.ImpactPoint * 2000.f);
			}
		}
	}
}

void UAbilityEffect_Linetrace::Init(UPrimitiveComponent* TargetComp)
{
	if (TargetComp)
	{
		if (bIgnoreSelf)
		{
			IgnoreActors.AddUnique(TargetComp->GetOwner());
		}

		MeshComponent = TargetComp;

		Sockets = TargetComp->GetAllSocketNames();

		/*
		//스켈레톤 메시인 경우 본 이름은 소켓에서 제거합니다.
		if (MeshComponent->IsA<USkeletalMeshComponent>())
		{
			for (auto i = Sockets.Num() - 1; i >= 0; i--)
			{
				if (IsBoneName(Sockets[i]))
				{
					Sockets.RemoveAt(i);
				}
			}
		}*/

		//무시 대상에 있다면 없앱니다.
		for (auto ignoreSocket : IgnoreSockets)
		{
			Sockets.Remove(ignoreSocket);
		}


		LastKnownSocketLocation.Empty();
		ClearHitActors();

		OnHitActor.AddUniqueDynamic(this, &UAbilityEffect_Linetrace::OnHitActorEvent);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s %s"), *FString(__FUNCTION__), * FString::FromInt(__LINE__));
	}
}

void UAbilityEffect_Linetrace::InitWithSockets(UPrimitiveComponent* TargetComp)
{
	if (TargetComp)
	{
		if (bIgnoreSelf)
		{
			IgnoreActors.AddUnique(TargetComp->GetOwner());
		}

		MeshComponent = TargetComp;

		Sockets = CustomSockets;

		/*//스켈레톤 메시인 경우 본 이름은 소켓에서 제거합니다.
		if (MeshComponent->IsA<USkeletalMeshComponent>())
		{
			for (auto i = Sockets.Num() - 1; i >= 0; i--)
			{
				if (IsBoneName(Sockets[i]))
				{
					UE_LOGFMT(LogTemp,Error,"본 이름을 소켓으로 주면 안 됩니다.");
					Sockets.RemoveAt(i);
				}
			}
		}*/

		//무시 대상에 있다면 없앱니다.
		for (auto ignoreSocket : IgnoreSockets)
		{
			Sockets.Remove(ignoreSocket);
		}


		LastKnownSocketLocation.Empty();
		ClearHitActors();

		OnHitActor.AddUniqueDynamic(this, &UAbilityEffect_Linetrace::OnHitActorEvent);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s %s"), *FString(__FUNCTION__), * FString::FromInt(__LINE__));
	}
}

void UAbilityEffect_Linetrace::ActivateTrace(bool bActive, class ABaseCharacter* Target)
{
	if (!MeshComponent->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("%s %s"), *FString(__FUNCTION__), * FString::FromInt(__LINE__));
		//ErrorLog(this, TEXT("라인트레이스 컴포넌트가 정상적으로 초기화 되지 않았습니다."));
		return;
	}

	ClearHitActors();
	LastKnownSocketLocation.Empty();

	if (bActive)
	{
		for (auto Socket : Sockets)
		{
			LastKnownSocketLocation.Add(Socket, MeshComponent->GetSocketLocation(Socket));
		}
		//테스크를 만들고 틱을 돌립니다.
		ApplyTickEffect(Target);
	}
	else
	{
		if (TickTask != nullptr)
		{
			//UE_LOGFMT(LogEffect, Warning, "{0}이 소유한 {1}의 라인트레이스 테스크 종료 : {2}",Target->GetController()->GetActorNameOrLabel(),Target->GetName(), UniqueEffectTag.ToString());
			TickTask->EndTask();
			TickTask.Reset();
		}
	}
}
#if WITH_EDITOR
void UAbilityEffect_Linetrace::CopyValues(UAbilityEffect* Effect)
{
	Super::CopyValues(Effect);
	auto linetraceData = Cast<UAbilityEffect_Linetrace>(Effect);

	bUseWithCustomSockets = linetraceData->bUseWithCustomSockets;
	CustomSockets = linetraceData->CustomSockets;

	bTraceOtherSocketsAtSameTime = linetraceData->bTraceOtherSocketsAtSameTime;
	bTraceOtherSocketsAtDifferentTime = linetraceData->bTraceOtherSocketsAtDifferentTime;
	TraceType = linetraceData->TraceType;
	TraceChannel = linetraceData->TraceChannel;
	ObjectTypes = linetraceData->ObjectTypes;
	bTraceComplex = linetraceData->bTraceComplex;
	IgnoreActors = linetraceData->IgnoreActors;
	bIgnoreSelf = linetraceData->bIgnoreSelf;
	IgnoreSockets = linetraceData->IgnoreSockets;
	TraceColor = linetraceData->TraceColor;
	TraceHitColor = linetraceData->TraceHitColor;
	DrawDebugType = linetraceData->DrawDebugType;
	DrawTime = linetraceData->DrawTime;
	TraceShapeType = linetraceData->TraceShapeType;
	BoxHalfSize = linetraceData->BoxHalfSize;
	BoxOrientation = linetraceData->BoxOrientation;
	CapsuleRadius = linetraceData->CapsuleRadius;
	CapsuleHalfHeight = linetraceData->CapsuleHalfHeight;
	SphereRadius = linetraceData->SphereRadius;
}
#endif

/*
void UAbilityEffect_Linetrace::ApplyAbilityDamageTalent(ABaseCharacter* Target, ABaseCharacter* DamagedCharacter)
{
	if (Target->IsA<APlayerCharacter>())
	{
		if (const auto atComp = Cast<APlayerCharacter>(Target)->GetAbilityTalentComponent())
		{
			for (auto& effect : UpdatedAttributeEffectsAffectedByTargetsAttribute)
			{
				UE_LOGFMT(LogTemp, Warning, "------------------------");
				UE_LOGFMT(LogTemp, Warning, "기본 피해량 : {0}", effect.ApplyValue);

				//피해량 특성 적용
				{
					const auto& sum = atComp->CalculateModifiedDamageWithTraits(Target,DamagedCharacter, AttackType, effect.ApplyValue);
					UE_LOGFMT(LogTemp, Warning, "피해량 특성으로 증가할 예정인 피해량 : {0}", sum);
					effect.ApplyValue += sum;
				}


				UE_LOGFMT(LogTemp, Warning, "특성이 적용된 피해량 : {0}", effect.ApplyValue);
				UE_LOGFMT(LogTemp, Warning, "------------------------");

				//최종 피해량 특성 적용
				{
					const auto& sum = atComp->CalculateModifiedResultDamageWithTraits(Target,DamagedCharacter,AttackType, effect.ApplyValue);
					UE_LOGFMT(LogTemp, Warning, "피해량 특성으로 증가할 예정인 피해량 : {0}", sum);
					effect.ApplyValue += sum;
					UE_LOGFMT(LogTemp, Warning, "최종 피해량 특성이 적용된 피해량 : {0}", effect.ApplyValue);
					UE_LOGFMT(LogTemp, Warning, "------------------------");
				}
			}
		}
	}
}
*/

/*
void UAbilityEffect_Linetrace::ApplyAbilityDecreaseDamageTalent(ABaseCharacter* Target, ABaseCharacter* DamagedCharacter)
{
	if(DamagedCharacter->IsA<APlayerCharacter>())
	{
		//방어력 특성 적용
		if (const auto atComp = Cast<APlayerCharacter>(DamagedCharacter)->GetAbilityTalentComponent())
		{
			for (auto& effect : UpdatedAttributeEffectsAffectedByTargetsAttribute)
			{
				UE_LOGFMT(LogTemp, Warning, "------------------------");
				UE_LOGFMT(LogTemp, Warning, "기본 피해량 : {0}", effect.ApplyValue);
				//피해량 특성 적용
				{
					const auto& sum = atComp->CalculateModifiedDecreaseDamageWithTraits(DamagedCharacter, Target, AttackType, effect.ApplyValue);
					UE_LOGFMT(LogTemp, Warning, "방어 특성으로 감소할 예정인 피해량 : {0}", sum);
					effect.ApplyValue -= sum;

					const auto& increaseDamage = atComp->CalculateModifiedIncreaseGotHitDamageWithTraits(DamagedCharacter, Target, AttackType, effect.ApplyValue);
					UE_LOGFMT(LogTemp, Warning, "입는 피해량 증가 특성으로 증가할 피해량 : {0}", increaseDamage);
					effect.ApplyValue+=increaseDamage;
				}


				UE_LOGFMT(LogTemp, Warning, "모든 특성이 적용된 피해량 : {0}", effect.ApplyValue);
				UE_LOGFMT(LogTemp, Warning, "------------------------");

			}

			if(auto attComp = Cast<APlayerCharacter>(DamagedCharacter)->GetAttributeComponent())
			{
				//MP로 받을 피해량이 저장될 구조체
				FAttributeEffect mpEffect;
				//피해량 MP치환 특성 적용
				for(auto& effect : UpdatedAttributeEffectsAffectedByTargetsAttribute)
				{
					if(effect.Attribute == EAttributeType::HP){

						//지금 플레이어가 가진 MP수치를 가져옵니다.
						const auto& mp = attComp->GetMP();
						UE_LOGFMT(LogTemp, Warning, "현재 MP : {0}", mp);
						//MP로 치환될 피해량 수치
						const auto& toMp = FMath::Clamp(atComp->CalculateModifiedDamageToMPWithTraits(effect.ApplyValue),0,mp);
						UE_LOGFMT(LogTemp, Warning, "MP로 치환될 피해량 : {0}", toMp);
						//mp량을 초과하는 수치
						const auto& overMP = FMath::Clamp(mp - toMp,0,mp);
						UE_LOGFMT(LogTemp, Warning, "MP를 초과한 피해량 : {0}", overMP);
						//MP로 받을 피해량 저장
						mpEffect.ApplyValue = toMp;
						mpEffect.Attribute = EAttributeType::MP;
						mpEffect.ApplyMethod = EAttributeApplyMethod::REMOVE;
						UE_LOGFMT(LogTemp, Warning, "MP로 받을 피해량 : {0}", mpEffect.ApplyValue);
						
						//치환되고 남은 피해량
						const auto& remainDamage = FMath::Clamp(effect.ApplyValue - toMp,0,effect.ApplyValue) + overMP;
						UE_LOGFMT(LogTemp, Warning, "치환되고 남은 HP 피해량 : {0}", remainDamage);
						
						//남은 피해량을 HP로
						effect.ApplyValue = remainDamage;
						
					}
				}

				UpdatedAttributeEffectsAffectedByTargetsAttribute.Add(mpEffect);
			}
		}
	}
}*/


void UAbilityEffect_Linetrace::AddHitActors(TArray<FHitResult> HitArray, ABaseCharacter* Target)
{
	for (const auto& Hit : HitArray)
	{
		if (!HitActors.ContainsByPredicate([&](const FHitResult& Inner) { return Inner.GetActor() == Hit.GetActor(); }))
		{
			HitActors.Add(Hit);
			OnHitActor.Broadcast(Hit, Target);
		}
	}
}

void UAbilityEffect_Linetrace::TraceOtherSocketAtSameTime(ABaseCharacter* Target)
{
	for (auto Socket1 : Sockets)
	{
		for (auto Socket2 : Sockets)
		{
			//Private Variables
			const FVector Start = MeshComponent->GetSocketLocation(Socket1);
			const FVector End = MeshComponent->GetSocketLocation(Socket2);
			TArray<FHitResult> OutHits;

			if (TraceType == ETraceType::BY_CHANNEL)
			{
				switch (TraceShapeType)
				{
				case ETraceShapeType::LINE:
					{
						UKismetSystemLibrary::LineTraceMulti(Target->GetWorld(), Start, End, TraceChannel,
						                                     bTraceComplex,
						                                     IgnoreActors, DrawDebugType, OutHits, bIgnoreSelf,
						                                     TraceColor, TraceHitColor, DrawTime);

						break;
					}

				case ETraceShapeType::BOX:
					{
						UKismetSystemLibrary::BoxTraceMulti(Target->GetWorld(), Start, End, BoxHalfSize, BoxOrientation,
						                                    TraceChannel,
						                                    bTraceComplex, IgnoreActors, DrawDebugType, OutHits,
						                                    bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
						break;
					}

				case ETraceShapeType::CAPSULE:
					{
						UKismetSystemLibrary::CapsuleTraceMulti(Target->GetWorld(), Start, End, CapsuleHalfHeight,
						                                        CapsuleRadius,
						                                        TraceChannel, bTraceComplex, IgnoreActors,
						                                        DrawDebugType, OutHits, bIgnoreSelf, TraceColor,
						                                        TraceHitColor, DrawTime);
						break;
					}

				case ETraceShapeType::SPHERE:
					{
						UKismetSystemLibrary::SphereTraceMulti(Target->GetWorld(), Start, End, SphereRadius,
						                                       TraceChannel,
						                                       bTraceComplex, IgnoreActors, DrawDebugType, OutHits,
						                                       bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
						break;
					}
				}
			}
			else
			{
				switch (TraceShapeType)
				{
				case ETraceShapeType::LINE:
					{
						UKismetSystemLibrary::LineTraceMultiForObjects(Target->GetWorld(), Start, End, ObjectTypes,
						                                               bTraceComplex,
						                                               IgnoreActors, DrawDebugType, OutHits,
						                                               bIgnoreSelf, TraceColor, TraceHitColor,
						                                               DrawTime);
						break;
					}

				case ETraceShapeType::BOX:
					{
						UKismetSystemLibrary::BoxTraceMultiForObjects(Target->GetWorld(), Start, End, BoxHalfSize,
						                                              BoxOrientation,
						                                              ObjectTypes, bTraceComplex, IgnoreActors,
						                                              DrawDebugType, OutHits, bIgnoreSelf, TraceColor,
						                                              TraceHitColor, DrawTime);
						break;
					}

				case ETraceShapeType::CAPSULE:
					{
						UKismetSystemLibrary::CapsuleTraceMultiForObjects(
							Target->GetWorld(), Start, End, CapsuleHalfHeight, CapsuleRadius, ObjectTypes,
							bTraceComplex,
							IgnoreActors, DrawDebugType, OutHits, bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
						break;
					}

				case ETraceShapeType::SPHERE:
					{
						UKismetSystemLibrary::SphereTraceMultiForObjects(
							Target->GetWorld(), Start, End, SphereRadius, ObjectTypes, bTraceComplex, IgnoreActors,
							DrawDebugType,
							OutHits, bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
						break;
					}
				}
			}

			AddHitActors(OutHits, Target);
		}
	}
}

void UAbilityEffect_Linetrace::TraceOtherSocketAtDiffrentTime(ABaseCharacter* Target)
{
	for (auto Socket1 : Sockets)
	{
		for (auto Socket2 : Sockets)
		{
			if (LastKnownSocketLocation.Contains(Socket2) == false)
			{
				UE_LOGFMT(LogEffect, Error, "{0} {1}", __FUNCTION__, __LINE__);
				return;
			}

			TArray<FHitResult> OutHits1;
			FVector Start = MeshComponent->GetSocketLocation(Socket1);
			FVector End = *LastKnownSocketLocation.Find(Socket2);

			if (TraceType == ETraceType::BY_CHANNEL)
			{
				switch (TraceShapeType)
				{
				case ETraceShapeType::LINE:
					{
						UKismetSystemLibrary::LineTraceMulti(Target->GetWorld(), Start, End, TraceChannel,
						                                     bTraceComplex,
						                                     IgnoreActors, DrawDebugType, OutHits1, bIgnoreSelf,
						                                     TraceColor, TraceHitColor, DrawTime);
						break;
					}

				case ETraceShapeType::BOX:
					{
						UKismetSystemLibrary::BoxTraceMulti(Target->GetWorld(), Start, End, BoxHalfSize, BoxOrientation,
						                                    TraceChannel,
						                                    bTraceComplex, IgnoreActors, DrawDebugType, OutHits1,
						                                    bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
						break;
					}

				case ETraceShapeType::CAPSULE:
					{
						UKismetSystemLibrary::CapsuleTraceMulti(Target->GetWorld(), Start, End, CapsuleHalfHeight,
						                                        CapsuleRadius,
						                                        TraceChannel, bTraceComplex, IgnoreActors,
						                                        DrawDebugType, OutHits1, bIgnoreSelf, TraceColor,
						                                        TraceHitColor, DrawTime);
						break;
					}

				case ETraceShapeType::SPHERE:
					{
						UKismetSystemLibrary::SphereTraceMulti(Target->GetWorld(), Start, End, SphereRadius,
						                                       TraceChannel,
						                                       bTraceComplex, IgnoreActors, DrawDebugType, OutHits1,
						                                       bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
						break;
					}
				}
			}
			else
			{
				switch (TraceShapeType)
				{
				case ETraceShapeType::LINE:
					{
						UKismetSystemLibrary::LineTraceMultiForObjects(Target, Start, End, ObjectTypes,
						                                               bTraceComplex,
						                                               IgnoreActors, DrawDebugType, OutHits1,
						                                               bIgnoreSelf, TraceColor, TraceHitColor,
						                                               DrawTime);
						break;
					}

				case ETraceShapeType::BOX:
					{
						UKismetSystemLibrary::BoxTraceMultiForObjects(Target, Start, End, BoxHalfSize,
						                                              BoxOrientation,
						                                              ObjectTypes, bTraceComplex, IgnoreActors,
						                                              DrawDebugType, OutHits1, bIgnoreSelf, TraceColor,
						                                              TraceHitColor, DrawTime);
						break;
					}

				case ETraceShapeType::CAPSULE:
					{
						UKismetSystemLibrary::CapsuleTraceMultiForObjects(
							Target, Start, End, CapsuleHalfHeight, CapsuleRadius, ObjectTypes, bTraceComplex,
							IgnoreActors, DrawDebugType, OutHits1, bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
						break;
					}

				case ETraceShapeType::SPHERE:
					{
						UKismetSystemLibrary::SphereTraceMultiForObjects(
							Target, Start, End, SphereRadius, ObjectTypes, bTraceComplex, IgnoreActors,
							DrawDebugType,
							OutHits1, bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
						break;
					}
				}
			}

			AddHitActors(OutHits1, Target);
		}

		for (auto Socket3 : Sockets)
		{
			TArray<FHitResult> OutHits2;
			FVector Start = *LastKnownSocketLocation.Find(Socket3);
			FVector End = MeshComponent->GetSocketLocation(Socket1);

			if (TraceType == ETraceType::BY_CHANNEL)
			{
				switch (TraceShapeType)
				{
				case ETraceShapeType::LINE:
					{
						UKismetSystemLibrary::LineTraceMulti(Target->GetWorld(), Start, End, TraceChannel,
						                                     bTraceComplex,
						                                     IgnoreActors, DrawDebugType, OutHits2, bIgnoreSelf,
						                                     TraceColor, TraceHitColor, DrawTime);
						break;
					}

				case ETraceShapeType::BOX:
					{
						UKismetSystemLibrary::BoxTraceMulti(Target->GetWorld(), Start, End, BoxHalfSize, BoxOrientation,
						                                    TraceChannel,
						                                    bTraceComplex, IgnoreActors, DrawDebugType, OutHits2,
						                                    bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
						break;
					}

				case ETraceShapeType::CAPSULE:
					{
						UKismetSystemLibrary::CapsuleTraceMulti(Target->GetWorld(), Start, End, CapsuleHalfHeight,
						                                        CapsuleRadius,
						                                        TraceChannel, bTraceComplex, IgnoreActors,
						                                        DrawDebugType, OutHits2, bIgnoreSelf, TraceColor,
						                                        TraceHitColor, DrawTime);
						break;
					}

				case ETraceShapeType::SPHERE:
					{
						UKismetSystemLibrary::SphereTraceMulti(Target->GetWorld(), Start, End, SphereRadius,
						                                       TraceChannel,
						                                       bTraceComplex, IgnoreActors, DrawDebugType, OutHits2,
						                                       bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
						break;
					}
				}
			}
			else
			{
				switch (TraceShapeType)
				{
				case ETraceShapeType::LINE:
					{
						UKismetSystemLibrary::LineTraceMultiForObjects(Target->GetWorld(), Start, End, ObjectTypes,
						                                               bTraceComplex,
						                                               IgnoreActors, DrawDebugType, OutHits2,
						                                               bIgnoreSelf, TraceColor, TraceHitColor,
						                                               DrawTime);
						break;
					}

				case ETraceShapeType::BOX:
					{
						UKismetSystemLibrary::BoxTraceMultiForObjects(Target->GetWorld(), Start, End, BoxHalfSize,
						                                              BoxOrientation,
						                                              ObjectTypes, bTraceComplex, IgnoreActors,
						                                              DrawDebugType, OutHits2, bIgnoreSelf, TraceColor,
						                                              TraceHitColor, DrawTime);
						break;
					}

				case ETraceShapeType::CAPSULE:
					{
						UKismetSystemLibrary::CapsuleTraceMultiForObjects(
							Target->GetWorld(), Start, End, CapsuleHalfHeight, CapsuleRadius, ObjectTypes,
							bTraceComplex,
							IgnoreActors, DrawDebugType, OutHits2, bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
						break;
					}

				case ETraceShapeType::SPHERE:
					{
						UKismetSystemLibrary::SphereTraceMultiForObjects(
							Target->GetWorld(), Start, End, SphereRadius, ObjectTypes, bTraceComplex, IgnoreActors,
							DrawDebugType,
							OutHits2, bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
						break;
					}
				}
			}

			AddHitActors(OutHits2, Target);
		}
	}
}

void UAbilityEffect_Linetrace::ClearHitActors()
{
	HitActors.Empty();
}

bool UAbilityEffect_Linetrace::IsContainActor(AActor* Actor) const
{
	return HitActors.ContainsByPredicate([Actor](const FHitResult& hit)
	{
		return hit.GetActor() == Actor;
	});
}

void UAbilityEffect_Linetrace::ProcessEffect_Implementation(ABaseCharacter* Target, AActor* EffectBy,UAbilityBase* From)
{
	if (bUseWithCustomSockets)
	{
		if (bUseOtherMeshComponent)
		{			
			InitWithSockets(Target->GetPrimitiveComponentForLineTrace());
		}
		else
		{
			InitWithSockets(Target->GetMesh());
		}
	}
	else
	{
		if (bUseOtherMeshComponent)
		{
			Init(Target->GetPrimitiveComponentForLineTrace());
		}
		else
		{
			Init(Target->GetMesh());
		}
	}

	CauseFromThisAbility = From;
	ChainSetting(Target);

	RegisterEffectTag(Target);
	ActivateTrace(true, Target);
}

void UAbilityEffect_Linetrace::OnTaskTickEvent_Implementation(float DeltaTime)
{
	TickEffectDeltaTime = DeltaTime;
	
	if (TickOwner.IsValid() == false)
	{
		UE_LOGFMT(LogEffect, Error, "틱 오너가 유효하지 않습니다 : {0} {1}", __FUNCTION__, __LINE__);
		return;
	}
	if (MeshComponent->IsValidLowLevel() == false)
	{
		UE_LOGFMT(LogEffect, Error, "MeshComponent가 유효하지 않습니다 : {0} {1}", __FUNCTION__, __LINE__);
		return;
	}

	if (TickOwner.IsValid())
	{
		//UE_LOGFMT(LogEffect, Error, "{0} : {1}의 라인트레이스 틱",GetName(),TickOwner->GetActorNameOrLabel());
		// ... 

		if (bTraceOtherSocketsAtSameTime)
		{
			TraceOtherSocketAtSameTime(TickOwner.Get());
		}

		if (bTraceOtherSocketsAtDifferentTime)
		{
			TraceOtherSocketAtDiffrentTime(TickOwner.Get());
		}

		UpdateLastSocketLocation();
	}
}

void UAbilityEffect_Linetrace::EndEffect_Implementation(ABaseCharacter* Target)
{
	//UE_LOGFMT(LogTemp, Warning, "UAbilityEffect_Linetrace {0}이 소유한 {1}의 이팩트 종료됨 : {2}",Target->GetController()->GetActorNameOrLabel(),Target->GetActorNameOrLabel(), UniqueEffectTag.ToString());

	ActivateTrace(false, Target);

	Super::EndEffect_Implementation(Target);
}

void UAbilityEffect_Linetrace::UpdateCueLocation(const FVector& NewLocation)
{
	for (auto& info : InstanceAbilityCues)
	{
		info.SpawnLocation = NewLocation;
	}
}

bool UAbilityEffect_Linetrace::IsAlreadyHit(AActor* Actor)
{
	return HitActors.ContainsByPredicate([Actor](const FHitResult& Hit)
	{
		return Hit.GetActor() == Actor;
	});
}

void UAbilityEffect_Linetrace::ApplyKnockDown(const FHitResult& Hit,class ABaseCharacter* EffectedBy, FGameplayTag KnockdownTag,bool bRotationToHitPoint, bool bReset)
{
	if(const auto hitActor = Hit.GetActor())
	{
		if(hitActor->IsA<ABaseCharacter>())
		{
			const auto character = Cast<ABaseCharacter>(hitActor);
			const auto abComp = character->GetAbilityComponent();
			if(abComp->IsInvincible()==false)
			{
				if(!bReset)
				{
					//넉다운이 이미 적용되어있는지 확인합니다.
					if(abComp->HasAnyActivateAbilityTags(FGameplayTagContainer(KnockdownTag)))
					{
						return;
					}
				}else
				{
					if(abComp->HasAnyActivateAbilityTags(FGameplayTagContainer(KnockdownTag)))
					{
						abComp->ForceEndAbility(KnockdownTag);
					}
				}
				
				if(bRotationToHitPoint)
				{
					auto rot = UKismetMathLibrary::FindLookAtRotation(character->GetActorLocation(),Hit.Location);
					character->SetActorRotation(FRotator(0,rot.Yaw,0));
				}

				abComp->ActivateAbility(KnockdownTag,character);
			}
		}
	}
	
}
