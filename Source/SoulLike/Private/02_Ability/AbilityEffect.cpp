// Fill out your copyright notice in the Description page of Project Settings.


#include "02_Ability/AbilityEffect.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "02_Ability/00_AttributeProcessor/AttributeProcessor.h"
#include "02_Ability/01_Task/GameplayTask_LaunchEvent.h"
#include "96_Library/AbilityHelperLibrary.h"
#include "99_Subsystem/AttackChainSubsystem.h"
#include "99_Subsystem/AttributeProcessSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogEffect)
DEFINE_LOG_CATEGORY(LogTalent)

#if WITH_EDITOR
void UAbilityEffect::CopyValues(UAbilityEffect* Effect)
{
	EffectApplyType = Effect->EffectApplyType;
	Time = Effect->Time;
	IntervalTime = Effect->IntervalTime;
	AttributeEffects = Effect->AttributeEffects;
	AttributeEffectsAffectedByOwnersAttribute = Effect->AttributeEffectsAffectedByOwnersAttribute;
	bRestoreAttributeOnEnd = Effect->bRestoreAttributeOnEnd;
	UniqueEffectTag = Effect->UniqueEffectTag;
	AdditionalEffectTags = Effect->AdditionalEffectTags;
	BannedEffectTags = Effect->BannedEffectTags;
	NeedEffectTags = Effect->NeedEffectTags;
	BannedActivateAbilityTags = Effect->BannedActivateAbilityTags;
	NeedActivateAbilityTags = Effect->NeedActivateAbilityTags;
	InstanceAbilityCues = Effect->InstanceAbilityCues;
	IntervalAbilityCues = Effect->IntervalAbilityCues;
	ChainTagFromAbility = Effect->ChainTagFromAbility;
	bIsHitDamageEffect = Effect->bIsHitDamageEffect;
	AttackType = Effect->AttackType;
}
#endif

void UAbilityEffect::SetExpiredDelegate(const FOnEffectExpired& OnEffectExpiredDel)
{
	OnEffectExpired = OnEffectExpiredDel;
}

void UAbilityEffect::RegisterEffectTag(ABaseCharacter* Target)
{
	if (auto abComp = Target->GetAbilityComponent())
	{
		UE_LOGFMT(LogEffect, Warning, "이팩트 태그 등록 : {0}", UniqueEffectTag.GetTagName());
		abComp->RegisterEffectTag(UniqueEffectTag);

		if (!AdditionalEffectTags.IsEmpty())
		{
			UE_LOGFMT(LogEffect, Warning, "이팩트 추가 태그 등록 : {0}", AdditionalEffectTags.ToString());
			abComp->RegisterEffectTags(AdditionalEffectTags);
		}
	}
}

void UAbilityEffect::UnRegisterEffectTag(ABaseCharacter* Target)
{
	//스택형식인 경우
	if (EffectApplyType == EEffectApplyType::Stack)
	{
		//스택 누적값이 0인 경우에만, 태그를 제거합니다.
		if (Stack.Num() > 0)
		{
			return;
		}
	}

	if (auto abComp = Target->GetAbilityComponent())
	{
		UE_LOGFMT(LogEffect, Warning, "{0}, 이팩트 태그 제거 : {1}", UniqueEffectTag.GetTagName(),
		          UniqueEffectTag.GetTagName());
		abComp->UnRegisterEffectTag(UniqueEffectTag);

		if (!AdditionalEffectTags.IsEmpty())
		{
			UE_LOGFMT(LogEffect, Warning, "{0},이팩트 추가 태그 제거 : {1}", UniqueEffectTag.GetTagName(),
			          AdditionalEffectTags.ToString());
			abComp->UnRegisterEffectTags(AdditionalEffectTags);
		}
	}
}

void UAbilityEffect::ChainSetting(ABaseCharacter* Target, UAbilityBase* From)
{
	if (CauseFromThisAbility != nullptr && bUseSelfChainSystem == false)
	{
		bUseChainSystemFromAbility = CauseFromThisAbility->GetUseChainSystem();
		ChainTagFromAbility = CauseFromThisAbility->GetChainTag();
	}

#if WITH_EDITOR
	if(bUseChainSystemFromAbility)
	{
		UE_LOGFMT(LogEffect,Log,"어빌리티로부터 배율 시스템을 사용하도록 설정되었습니다.");	
	}
#endif
	

	
	if (auto subsystem = UGameplayStatics::GetGameInstance(Target)->GetSubsystem<
		UAttackChainSubsystem>())
	{
		if (bUseSelfChainSystem)
		{
			UE_LOGFMT(LogTemp, Log, "셀프 체인 저장 : {0} , {1}", SelfChainTag.ToString(), SelfChainValue);
			subsystem->AddChainValue(SelfChainTag, SelfChainValue,From);
		}else
		{
			if(From!=nullptr){
				UE_LOGFMT(LogTemp, Log, "어빌리티 체인 저장 : {0} , {1}", From->GetChainTag().ToString(), SelfChainValue);
				subsystem->AddChainValue(ChainTagFromAbility, SelfChainValue,From);
			}
		}
	}
}

