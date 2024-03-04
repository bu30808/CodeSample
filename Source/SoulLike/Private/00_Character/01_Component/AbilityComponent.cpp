// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/01_Component/AbilityComponent.h"

#include "TimerManager.h"
#include "02_Ability/AbilityBase.h"
#include "02_Ability/AbilityEffect.h"
#include "03_Widget/07_NPC/NPCActionButtonWidget.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogAbilityComponent)

void UAbilityCueAdditionalInformation::UpdateCueLocation(TArray<FAbilityCueInformation>& CueInfos) const
{
	for(auto &c : CueInfos)
	{
		c.SpawnLocation = HitLocation;
	}
}

// Sets default values for this component's properties
UAbilityComponent::UAbilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
}

UAbilityBase* UAbilityComponent::GetAbilityByTag(FGameplayTag AbilityTag)
{
	const auto findAbility = AvailableAbilities.FindByPredicate([AbilityTag](class UAbilityBase* ability)
	{
		return ability && ability->GetAbilityTag().MatchesTagExact(AbilityTag);
	});

	if (findAbility != nullptr)
	{
		return *findAbility;
	}

	return nullptr;
}

bool UAbilityComponent::HasAbility(FGameplayTag AbilityTag)
{
	if (GetAbilityByTag(AbilityTag) != nullptr)
	{
		return true;
	}
	return false;
}

bool UAbilityComponent::GiveAbility(TSubclassOf<UAbilityBase> Ability)
{
	if (Ability == nullptr)
	{
		UE_LOGFMT(LogTemp, Error, "받아온 어빌리티가 NULL입니다 {0} {1}", __FUNCTION__, __LINE__);
		return false;
	}
	const auto& abilityTag = Ability.GetDefaultObject()->GetAbilityTag();
	if (abilityTag.IsValid() && !HasAbility(abilityTag))
	{
		
		//CDO를 복제한 뒤, 부여합니다
		if (auto copy = DuplicateObject(Ability.GetDefaultObject(), GetOwner()))
		{
			UE_LOGFMT(LogTemp, Warning, "어빌리티 등록 : {0}", abilityTag.GetTagName());
			AvailableAbilities.Emplace(copy);
			OnAddAbility.Broadcast(copy->GetAbilityInformation());
		}
		/*UE_LOGFMT(LogTemp, Warning, "어빌리티 등록 : {0}", abilityTag.GetTagName());
		AvailableAbilities.Emplace(Ability.GetDefaultObject());
		OnAddAbility.Broadcast(Ability.GetDefaultObject()->GetAbilityInformation());*/

		return true;
	}
	UE_LOGFMT(LogTemp, Error, "이미 등록중인 어빌리티입니다 : {0}", abilityTag.GetTagName());
	return false;
}

void UAbilityComponent::RemoveAbility(FGameplayTag AbilityTag)
{
	if (const auto ability = GetAbilityByTag(AbilityTag))
	{
		UE_LOGFMT(LogTemp, Log, "어빌리티를 컴포넌트에서 제거합니다 : {0}", AbilityTag.ToString());
		AvailableAbilities.Remove(ability);
		//ability->ConditionalBeginDestroy();
		OnRemoveAbility.Broadcast(AbilityTag);
	}
}

bool UAbilityComponent::IsAlreadyActivated(FGameplayTag AbilityTag)
{
	return ActivatedAbilityTags.HasTagExact(AbilityTag);
}

bool UAbilityComponent::ActivateAbility(FGameplayTag AbilityTag, AActor* AbilityTarget,
                                        /* class UInputAction* InputAction,*/
                                        UObject* AdditionalInfo)
{
	//이미 사용중인 어빌리티인지 체크합니다.
	if (!IsAlreadyActivated(AbilityTag))
	{
		if (const auto ability = GetAbilityByTag(AbilityTag))
		{
			ability->OnEndAbility.AddUniqueDynamic(this, &UAbilityComponent::OnEndAbilityEvent);
			HandleAbilityAdditionalData(ability, AdditionalInfo);

			OnActivatedAbility.Broadcast(ability->GetAbilityInformation());
			ability->InitAbility(GetOwner<ABaseCharacter>(), AbilityTarget);
			ability->TryActivateAbility(AdditionalInfo);

			return true;
		}
		else
		{
			UE_LOGFMT(LogTemp, Error, "사용하려는 어빌리티를 찾을 수 없습니다 : {0}", AbilityTag.GetTagName());
		}
	}
	else
	{
		UE_LOGFMT(LogAbilityComponent, Error, "{0}는 이미 사용중입니다.", AbilityTag.ToString());
	}

	return false;
}