void UAbilityEffect::ProcessEffect_Implementation(ABaseCharacter* Target, AActor* EffectBy, UAbilityBase* From,
                                                  UObject* AdditionalData)
{
	ensure(Target);
	ensure(EffectBy);

	UE_LOGFMT(LogEffect, Log, "이팩트 정보 :  {0}, {1}", UniqueEffectTag.ToString(),
	          StaticEnum<EEffectApplyType>()->GetValueAsString(EffectApplyType));

	if (IsRooted() == false)
	{
		UE_LOGFMT(LogEffect, Warning, "루트세트에 추가되었습니다 : {0}", UniqueEffectTag.ToString());
		AddToRoot();
	}

	CauseFromThisAbility = From;
	ChainSetting(Target, CauseFromThisAbility);

#if WITH_EDITOR
	for (auto attributeEffect : AttributeEffects)
	{
		UE_LOGFMT(LogEffect, Log, "{0} : {1}을 {2} 만큼 변화 ,{3} {4}", UniqueEffectTag.ToString(),
		          StaticEnum<EAttributeType>()->GetValueAsString(attributeEffect.Attribute),
		          attributeEffect.ApplyValue, __FUNCTION__, __LINE__);
	}
#endif

	World = Target->GetWorld();

	UAbilityEffectAdditionalInformation* additionalInfo = nullptr;
	if (!bIsHitDamageEffect)
	{
		AttackType = EAttackType::None;
	}
	else
	{
		additionalInfo = CreateAdditionalInfo();
		additionalInfo->HitBy = EffectBy;
		additionalInfo->bTriggerHitAnimation = bTriggerHitAnimation;
	}


	OverrideTime(Target);
	OverrideAttributeEffects(Target, EffectBy);

	if (EffectBy->IsA<ABaseCharacter>())
	{
		UpdateAttributeEffectsAffectedByOwnersAttribute(Cast<ABaseCharacter>(EffectBy));
	}

	RegisterEffectTag(Target);

	if (EffectApplyType == EEffectApplyType::DurationWithInterval)
	{
		ApplyCueInfinite(Target, AdditionalData);
		ApplyDurationEffect(Target);
		ApplyIntervalEffect(Target);
		return;
	}

	if (EffectApplyType == EEffectApplyType::DurationWithTick)
	{
		ApplyCueInfinite(Target, AdditionalData);
		ApplyDurationEffect(Target);
		ApplyTickEffect(Target);
		return;
	}

	if (EffectApplyType == EEffectApplyType::InfiniteWithInterval)
	{
		ApplyCueInfinite(Target, AdditionalData);
		ApplyIntervalEffect(Target);
		return;
	}

	if (EffectApplyType == EEffectApplyType::InfiniteWithTick)
	{
		ApplyCueInfinite(Target, AdditionalData);
		ApplyTickEffect(Target);
		return;
	}


	if (!CanApplyEffect(Target))
	{
		WhenProcessEffectFailed(Target);
	}
	else
	{
		switch (EffectApplyType)
		{
		case EEffectApplyType::Instant:
			ApplyCueInstance(Target,AdditionalData);
			ApplyInstantEffect(Target, additionalInfo);
			EndEffect(Target);
			break;
		case EEffectApplyType::Duration:
			ApplyCueInfinite(Target,AdditionalData);
			ApplyInstantEffect(Target, additionalInfo);
			ApplyDurationEffect(Target);
			break;
		case EEffectApplyType::Infinite:
			ApplyCueInfinite(Target,AdditionalData);
			ApplyInfiniteEffect(Target);
			break;
		case EEffectApplyType::Stack:
			if (From != nullptr)
			{
				UE_LOGFMT(LogTemp, Warning, "{0}으로부터의 스택형 이팩트 적용됨", From->GetAbilityTag().ToString());
			}
			else
			{
				UE_LOGFMT(LogTemp, Error, "받아온 어빌리티가 널포인터입니다.");
			}
			AddStack(From);
			break;
		default: ;
		}
	}
}

void UAbilityEffect::EndEffect_Implementation(ABaseCharacter* Target)
{
	if (IsRooted())
	{
		UE_LOGFMT(LogEffect, Warning, "루트세트에서 제거되었습니다 : {0}", UniqueEffectTag.ToString());
		RemoveFromRoot();
	}

	if (Target != nullptr && Target->GetController() != nullptr)
	{
		if (EffectApplyType == EEffectApplyType::Stack)
		{
			if (Stack.Num() > 0)
			{
				Stack.Pop();
				UE_LOGFMT(LogEffect, Log, "{0} 스택 제거 : {1}", UniqueEffectTag.ToString(), Stack.Num());
			}
			UnRegisterEffectTag(Target);
			RemoveEffectFromAbilityComponent(Target);

			//무한대로 적용된 큐 제거
			Target->GetAbilityComponent()->RemoveSavedCues(InfiniteAbilityCues);
			Target->GetAbilityComponent()->OnEndEffect.Broadcast(
				Target->GetAbilityComponent()->GetActivatedAbilityTags());
		}
		else
		{
			ApplyEndInstanceCue(Target, nullptr);

			if (bRestoreAttributeOnEnd)
			{
				RestoreAttribute(Target);
			}

			if (TickTask.IsValid())
			{
				UE_LOGFMT(LogTemp, Error, "{0},{1}에서 테스크를 제거합니다.", UniqueEffectTag.ToString(), GetName());
				TickTask->OnTaskTickWithDelta.RemoveDynamic(this, &UAbilityEffect::OnTaskTickEvent);
				TickTask->EndTask();
			}

			//돌아가는 타이머가 있다면 제거합니다.
			if (Target->GetWorldTimerManager().TimerExists(IntervalTimerHandle))
			{
				UE_LOGFMT(LogEffect,Log,"주기 타이머 종료");
				Target->GetWorldTimerManager().ClearTimer(IntervalTimerHandle);
			}

			if (Target->GetWorldTimerManager().TimerExists(IntervalCueTimerHandle))
			{
				Target->GetWorldTimerManager().ClearTimer(IntervalCueTimerHandle);
			}

			if (Target->GetWorldTimerManager().TimerExists(DurationTimerHandle))
			{
				Target->GetWorldTimerManager().ClearTimer(DurationTimerHandle);
			}

			UE_LOGFMT(LogTemp, Warning, "UAbilityEffect {0}이 소유한 {1}의 이팩트 종료됨 : {2}",
			          Target->GetController()->GetActorNameOrLabel(), Target->GetActorNameOrLabel(),
			          UniqueEffectTag.ToString());


			UnRegisterEffectTag(Target);
			RemoveEffectFromAbilityComponent(Target);

			//무한대로 적용된 큐 제거
			Target->GetAbilityComponent()->RemoveSavedCues(InfiniteAbilityCues);
			Target->GetAbilityComponent()->OnEndEffect.Broadcast(
				Target->GetAbilityComponent()->GetActivatedAbilityTags());
		}
	}
}

void UAbilityEffect::RemoveEffectFromAbilityComponent(ABaseCharacter* Target)
{
	if (Target)
	{
		//스택형식인 경우
		if (EffectApplyType == EEffectApplyType::Stack)
		{
			//스택 누적값이 0인 경우에만 어빌리티 컴포넌트에서 제거하도록 합니다.
			if (Stack.Num() > 0)
			{
				return;
			}
		}


		if (auto abComp = Target->GetAbilityComponent())
		{
			abComp->RemoveFromAppliedEffectsArray(this);
		}
	}
}

void UAbilityEffect::ApplyAbilityDamageTalent(ABaseCharacter* DamagedCharacter, ABaseCharacter* DamagedBy, FAttributeEffect& AttributeEffect)
{

	if(DamagedBy->IsA<ABaseCharacter>())
	{
		UE_LOGFMT(LogEffect,Log,"{0} : 공격력 특성 적용",DamagedBy->GetActorNameOrLabel());
		if (const auto atComp = Cast<ABaseCharacter>(DamagedBy)->GetAbilityTalentComponent())
		{
			UE_LOGFMT(LogEffect, Warning, "{0}가 {1}에 주는 피해에특성을 적용합니다", DamagedBy->GetActorNameOrLabel(),
					  DamagedCharacter->GetActorNameOrLabel());

			UE_LOGFMT(LogEffect, Warning, "------------------------");
			UE_LOGFMT(LogEffect, Warning, "기본 피해량 : {0}", AttributeEffect.ApplyValue);

			//타격 성공 특성 적용
			{
				atComp->BroadcastOnSuccessHit(DamagedBy, DamagedCharacter);
			}
			//피해량 특성 적용
			{
				const auto& sum = atComp->CalculateModifiedDamageWithTraits(
					DamagedCharacter, DamagedBy, AttackType, AttributeEffect.ApplyValue);
				UE_LOGFMT(LogEffect, Warning, "피해량 특성으로 증가할 예정인 피해량 : {0}", sum);
				AttributeEffect.ApplyValue += sum;
			}

			//회피 성공 피해량 특성 적용
			{
				const auto& sum = atComp->CalculateModifiedDodgeSuccWithTraits(
					Cast<APlayerCharacter>(DamagedBy), AttributeEffect.ApplyValue);
				UE_LOGFMT(LogEffect, Warning, "회피 피해량 특성으로 증가할 예정인 피해량 : {0}", sum);
				AttributeEffect.ApplyValue += sum;
			}


			UE_LOGFMT(LogEffect, Warning, "특성이 적용된 피해량 : {0}", AttributeEffect.ApplyValue);
			UE_LOGFMT(LogEffect, Warning, "------------------------");

			//최종 피해량 특성 적용
			{
				const auto& sum = atComp->CalculateModifiedResultDamageWithTraits(
					DamagedCharacter, DamagedBy, AttackType, AttributeEffect.ApplyValue);
				UE_LOGFMT(LogEffect, Warning, "피해량 특성으로 증가할 예정인 피해량 : {0}", sum);
				AttributeEffect.ApplyValue += sum;
				UE_LOGFMT(LogEffect, Warning, "최종 피해량 특성이 적용된 피해량 : {0}", AttributeEffect.ApplyValue);
				UE_LOGFMT(LogEffect, Warning, "------------------------");
			}
			
		}
	}
	
}

void UAbilityEffect::ApplyAbilityDecreaseDamageTalent(ABaseCharacter* DamagedCharacter, ABaseCharacter* DamagedBy, FAttributeEffect& AttributeEffect, TArray<FAttributeEffect
                                                      >& EffectsByTalent)
{
	if (DamagedCharacter->IsA<ABaseCharacter>())
	{
		UE_LOGFMT(LogEffect,Log,"{0}의 방어력 특성 적용, {1} 에게 피해를 입음",DamagedCharacter->GetActorNameOrLabel(),DamagedBy->GetActorNameOrLabel());
		
		//방어력 특성 적용
		if (const auto atComp = Cast<ABaseCharacter>(DamagedCharacter)->GetAbilityTalentComponent())
		{
			UE_LOGFMT(LogEffect, Warning, "------------------------");
			UE_LOGFMT(LogEffect, Warning, "{0} : 기본 피해량 : {1}",DamagedCharacter->GetActorNameOrLabel(), AttributeEffect.ApplyValue);
			{
				const auto& sum = atComp->CalculateModifiedDecreaseDamageWithTraits(
					DamagedCharacter, DamagedBy, AttackType, AttributeEffect.ApplyValue);
				UE_LOGFMT(LogEffect, Warning, "{0} : 방어 특성으로 감소할 예정인 피해량 : {1}",DamagedCharacter->GetActorNameOrLabel(), sum);
				AttributeEffect.ApplyValue -= sum;

				const auto& increaseDamage = atComp->CalculateModifiedIncreaseGotHitDamageWithTraits(
					DamagedCharacter, DamagedBy, AttackType, AttributeEffect.ApplyValue);
				UE_LOGFMT(LogEffect, Warning, "{0} : 입는 피해량 증가 특성으로 증가할 피해량 : {1}",DamagedCharacter->GetActorNameOrLabel(), increaseDamage);
				AttributeEffect.ApplyValue += increaseDamage;
			}


			UE_LOGFMT(LogEffect, Warning, "{0} : 모든 특성이 적용된 피해량 : {1}",DamagedCharacter->GetActorNameOrLabel(), AttributeEffect.ApplyValue);
		

			if (auto attComp = Cast<ABaseCharacter>(DamagedCharacter)->GetAttributeComponent())
			{
				//MP로 받을 피해량이 저장될 구조체
				FAttributeEffect mpEffect;
				//피해량 MP치환 특성 적용
			
				if (AttributeEffect.Attribute == EAttributeType::HP)
				{
					//지금 플레이어가 가진 MP수치를 가져옵니다.
					const auto& mp = attComp->GetMP();
					UE_LOGFMT(LogEffect, Warning, "현재 MP : {0}", mp);
					//MP로 치환될 피해량 수치
					const auto& toMp = FMath::Clamp(
						atComp->CalculateModifiedDamageToMPWithTraits(
							DamagedCharacter, DamagedBy, AttributeEffect.ApplyValue), 0, mp);
					UE_LOGFMT(LogEffect, Warning, "MP로 치환될 피해량 : {0}", toMp);

					//mp량을 초과하는 수치
					const auto& overMP = FMath::Clamp(toMp - mp, 0, mp);
					UE_LOGFMT(LogEffect, Warning, "MP를 초과한 피해량 : {0}", overMP);

					//MP로 받을 피해량 저장
					mpEffect.ApplyValue = toMp;
					mpEffect.Attribute = EAttributeType::MP;
					mpEffect.ApplyMethod = EAttributeApplyMethod::REMOVE;
					UE_LOGFMT(LogEffect, Warning, "MP로 받을 피해량 : {0}", mpEffect.ApplyValue);

					//치환되고 남은 피해량
					const auto& remainDamage = FMath::Clamp(AttributeEffect.ApplyValue - toMp, 0, AttributeEffect.ApplyValue) +
						overMP;
					UE_LOGFMT(LogEffect, Warning, "치환되고 남은 HP 피해량 : {0}", remainDamage);

					//남은 피해량을 HP로
					AttributeEffect.ApplyValue = remainDamage;
				}
				EffectsByTalent.Emplace(mpEffect);
				
				/*//TODO 여기서 문제가 됨.
				UpdatedAttributeEffectsAffectedByOwnersAttribute.Add(mpEffect);*/
			}
		}
	}
	
}