bool UAbilityComponent::HasAnyActivateAbilityTags(const FGameplayTagContainer& Tags)
{
	return ActivatedAbilityTags.HasAny(Tags);
}

bool UAbilityComponent::HasAllActivateAbilityTags(const FGameplayTagContainer& Tags)
{
	return ActivatedAbilityTags.HasAll(Tags);
}

void UAbilityComponent::RegisterAbilityTag(FGameplayTag ActivateAbilityTag)
{
	if (ActivatedAbilityTags.HasTagExact(ActivateAbilityTag))
	{
		UE_LOGFMT(LogAbilityComponent, Error, "{0} {1} : 이미 사용중이라고 등록된 어빌리티 태그: {2}", __FUNCTION__, __LINE__,
		          ActivateAbilityTag.ToString());
		return;
	}
	/*UE_LOGFMT(LogAbilityComponent, Log, "============등록전=============");
	for(auto iter : ActivatedAbilityTags)
	{
		UE_LOGFMT(LogAbilityComponent, Log, "{0}",iter.ToString());
	}
	UE_LOGFMT(LogAbilityComponent, Log, "=========================");*/
	ActivatedAbilityTags.AddTag(ActivateAbilityTag);
	/*UE_LOGFMT(LogAbilityComponent, Warning, "=============등록후============");
	for(auto iter : ActivatedAbilityTags)
	{
		UE_LOGFMT(LogAbilityComponent, Warning, "{0}",iter.ToString());
	}
	UE_LOGFMT(LogAbilityComponent, Warning, "=========================");*/
}


void UAbilityComponent::UnRegisterAbilityTag(FGameplayTag ActivateAbilityTag)
{
	ActivatedAbilityTags.RemoveTag(ActivateAbilityTag);
	//UKismetSystemLibrary::PrintString(this,TEXT("제거하려는 테그 : ")+ActivateAbilityTag.ToString() + TEXT("결과 : ") + ActivatedAbilityTags.ToString());
}

bool UAbilityComponent::HasEffectTag(FGameplayTag GameplayTag)
{
	return AppliedEffectTags.HasTagExact(GameplayTag);
}

bool UAbilityComponent::HasAllEffectTag(const FGameplayTagContainer& Tags)
{
	return AppliedEffectTags.HasAll(Tags);
}

bool UAbilityComponent::HasAnyEffectTag(const FGameplayTagContainer& Tags)
{
	/*UE_LOGFMT(LogAbilityComponent,Log,"---------------전달받은 이팩트 태그가 하나라도 있는지 확인합니다.---------------");
	for(const auto& t : Tags)
	{
		UE_LOGFMT(LogAbilityComponent,Log,"{0}",t.ToString());
	}
	UE_LOGFMT(LogAbilityComponent,Log,"------------소유중인 태그------------------");
	for(const auto& t : AppliedEffectTags)
	{
		UE_LOGFMT(LogAbilityComponent,Log,"{0}",t.ToString());
	}
	UE_LOGFMT(LogAbilityComponent,Log,"------------실제 결과------------------");
	UE_LOGFMT(LogAbilityComponent,Log,"{0}",AppliedEffectTags.HasAny(Tags));
	UE_LOGFMT(LogAbilityComponent,Log,"--------------------------------------");*/
	return AppliedEffectTags.HasAny(Tags);
}

void UAbilityComponent::RegisterEffectTag(const FGameplayTag& EffectTag)
{
	AppliedEffectTags.AddTag(EffectTag);
}

void UAbilityComponent::RegisterEffectTags(const FGameplayTagContainer& EffectTags)
{
	AppliedEffectTags.AppendTags(EffectTags);
}

void UAbilityComponent::UnRegisterEffectTags(const FGameplayTagContainer& EffectTags)
{
	AppliedEffectTags.RemoveTags(EffectTags);
}

void UAbilityComponent::K2_ApplyEffect(TSubclassOf<UAbilityEffect> Effect, AActor* EffectBy,
                                       FOnEffectExpired OnEffectExpired)
{
	if(auto instance = DuplicateObject(Effect.GetDefaultObject(),this))
	{
		ApplyEffect(instance, EffectBy, OnEffectExpired, nullptr);
	}
}

void UAbilityComponent::UnRegisterEffectTag(const FGameplayTag& EffectTag)
{
	AppliedEffectTags.RemoveTag(EffectTag);
}

void UAbilityComponent::ApplyEffect(UAbilityEffect* Effect, AActor* EffectBy, FOnEffectExpired OnEffectExpired,
                                    UAbilityBase* From)
{
	ensure(Effect);
	ensure(EffectBy);

	if (Effect->EffectApplyType == EEffectApplyType::Stack)
	{
		if (From != nullptr)
		{
			UE_LOGFMT(LogTemp, Log, "{0}으로부터 스택형 이팩트 적용 호출", From->GetAbilityTag().ToString());
		}
		auto existEffect = GetAppliedEffectByTag(Effect->UniqueEffectTag);

		if (existEffect == nullptr)
		{
			UE_LOGFMT(LogTemp, Log, "스택형 이팩트 새로 추가함 : {0}", Effect->GetName());
			AppliedEffects.Add(Effect);
			OnApplyEffect.Broadcast(AppliedEffectTags);
			Effect->OnEffectExpired = OnEffectExpired;
			UE_LOGFMT(LogTemp, Log, "스택형 이팩트 적용 {0}", Effect->GetName());
			Effect->ProcessEffect(GetOwner<ABaseCharacter>(), EffectBy, From);
			return;
		}
		UE_LOGFMT(LogTemp, Log, "이미 존재하는 이팩트 오브젝트로부터 스택형 이팩트 적용 {0}", existEffect->GetName());
		existEffect->ProcessEffect(GetOwner<ABaseCharacter>(), EffectBy, From);
	}
	else
	{
		//이미 적용된 이팩트가 있는 경우,
		if (HasEffectTag(Effect->UniqueEffectTag))
		{
			switch (Effect->EffectApplicationDecision)
			{
			case EEffectApplicationDecision::Skip:
				UE_LOGFMT(LogTemp, Warning, "이미 같은 태그의 이팩트가 있기 때문에 적용하지 않고 넘어갑니다.");
				return;
			case EEffectApplicationDecision::SkipButApply:
				if (HasEffectTag(Effect->UniqueEffectTag))
				{
					auto existEffect = GetAppliedEffectByTag(Effect->UniqueEffectTag);
					UE_LOGFMT(LogTemp, Warning, "이미 같은 태그의 이팩트가 있지만, 기존에 적용되던 이팩트 오브젝트로부터 다시 적용합니다.");
					OnApplyEffect.Broadcast(AppliedEffectTags);
					existEffect->OnEffectExpired = OnEffectExpired;
					existEffect->ProcessEffect(GetOwner<ABaseCharacter>(), EffectBy, From);
				}
				break;
			case EEffectApplicationDecision::AlwaysApply:
				AppliedEffects.Add(Effect);
				OnApplyEffect.Broadcast(AppliedEffectTags);
				Effect->OnEffectExpired = OnEffectExpired;
				Effect->ProcessEffect(GetOwner<ABaseCharacter>(), EffectBy, From);
				break;
			case EEffectApplicationDecision::MAX:
				break;
			default: ;
			}
		}
		else
		{
			AppliedEffects.Add(Effect);
			OnApplyEffect.Broadcast(AppliedEffectTags);
			Effect->OnEffectExpired = OnEffectExpired;
			Effect->ProcessEffect(GetOwner<ABaseCharacter>(), EffectBy, From);
		}
		
	}
}

void UAbilityComponent::ApplyEffects(const TArray<UAbilityEffect*>& Effects, AActor* EffectBy,
                                     FOnEffectExpired OnEffectExpired, UAbilityBase* From)
{
	ensure(EffectBy);
	for (auto e : Effects)
	{
		ApplyEffect(e, EffectBy, OnEffectExpired, From);
	}
}