void UAbilityEffect::ApplyGotHitTalent(ABaseCharacter* DamagedCharacter, float OriginalDamage, UObject* AdditionalInfo)
{
	if (bIsHitDamageEffect)
	{
		UE_LOGFMT(LogEffect,Log,"{0} : 피해 입었을 때, 특성 적용",DamagedCharacter->GetActorNameOrLabel());
		if (DamagedCharacter->IsA<ABaseCharacter>() && AdditionalInfo != nullptr)
		{
			if (const UAbilityEffectAdditionalInformation* addInfo = Cast<UAbilityEffectAdditionalInformation>(
				AdditionalInfo))
			{
				const auto damagedBy = Cast<ABaseCharacter>(addInfo->HitBy.Get());
				//피격 특성 호출
				if (const auto atComp = Cast<ABaseCharacter>(DamagedCharacter)->GetAbilityTalentComponent())
				{
					atComp->BroadcastOnGotHit(Cast<ABaseCharacter>(DamagedCharacter), damagedBy, OriginalDamage);
				}
			}
		}
	}
}

float UAbilityEffect::ApplyChangeHealAmountTalent(ABaseCharacter* Target, FAttributeEffect AttributeEffect) const
{
	if (bIsHitDamageEffect)
	{
		return AttributeEffect.ApplyValue;
	}

	if (Target->IsA<ABaseCharacter>())
	{
		UE_LOGFMT(LogEffect,Log,"{0} : 힐량 특성 적용",Target->GetActorNameOrLabel());
		if (auto atComp = Cast<ABaseCharacter>(Target)->GetAbilityTalentComponent())
		{
			if (AttributeEffect.Attribute == EAttributeType::HP)
			{
				AttributeEffect.ApplyValue += atComp->BroadcastChangeHealAmount(Target, AttributeEffect.ApplyValue);

				return AttributeEffect.ApplyValue;
			}
		}
	}

	return 0;
}

void UAbilityEffect::RestoreAttribute_Implementation(ABaseCharacter* Target)
{
	if (bRestoreAttributeOnEnd)
	{
		UE_LOGFMT(LogEffect, Log, "적용된 어트리뷰트를 되돌립니다 : {0}", UniqueEffectTag.ToString());
		if (auto atComp = Target->GetAttributeComponent())
		{
			const auto attributeProcessSubsystem = GetAttributeProcessSubsystem(Target);
			if (attributeProcessSubsystem->IsValidLowLevel())
			{
				for (auto iter : AffectedValueFromAttributeEffects)
				{
					if (const auto processor = attributeProcessSubsystem->
						GetReverseProcessor(iter.Value.Effect.ApplyMethod))
					{
						processor->ProcessAttributeEffect(atComp, iter.Value.Effect);
					}
				}
				AffectedValueFromAttributeEffects.Empty();

				for (auto iter : AffectedValueAttributeEffectsAffectedByOwnersAttribute)
				{
					if (const auto processor = attributeProcessSubsystem->
						GetReverseProcessor(iter.Value.Effect.ApplyMethod))
					{
						processor->ProcessAttributeEffect(atComp, iter.Value.Effect);
					}
				}
				AffectedValueAttributeEffectsAffectedByOwnersAttribute.Empty();
			}
		}
	}
}

void UAbilityEffect::UpdateCharacterInfoWidget(ABaseCharacter* Target)
{
	Target->GetAttributeComponent()->OnCharacterInformationUpdate.Broadcast();
}