TArray<UAbilityEffect*> UAbilityComponent::K2_ApplyEffectsWithReturn(const TArray<TSubclassOf<UAbilityEffect>>& Effects,
                                                                  AActor* EffectBy, UObject* AdditionalData)
{
	TArray<UAbilityEffect*> effectToApply;

	UAbilityCueAdditionalInformation* cueAddInfo = nullptr;
	if (AdditionalData != nullptr && AdditionalData->IsA<UAbilityCueAdditionalInformation>())
	{
		cueAddInfo = Cast<UAbilityCueAdditionalInformation>(AdditionalData);
	}
	
		for (auto e : Effects)
		{
			if (auto effect = DuplicateObject<UAbilityEffect>(e.GetDefaultObject(), GetOwner()))
			{
				effectToApply.Add(effect);
				if (cueAddInfo != nullptr)
				{
					cueAddInfo->UpdateCueLocation(effect->InstanceAbilityCues);
					cueAddInfo->UpdateCueLocation(effect->IntervalAbilityCues);
				}
				ApplyEffect(effect, EffectBy, FOnEffectExpired(), nullptr);
			}
		}
	

	return effectToApply;
}

TArray<UAbilityEffect*> UAbilityComponent::ApplyEffectsWithReturn(const TArray<UAbilityEffect*>& Effects,
	AActor* EffectBy, UObject* AdditionalData)
{
	TArray<UAbilityEffect*> effectToApply;

	UAbilityCueAdditionalInformation* cueAddInfo = nullptr;
	if (AdditionalData != nullptr && AdditionalData->IsA<UAbilityCueAdditionalInformation>())
	{
		cueAddInfo = Cast<UAbilityCueAdditionalInformation>(AdditionalData);
	}
	
	for (auto e : Effects)
	{
		effectToApply.Add(e);
		if (cueAddInfo != nullptr)
		{
			cueAddInfo->UpdateCueLocation(e->InstanceAbilityCues);
			cueAddInfo->UpdateCueLocation(e->IntervalAbilityCues);
		}
		ApplyEffect(e, EffectBy, FOnEffectExpired(), nullptr);
	}
	

	return effectToApply;
}

void UAbilityComponent::EndEffect(UAbilityEffect* Effect)
{
	if (Effect->IsValidLowLevel())
	{
		if (Effect->EffectApplyType == EEffectApplyType::Stack)
		{
			if (auto existEffect = GetAppliedEffectByTag(Effect->UniqueEffectTag))
			{
				existEffect->EndEffect(GetOwner<ABaseCharacter>());
			}
		}
		else
		{
			if (AppliedEffects.Contains(Effect))
			{
				/*AppliedEffects.Remove(Effect);*/
				Effect->EndEffect(GetOwner<ABaseCharacter>());
			}
		}
	}
}

void UAbilityComponent::EndEffects(const TArray<UAbilityEffect*>& Effects)
{
	for (auto e : Effects)
	{
		if (e->IsValidLowLevel())
		{
			EndEffect(e);
		}
		else
		{
			UE_LOGFMT(LogTemp, Error, "{0}의 이팩트 엔드 요청 그러나 유효하지 않습니다!!", GetOwner()->GetActorNameOrLabel());
		}
	}
}

void UAbilityComponent::EndEffectByTag(FGameplayTag Tag)
{
	if (HasEffectTag(Tag))
	{
		if (auto e = GetAppliedEffectByTag(Tag))
		{
			e->EndEffect(Cast<ABaseCharacter>(GetOwner()));
		}
	}
}

void UAbilityComponent::RemoveSavedCues(const TArray<FAbilityCueInformation>& Cues)
{
	for (auto c : Cues)
	{
		if (ReusableCues.Contains(c.CueTag))
		{
			if (auto cue = ReusableCues[c.CueTag])
			{
				cue->Destroy();
			}
			ReusableCues.Remove(c.CueTag);
		}

		if (DurableCues.Contains(c.CueTag))
		{
			if (auto cue = DurableCues[c.CueTag])
			{
				cue->Destroy();
			}
			DurableCues.Remove(c.CueTag);
		}
	}
}