bool UAbilityEffect::ApplyInstantEffect_Implementation(ABaseCharacter* Target, UObject* AdditionalInfo, float DeltaTime)
{
	ensure(Target);
	UE_LOGFMT(LogEffect,Log,"ApplyInstantEffect_Implementation");
	if (!CanApplyEffect(Target))
	{
		UE_LOGFMT(LogEffect, Error, "즉발 이팩트를 적용할 수 없습니다 : {0} {1}", __FUNCTION__, __LINE__);
		WhenProcessEffectFailed(Target);
		return false;
	}

	if(!Target->GetAttributeComponent()->IsValidLowLevel())
	{
		return false;
	}
	
	if (!GetAttributeProcessSubsystem(Target)->IsValidLowLevel())
	{
		return false;
	}
	

	//피해를 위한 이팩트인가요?
	if(bIsHitDamageEffect)
	{
		//무적에 대한 처리
		if (Target->GetAbilityComponent()->IsInvincible())
		{
			UE_LOGFMT(LogEffect, Error, "{0}는 무적 상태입니다.", Target->GetActorNameOrLabel());
			if (auto character = Cast<ABaseCharacter>(Target))
			{
				//무적이고, 회피태그가 있을 경우에
				const auto& dodgeTag = UAbilityHelperLibrary::GetDodgeTag(character->GetInventoryComponent());
				if (Target->GetAbilityComponent()->HasAllActivateAbilityTags(FGameplayTagContainer(dodgeTag)))
				{
					character->GetAbilityTalentComponent()->BroadcastOnSuccessDodge(character);
				}
			}

			return false;
		}

		// 고정 피해 처리
		ProcessDamageAttributeEffects(AttributeEffects, Target, AdditionalInfo, DeltaTime);
		// 일반 피해 처리
		ProcessDamageAttributeEffects(UpdatedAttributeEffectsAffectedByOwnersAttribute, Target, AdditionalInfo, DeltaTime);
			
		UpdateCharacterInfoWidget(Target);
		
		return true;
	}

	//피해가 아닌 다른 어트리뷰트에 대한 처리
	ProcessAttributeEffects(AttributeEffects, Target, AdditionalInfo, DeltaTime);
	ProcessAttributeEffects(UpdatedAttributeEffectsAffectedByOwnersAttribute, Target, AdditionalInfo, DeltaTime);

	UpdateCharacterInfoWidget(Target);
	
	return true;
}

void UAbilityEffect::ProcessDamageAttributeEffects(const TArray<FAttributeEffect>& AttributeEffectsArray,
                                                   ABaseCharacter* Target, UObject* AdditionalInfo, float DeltaTime)
{
	if (auto atComp = Target->GetAttributeComponent())
	{
		const auto attributeProcessSubsystem = GetAttributeProcessSubsystem(Target);
		if (attributeProcessSubsystem->IsValidLowLevel())
		{
			//특성으로 인해서 추가로 적용되어야 할 것들을 여기에 저장합니다.
			TArray<FAttributeEffect> effectsByTalent;
			
			for (auto attributeEffect : AttributeEffectsArray)
			{
				if (const auto processor = attributeProcessSubsystem->GetProcessor(attributeEffect.ApplyMethod))
				{
					if (UAbilityEffectAdditionalInformation* addInfo = Cast<UAbilityEffectAdditionalInformation>(AdditionalInfo))
					{
						if (addInfo->HitBy != nullptr)
						{
							// 1. 공격 배율 적용
							attributeEffect.ApplyValue = ApplyAttackChain(Target, attributeEffect.ApplyValue,Cast<ABaseCharacter>(addInfo->HitBy.Get()));
							UE_LOGFMT(LogEffect,Log,"공격 배율이 적용된 후의 값 : {0}",attributeEffect.ApplyValue);
							// 2. 방어력 적용
							attributeEffect.ApplyValue = ApplyDefence(Target, AdditionalInfo, attributeEffect.ApplyValue);
							// 3. 특성 적용

					
							// 3-0. 피해를 주는 대상의 공격 성공 시 호출되는 특성 (주로 피해량 증감 특성)
							ApplyAbilityDamageTalent(Target, Cast<ABaseCharacter>(addInfo->HitBy.Get()), attributeEffect);
							// 3-1. 피격 대상의 피해 감소 특성
							ApplyAbilityDecreaseDamageTalent(Target, Cast<ABaseCharacter>(addInfo->HitBy.Get()), attributeEffect,effectsByTalent);
						}
					}
					// 3-2. 피격 시 호출되는 특성 (예: 피격 당하면 폭발합니다)
					ApplyGotHitTalent(Target, attributeEffect.ApplyValue, AdditionalInfo);


					const auto applyValue = processor->ProcessAttributeEffect(atComp, attributeEffect, DeltaTime, AdditionalInfo);

					if (bRestoreAttributeOnEnd)
					{
						AffectedValueFromAttributeEffects.Add(attributeEffect.Attribute,
															  FAttributeRestoreData(
																  attributeEffect, applyValue));
					}
				}
			}
			

			//특성으로 추가로 적용되는 것들에 대해서는 특성을 다시 적용하지 않도록 합니다.
			for (auto attributeEffect : effectsByTalent)
			{
				if (const auto processor = attributeProcessSubsystem->GetProcessor(attributeEffect.ApplyMethod))
				{
					const auto applyValue = processor->ProcessAttributeEffect(atComp, attributeEffect, DeltaTime, AdditionalInfo);
				}
			}
		}
	}
}

void UAbilityEffect::ProcessAttributeEffects(const TArray<FAttributeEffect>& AttributeEffectsArray, ABaseCharacter* Target,
                                             UObject* AdditionalInfo, float DeltaTime)
{
	if (auto atComp = Target->GetAttributeComponent())
	{
		const auto attributeProcessSubsystem = GetAttributeProcessSubsystem(Target);
		if (attributeProcessSubsystem->IsValidLowLevel())
		{
			for (auto& attributeEffect : AttributeEffectsArray)
			{
				if (const auto processor = attributeProcessSubsystem->GetProcessor(attributeEffect.ApplyMethod))
				{
					const auto applyValue = processor->ProcessAttributeEffect(atComp, attributeEffect, DeltaTime, AdditionalInfo);

					if (bRestoreAttributeOnEnd)
					{
						AffectedValueFromAttributeEffects.Add(attributeEffect.Attribute,
															  FAttributeRestoreData(
																  attributeEffect, applyValue));
					}
				}
			}
		}
	}
}

void UAbilityEffect::ApplyDurationEffect(ABaseCharacter* Target)
{
	ensure(Target);
	FTimerManager& timerManager = Target->GetWorldTimerManager();

	if (timerManager.TimerExists(DurationTimerHandle))
	{
		UE_LOGFMT(LogEffect, Log, "지속시간 타이머 해제 {0} {1}", __FUNCTION__, __LINE__);
		timerManager.ClearTimer(DurationTimerHandle);
	}


	FTimerDelegate endTimerDel = FTimerDelegate::CreateUObject(this, &UAbilityEffect::EndEffect, Target);
	timerManager.SetTimer(DurationTimerHandle, endTimerDel, Time, false);
}

void UAbilityEffect::ApplyIntervalEffect(ABaseCharacter* Target)
{
	ensure(Target);
	UE_LOGFMT(LogEffect,Log,"ApplyIntervalEffect");

	FTimerManager& timerManager = Target->GetWorldTimerManager();

	if (timerManager.TimerExists(IntervalTimerHandle))
	{
		UE_LOGFMT(LogEffect, Log, "주기 타이머 해제 {0} {1}", __FUNCTION__, __LINE__);
		timerManager.ClearTimer(IntervalTimerHandle);
	}

	if (timerManager.TimerExists(IntervalCueTimerHandle))
	{
		UE_LOGFMT(LogEffect, Log, "주기 큐 타이머 해제 {0} {1}", __FUNCTION__, __LINE__);
		timerManager.ClearTimer(IntervalCueTimerHandle);
	}


	const FTimerDelegate intervalTimerDel = FTimerDelegate::CreateLambda([Target, this]()
	{
		ApplyInstantEffect(Target, nullptr);
	});
	timerManager.SetTimer(IntervalTimerHandle, intervalTimerDel, IntervalTime, true, 0);

	FTimerDelegate intervalCueTimerDel;
	intervalCueTimerDel.BindUFunction(this, FName("ApplyCueInterval"), Target, nullptr);
	timerManager.SetTimer(IntervalCueTimerHandle, intervalCueTimerDel, IntervalTime, true, 0);
}

void UAbilityEffect::ApplyInfiniteEffect(ABaseCharacter* Target)
{
	ensure(Target);
	UE_LOGFMT(LogEffect, Log, "영구 이팩트 적용 :{0}", AttributeEffects.Num());
#if WITH_EDITOR
	for (auto attributeEffect : AttributeEffects)
	{
		UE_LOGFMT(LogEffect, Log, "{0}을 {1} 만큼 변화 : {2} {3}",
		          StaticEnum<EAttributeType>()->GetValueAsString(attributeEffect.Attribute),
		          attributeEffect.ApplyValue, __FUNCTION__, __LINE__);
	}
#endif

	ApplyInstantEffect(Target);
}

void UAbilityEffect::ApplyTickEffect(ABaseCharacter* Target)
{
	ensure(Target);
	if (TickTask.IsValid())
	{
		if (TickTask->IsActive())
		{
			TickTask->EndTask();
		}
	}

	if (TickTask.IsValid() == false)
	{
		TickTask = UGameplayTask_LaunchEvent::LaunchEvent(Target, nullptr, TickRate);
		UE_LOGFMT(LogTemp, Warning, "{0},{1}에서 테스크를 생성합니다", UniqueEffectTag.ToString(), GetName());
	}

	//TickTask->OnTaskTickWithDelta.RemoveDynamic(this, &UAbilityEffect::OnTaskTickEvent);


	TickOwner = Target;
	if (TickTask.IsValid())
	{
		TickTask->OnTaskTickWithDelta.AddUniqueDynamic(this, &UAbilityEffect::OnTaskTickEvent);
		TickTask->SetTickRate(TickRate);
		TickTask->ReadyForActivation();
	}
}

void UAbilityEffect::AddStack(UAbilityBase* From)
{
	Stack.Push(true);
	if (From != nullptr)
	{
		UE_LOGFMT(LogEffect, Log, "{0}으로부터 {1} 스택 누적 : {0}", From->GetAbilityTag().ToString(),
		          UniqueEffectTag.ToString(), Stack.Num());
	}
}

float UAbilityEffect::ApplyAttackChain(const ABaseCharacter* Target, float Original, ABaseCharacter* Causer) const
{
	ensure(Target);
	
	if (Target->IsA<ABaseCharacter>())
	{
		if (auto subsystem = UGameplayStatics::GetGameInstance(Target)->GetSubsystem<UAttackChainSubsystem>())
		{
			if (bUseChainSystemFromAbility)
			{
				const auto result = Original * subsystem->GetChainValue(ChainTagFromAbility);
				UE_LOGFMT(LogEffect, Error, "{0} : {1}의 배율을 적용합니다. 기본값 : {2}, 배율 결과값 : {3}", UniqueEffectTag.ToString(), Causer->GetName(),Original,
				          result);
				return result;
			}

			if (bUseSelfChainSystem)
			{
				const auto result = Original * subsystem->GetChainValue(SelfChainTag);
				UE_LOGFMT(LogEffect, Error, "{0} : {1}의 셀프 배율을 적용합니다. 기본값 : {2}, 배율 결과값 : {3}", UniqueEffectTag.ToString(), Causer->GetName(),Original,
							  result);
				return result;
			}
		}else
		{
			UE_LOGFMT(LogEffect,Error,"배율시스템을 가져올 수 없습니다.");
		}
	}

	return Original;
}