void UAbilityComponent::RemoveSavedCueByTag(const FGameplayTagContainer& CueTags)
{
	for (auto tag : CueTags)
	{
		if (ReusableCues.Contains(tag))
		{
			if (auto cue = ReusableCues[tag])
			{
				cue->Destroy();
			}
			ReusableCues.Remove(tag);
		}

		if (DurableCues.Contains(tag))
		{
			if (auto cue = DurableCues[tag])
			{
				cue->Destroy();
			}
			DurableCues.Remove(tag);
		}
	}
}

bool UAbilityComponent::IsInvincible()
{
	return AppliedEffectTags.HasTag(FGameplayTag::RequestGameplayTag("Common.Passive.Invincibility"));
}

/*
void UAbilityComponent::ForceEndAllAbility()
{
	for(auto iter : AvailableAbilities)
	{
		iter->EndAbility();
	}
}
*/


AAbilityCue* UAbilityComponent::ApplyCue(FAbilityCueInformation CueInformation)
{
	if (CueInformation.AbilityCueObject != nullptr/* && GetOwner<ABaseCharacter>()->IsDead() == false*/)
	{
		if (CueInformation.AbilityCueType == EAbilityCueType::AttachToTarget || CueInformation.AbilityCueType ==
			EAbilityCueType::AttachToSocket)
		{
			CueInformation.AttachTarget = GetOwner();
		}

		class AAbilityCue* cueActor = nullptr;
		//큐 스폰 람다함수화
		auto spawnCueFunction = [this, cueActor](const FAbilityCueInformation& Info)
		{
			if (auto cueClass = Info.AbilityCueObject)
			{
				FActorSpawnParameters param;
				param.Owner = GetOwner<ABaseCharacter>();
				param.Instigator = GetOwner<ABaseCharacter>();
				param.SpawnCollisionHandlingOverride =
					ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				auto spawnCue = GetWorld()->SpawnActor<AAbilityCue>(cueClass, param);
				/*UE_LOGFMT(LogAbilityComponent,Log,"큐 스폰 : {0}, {1}",spawnCue->GetActorNameOrLabel(),Info.CueTag.ToString());*/
				return spawnCue;
			}
			UE_LOGFMT(LogAbilityComponent, Error, "큐 로드 실패 : {0} {1}", __FUNCTION__, __LINE__);

			return cueActor;
		};
		
		//재 사용 가능한 큐인 경우.
		if (CueInformation.bReusable == true)
		{
			if (ReusableCues.Contains(CueInformation.CueTag) == false)
			{
				cueActor = spawnCueFunction(CueInformation);
				UE_LOGFMT(LogAbilityComponent, Log, "재사용 가능한 큐 스폰 : {0}, {1}", cueActor->GetActorNameOrLabel(),
				          CueInformation.CueTag.ToString());
				ReusableCues.Add(CueInformation.CueTag, cueActor);
				OnAddCue.Broadcast(CueInformation.CueTag);
			}
			else
			{
				cueActor = ReusableCues[CueInformation.CueTag];
				UE_LOGFMT(LogAbilityComponent, Log, "재사용 가능한 큐 꺼내옴 : {0}, {1}", cueActor->GetActorNameOrLabel(),
				          CueInformation.CueTag.ToString());
			}
		}
		//재 사용 가능한 큐가 아닌 경우.
		else
		{
			//기간동안 적용되는 큐인지 확인합니다.
			if (CueInformation.bIsNotInstanceCue)
			{
				//일정 기간 후 지워져야 하는 큐라면, 이팩트 종료시 같이 제거하기 위해서 저장합니다.
				if (DurableCues.Contains(CueInformation.CueTag) == false)
				{
					cueActor = spawnCueFunction(CueInformation);
					DurableCues.Add(CueInformation.CueTag, cueActor);
					OnAddCue.Broadcast(CueInformation.CueTag);
				}
				else
				{
					cueActor = DurableCues[CueInformation.CueTag];
				}
			}
			else
			{
				cueActor = spawnCueFunction(CueInformation);
				OnAddCue.Broadcast(CueInformation.CueTag);
			}
		}


		if (cueActor != nullptr)
		{
			cueActor->PlayCue(CueInformation);
		}

		return cueActor;
	}
	UE_LOGFMT(LogAbilityComponent, Error, "큐 클래스 정보가 유효하지 않습니다 :  {0} {1}", __FUNCTION__, __LINE__);

	return nullptr;
}