float UAbilityEffect::ApplyDefence(ABaseCharacter* DamagedCharacter, UObject* AdditionalInfo, const float& Damage)
{
	if (!bIsHitDamageEffect)
	{
		return Damage;
	}

	if (DamagedCharacter->IsA<ABaseCharacter>() && AdditionalInfo != nullptr)
	{
		if (auto addInfo = Cast<UAbilityEffectAdditionalInformation>(AdditionalInfo))
		{
			if (auto atComp = Cast<ABaseCharacter>(DamagedCharacter)->GetAbilityTalentComponent())
			{
				float def = 0;
				switch (AttackType)
				{
				case EAttackType::Physical:
					def = atComp->
						CalculateModifiedPhysicalDefenceWithTraits(Cast<ABaseCharacter>(addInfo->HitBy.Get()));
					break;
				case EAttackType::Magical:
					def = atComp->CalculateModifiedMagicalDefenceWithTraits(Cast<ABaseCharacter>(addInfo->HitBy.Get()));
					break;
				}

				def = atComp->CalculateModifiedDefenceByMonsterTypeWithTraits(
					def, Cast<ABaseCharacter>(addInfo->HitBy.Get()));

				UE_LOGFMT(LogEffect, Warning, "{0}의 특성으로 변경된 방어력 : {1}, 최종 피해량 : {2}",
				          DamagedCharacter->GetActorNameOrLabel(), def,
				          FMath::Clamp(Damage - def, 0, Damage + FMath::Abs(def)));
				return FMath::Clamp(Damage - def, 0, Damage + FMath::Abs(def));
			}
		}
	}
	else
	{
		if (auto atComp = DamagedCharacter->GetAttributeComponent())
		{
			switch (AttackType)
			{
			case EAttackType::Physical:
				UE_LOGFMT(LogEffect, Warning, "{0}의 물리방어력 적용 : {1}", DamagedCharacter->GetActorNameOrLabel(),
				          atComp->GetPhysicalDefense());
				return FMath::Clamp(Damage - atComp->GetPhysicalDefense(), 0, Damage);
			case EAttackType::Magical:
				UE_LOGFMT(LogEffect, Warning, "{0}의 마법방어력 적용 : {1}", DamagedCharacter->GetActorNameOrLabel(),
				          atComp->GetMagicalDefense());
				return FMath::Clamp(Damage - atComp->GetMagicalDefense(), 0, Damage);
			}

			//UE_LOGFMT(LogTemp, Warning, "{0}의 방어력이 적용되고 난 후 입을 피해량 : {1}", DamagedCharacter->GetActorNameOrLabel(), Damage);
		}
	}


	return Damage;
}

bool UAbilityEffect::CanApplyEffect_Implementation(ABaseCharacter* Target, bool bShowLog) const
{
	ensure(Target);
	if (auto abComp = Target->GetAbilityComponent())
	{
		if (Target->GetCharacterState() == ECharacterState::DEAD)
		{
			if (bShowLog)
			{
				UE_LOGFMT(LogEffect, Error, "이팩트를 적용할 대상이 사망상태라 적용할 수 없습니다.");
			}
			return false;
		}

		bool bApply = true;

		bApply &= !HasAnyBannedEffectTags(abComp);
		if (bApply == false)
		{
			if (bShowLog)
			{
				UE_LOGFMT(LogEffect, Error, "금지된 이팩트 태그를 가지고 있어서 대상에게 어빌리티 이팩트를 적용할 수 없습니다.");
			}
			return false;
		}

		bApply &= HasAllNeedEffectTags(abComp);
		if (bApply == false)
		{
			if (bShowLog)
			{
				UE_LOGFMT(LogEffect, Error, "필요한 이팩트 태그를 모두 가지고 있지 않아 대상에게 어빌리티 이팩트를 적용할 수 없습니다.");
			}


			return false;
		}

		bApply &= !HasAnyBannedAbilityTags(abComp);
		if (bApply == false)
		{
			if (bShowLog)
			{
				UE_LOGFMT(LogEffect, Error, "금지된 어빌리티 태그를 가지고 있어서 대상에게 어빌리티 이팩트를 적용할 수 없습니다.");
			}

			return false;
		}

		bApply &= HasAllNeedAbilityTags(abComp);
		if (bApply == false)
		{
			if (bShowLog)
			{
				UE_LOGFMT(LogEffect, Error, "필요한 어빌리티 태그를 모두 가지고 있지 않아 대상에게 어빌리티 이팩트를 적용할 수 없습니다.");
			}


			return false;
		}

		return bApply;
	}


	return false;
}

bool UAbilityEffect::HasAnyBannedEffectTags(UAbilityComponent* AbComp) const
{
	return AbComp->HasAnyEffectTag(BannedEffectTags);
}

bool UAbilityEffect::HasAllNeedEffectTags(UAbilityComponent* AbComp) const
{
	return AbComp->HasAllEffectTag(NeedEffectTags);
}

bool UAbilityEffect::HasAnyBannedAbilityTags(UAbilityComponent* AbComp) const
{
	return AbComp->HasAnyActivateAbilityTags(BannedActivateAbilityTags);
}

bool UAbilityEffect::HasAllNeedAbilityTags(UAbilityComponent* AbComp) const
{
	return AbComp->HasAllActivateAbilityTags(NeedActivateAbilityTags);
}

UAttributeProcessSubsystem* UAbilityEffect::GetAttributeProcessSubsystem(ABaseCharacter* Target)
{
	if (Target != nullptr)
	{
		return UGameplayStatics::GetGameInstance(Target)->GetSubsystem<UAttributeProcessSubsystem>();
	}

	return nullptr;
}

bool UAbilityEffect::UpdateAttributeEffectsAffectedByOwnersAttribute_Implementation(
	ABaseCharacter* Target)
{
	ensure(Target);

	if (const auto manager = GetAttributeProcessSubsystem(Target))
	{
		UpdatedAttributeEffectsAffectedByOwnersAttribute = manager->UpdateAttributeEffectsAffectedByOwnersAttribute(
			Cast<ABaseCharacter>(Target)->GetAttributeComponent(),
			AttributeEffectsAffectedByOwnersAttribute, AttackType);

		return true;
	}
		UE_LOGFMT(LogTemp, Error, "{0} : 속성 처리 서브시스템을 가져올 수 없어요!", Target->GetActorNameOrLabel());
	

	return false;
}

void UAbilityEffect::OverrideAttributeEffects_Implementation(ABaseCharacter* Target, AActor* EffectBy)
{
}