const TArray<AAbilityCue*> UAbilityComponent::ApplyCues(const TArray<FAbilityCueInformation>& CueInformations)
{
	TArray<class AAbilityCue*> cues;
	for (auto c : CueInformations)
	{
		if (auto cue = ApplyCue(c))
		{
			cues.Emplace(cue);
		}
	}

	return cues;
}

void UAbilityComponent::ForceEndAbility(const FGameplayTag& AbilityTag)
{
	UE_LOGFMT(LogAbilityComponent, Warning, "어빌리티 강제 종료 : {0}", AbilityTag.ToString());
	if (ActivatedAbilityTags.HasTagExact(AbilityTag))
	{
		auto find = AvailableAbilities.FindByPredicate([AbilityTag](class UAbilityBase* Ability)
		{
			return Ability->GetAbilityTag().MatchesTagExact(AbilityTag);
		});

		if (find != nullptr)
		{
			(*find)->EndAbility();
		}
	}
	else
	{
		UE_LOGFMT(LogAbilityComponent, Error, "강제 종료할 어빌리티가 없습니다 : {0}", AbilityTag.ToString());
	}
}

void UAbilityComponent::OnEndAbilityEvent()
{
	OnEndAbility.Broadcast();
}

void UAbilityComponent::ClearReusableCue()
{
	if (GetWorld()->GetTimerManager().TimerExists(CueClearTimerHandle) == false)
	{
		/*FTimerDelegate destroyTimerDel = FTimerDelegate::CreateLambda([this]()
		{
			for(auto iter : ReusableCues)
			{
				if(iter.Value->IsValidLowLevel())
				{
					iter.Value->Destroy();
				}
			}

			ReusableCues.Empty();
		});
		
		GetWorld()->GetTimerManager().SetTimer(CueClearTimerHandle,destroyTimerDel,5.f,false);*/
		GetWorld()->GetTimerManager().SetTimer(CueClearTimerHandle, this, &UAbilityComponent::DestroyReusableCues, 5.f,
		                                       false);
	}
}

void UAbilityComponent::DestroyReusableCues()
{
	for (auto iter : ReusableCues)
	{
		if (iter.Value->IsValidLowLevel())
		{
			iter.Value->Destroy();
		}
	}

	ReusableCues.Empty();
}

void UAbilityComponent::HandleAbilityAdditionalData(UAbilityBase* Ability, UObject* Object)
{
	if (Ability)
	{
		if (Object != nullptr && Object->IsA<UAbilityAdditionalInformation>())
		{
			auto data = Cast<UAbilityAdditionalInformation>(Object);
			Ability->OnEndAbility.AddUnique(data->OnEndAbility);
		}
	}
}

void UAbilityComponent::RemoveFromAppliedEffectsArray(UAbilityEffect* EffectToRemove)
{
	if (AppliedEffects.Contains(EffectToRemove))
	{
		AppliedEffects.Remove(EffectToRemove);
	}
}


UAbilityEffect* UAbilityComponent::GetAppliedEffectByTag(FGameplayTag TagToFind)
{
	auto find = AppliedEffects.FindByPredicate([TagToFind](const UAbilityEffect* e)
	{
		return e->UniqueEffectTag.MatchesTagExact(TagToFind);
	});

	if (find != nullptr)
	{
		return *find;
	}

	return nullptr;
}


void UAbilityComponent::UnRootAppliedTalents()
{
	for(auto iter : AvailableAbilities)
	{
		if(iter)
		{
			iter->UnBindTalent();
		}
	}
}


void UAbilityComponent::DestroyComponent(bool bPromoteChildren)
{
	UnRootAppliedEffects();
	UnRootAppliedTalents();
	Super::DestroyComponent(bPromoteChildren);
}

void UAbilityComponent::UnRootAppliedEffects()
{
	for(auto iter : AppliedEffects)
	{
		if(iter)
		{
			if(iter->IsRooted())
			{
				iter->RemoveFromRoot();
			}
		}
	}
}

void UAbilityComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnRootAppliedEffects();
	UnRootAppliedTalents();
	Super::EndPlay(EndPlayReason);
}