void UAbilityEffect::OnTaskTickEvent_Implementation(float DeltaTime)
{
	TickEffectDeltaTime = DeltaTime;

	if (TickOwner.IsValid())
	{
		if (!CanApplyEffect(TickOwner.Get(), false))
		{
			WhenProcessEffectFailed(TickOwner.Get());
			return;
		}

		UpdateAttributeEffectsAffectedByOwnersAttribute(TickOwner.Get());

		const auto manager = GetAttributeProcessSubsystem(TickOwner.Get());
		if (manager->IsValidLowLevel())
		{
			if (const auto atComp = TickOwner->GetAttributeComponent())
			{
				for (auto attributeEffect : AttributeEffects)
				{
					if (const auto processor = manager->GetProcessor(attributeEffect.ApplyMethod))
					{
						if (processor->IsValidLowLevel())
						{
							const float& applyValue = processor->ProcessAttributeEffect(
								atComp, attributeEffect, DeltaTime);
						}
					}
				}


				for (auto attributeEffect : UpdatedAttributeEffectsAffectedByOwnersAttribute)
				{
					if (const auto processor = manager->GetProcessor(attributeEffect.ApplyMethod))
					{
						if (processor->IsValidLowLevel())
						{
							const float& applyValue = processor->ProcessAttributeEffect(
								atComp, attributeEffect, DeltaTime);
						}
					}
				}
			}
			else
			{
				UE_LOGFMT(LogEffect, Error, "어트리뷰트 컴포넌트를 가져올 수 없습니다.");
			}
		}
		else
		{
			UE_LOGFMT(LogEffect, Error, "속성 메니저가 유효하지 않습니다.");
		}
	}
}

void UAbilityEffect::UpdateCueDataFromAdditionalData(UObject* AdditionalData, TArray<FAbilityCueInformation>& CueInfos)
{
	if(auto cueInfo = Cast<UAbilityCueAdditionalInformation>(AdditionalData))
	{
		for(auto &iter : CueInfos)
		{
			iter.ImpactNormal = cueInfo->ImpactNormal;
			iter.SpawnLocation = cueInfo->HitLocation;
		}
	}
}

void UAbilityEffect::ApplyCueInstance(ABaseCharacter* Target, UObject* AdditionalData)
{
	ensure(Target);
	if (auto abComp = Target->GetAbilityComponent())
	{
		UpdateCueDataFromAdditionalData(AdditionalData, InstanceAbilityCues);
		AppliedCues = abComp->ApplyCues(InstanceAbilityCues);
	}
}

void UAbilityEffect::ApplyCueInterval(ABaseCharacter* Target, UObject* AdditionalData)
{
	ensure(Target);
	if (auto abComp = Target->GetAbilityComponent())
	{
		UE_LOGFMT(LogEffect,Log,"ApplyCueInterval");
		UpdateCueDataFromAdditionalData(AdditionalData, IntervalAbilityCues);
		abComp->ApplyCues(IntervalAbilityCues);
	}
}

void UAbilityEffect::ApplyCueInfinite(ABaseCharacter* Target, UObject* AdditionalData)
{
	ensure(Target);
	if (auto abComp = Target->GetAbilityComponent())
	{
		for (auto& iter : InfiniteAbilityCues)
		{
			iter.bIsNotInstanceCue = true;
		}
		UpdateCueDataFromAdditionalData(AdditionalData, InfiniteAbilityCues);
		abComp->ApplyCues(InfiniteAbilityCues);
	}
}

void UAbilityEffect::ApplyEndInstanceCue(ABaseCharacter* Target, UObject* AdditionalData)
{
	ensure(Target);
	if (auto abComp = Target->GetAbilityComponent())
	{
		UpdateCueDataFromAdditionalData(AdditionalData, InstanceEndAbilityCues);
		abComp->ApplyCues(InstanceEndAbilityCues);
	}
}

void UAbilityEffect::WhenProcessEffectFailed(ABaseCharacter* Target)
{
	ensure(Target);
	switch (ApplyFailedAction)
	{
	case EApplyFailedAction::NoAction:
		break;
	case EApplyFailedAction::EndEffect:
		EndEffect(Target);
		break;
	case EApplyFailedAction::EndAbility:
		OnEffectExpired.ExecuteIfBound();
		break;
	default: ;
	}
}

UAbilityEffectAdditionalInformation* UAbilityEffect::CreateAdditionalInfo_Implementation()
{
	return NewObject<UAbilityEffectAdditionalInformation>(this);
}

void UAbilityEffect::RemoveAppliedCues(ABaseCharacter* Target)
{
	ensure(Target);

	for (auto i = AppliedCues.Num() - 1; i >= 0; i--)
	{
		auto c = AppliedCues[i];
		if (c->IsValidLowLevel())
		{
			if (!c->IsReusable())
			{
				AppliedCues.RemoveAt(i);
				c->Destroy();
			}
			else
			{
				c->DeactivateCue();
			}
		}
	}

	AppliedCues.Empty();
}

void UAbilityEffect::ResetDurationTimer(ABaseCharacter* Target)
{
	ensure(Target);
	if (Target)
	{
		if (EffectApplyType == EEffectApplyType::DurationWithInterval)
		{
			ApplyDurationEffect(Target);
			ApplyIntervalEffect(Target);
			return;
		}

		if (EffectApplyType == EEffectApplyType::DurationWithTick)
		{
			ApplyDurationEffect(Target);
			ApplyTickEffect(Target);
			return;
		}

		if (EffectApplyType == EEffectApplyType::Duration)
		{
			ApplyDurationEffect(Target);
			ApplyTickEffect(Target);
		}
	}
}

void UAbilityEffect::BeginDestroy()
{
	if (IsRooted())
	{
		RemoveFromRoot();
	}

	if (World)
	{
		//돌아가는 타이머가 있다면 제거합니다.
		if (GetWorld()->GetTimerManager().TimerExists(IntervalTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(IntervalTimerHandle);
		}

		if (GetWorld()->GetTimerManager().TimerExists(IntervalCueTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(IntervalCueTimerHandle);
		}

		if (GetWorld()->GetTimerManager().TimerExists(DurationTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(DurationTimerHandle);
		}
	}

	UObject::BeginDestroy();
}

UWorld* UAbilityEffect::GetWorld() const
{
	return World;
}
