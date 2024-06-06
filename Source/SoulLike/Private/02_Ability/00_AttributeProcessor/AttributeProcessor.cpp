// Fill out your copyright notice in the Description page of Project Settings.


#include "02_Ability/00_AttributeProcessor/AttributeProcessor.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Logging/StructuredLog.h"




void UAttributeProcessor::UpdateCharacterInfoToWidget(const FAttributeEffect& Effect,
                                                      UAttributeComponent* AttributeComponent)
{
	if (Effect.Attribute == EAttributeType::EXP)
	{
		AttributeComponent->OnUpdateExp.Broadcast(Effect.ApplyValue);
		return;
	}

	if (Effect.Attribute != EAttributeType::HP && Effect.Attribute != EAttributeType::SP && Effect.Attribute !=
		EAttributeType::MP && Effect.Attribute != EAttributeType::EXP)
	{
		AttributeComponent->OnCharacterInformationUpdate.Broadcast();
	}
}

float UAttributeProcessor_Add::ProcessAttributeEffect(class UAttributeComponent* AttributeComponent,
                                                      const FAttributeEffect& Effect, float DeltaTime,
                                                      UObject* AdditionalInformation)
{
	Super::ProcessAttributeEffect(AttributeComponent, Effect, DeltaTime, AdditionalInformation);

	float setValue = 0;
	if (AttributeComponent)
	{
		switch (Effect.Attribute)
		{
		case EAttributeType::NONE:
			break;
		case EAttributeType::HP:
			setValue = FMath::Clamp(AttributeComponent->GetHP() + Effect.ApplyValue * DeltaTime, 0,
			                        AttributeComponent->GetMaxHP());
			AttributeComponent->SetHP(setValue);
			AttributeComponent->BroadcastHPEvent();
			break;
		case EAttributeType::SP:
			//UE_LOGFMT(LogTemp,Log,"초당 회복량 : {0}",Effect.ApplyValue);
			setValue = FMath::Clamp(AttributeComponent->GetSP() + Effect.ApplyValue * DeltaTime, 0,
			                        AttributeComponent->GetMaxSP());
			AttributeComponent->SetSP(setValue);
			AttributeComponent->BroadcastSPEvent();
			break;
		case EAttributeType::MP:
			setValue = FMath::Clamp(AttributeComponent->GetMP() + Effect.ApplyValue * DeltaTime, 0,
			                        AttributeComponent->GetMaxMP());
			AttributeComponent->SetMP(setValue);
			AttributeComponent->BroadcastMPEvent();
			break;
		case EAttributeType::MaxHP:
			setValue = AttributeComponent->GetMaxHP() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetMaxHP(setValue);
			AttributeComponent->SetHP(AttributeComponent->GetHP() + Effect.ApplyValue * DeltaTime);
			AttributeComponent->BroadcastHPEvent();
			AttributeComponent->BroadcastMaxHPEvent();
			break;
		case EAttributeType::MaxSP:
			setValue = AttributeComponent->GetMaxSP() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetMaxSP(setValue);
			AttributeComponent->SetSP(AttributeComponent->GetSP() + Effect.ApplyValue * DeltaTime);
			AttributeComponent->BroadcastSPEvent();
			AttributeComponent->BroadcastMaxSPEvent();
			break;
		case EAttributeType::MaxMP:
			setValue = AttributeComponent->GetMaxMP() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetMaxMP(setValue);
			AttributeComponent->SetMP(AttributeComponent->GetMP() + Effect.ApplyValue * DeltaTime);
			AttributeComponent->BroadcastMPEvent();
			AttributeComponent->BroadcastMaxMPEvent();
			break;
		case EAttributeType::RecoverHP:
			setValue = AttributeComponent->GetRecoverHP() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetRecoverHP(setValue);
			break;
		case EAttributeType::RecoverSP:
			setValue = AttributeComponent->GetRecoverSP() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetRecoverSP(setValue);
			break;
		case EAttributeType::RecoverMP:
			setValue = AttributeComponent->GetRecoverHitMP() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetRecoverHitMP(setValue);
			break;
		case EAttributeType::PhysicalAttack:
			setValue = AttributeComponent->GetPhysicalAttack() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPhysicalAttack(
				setValue);
			break;
		case EAttributeType::MagicalAttack:
			setValue = AttributeComponent->GetMagicalAttack() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->
				SetMagicalAttack(setValue);
			break;
		case EAttributeType::PhysicalDefense:
			setValue = AttributeComponent->GetPhysicalDefense() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPhysicalDefense(
				setValue);
			break;
		case EAttributeType::MagicalDefense:
			setValue = AttributeComponent->GetMagicalDefense() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetMagicalDefense(
				setValue);
			break;
		case EAttributeType::Endurance:
			setValue = AttributeComponent->GetEndurance() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetEndurance(setValue);
			break;
		case EAttributeType::Strength:
			setValue = AttributeComponent->GetStrength() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetStrength(setValue);
			break;
		case EAttributeType::Dexterity:
			setValue = AttributeComponent->GetDexterity() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetDexterity(setValue);
			break;
		case EAttributeType::Intelligence:
			setValue = AttributeComponent->GetIntelligence() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetIntelligence(setValue);
			break;
		case EAttributeType::Willpower:
			setValue = AttributeComponent->GetWillpower() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetWillpower(setValue);
			break;
		case EAttributeType::ActionSpeed:
			setValue = AttributeComponent->GetActionSpeed() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetActionSpeed(setValue);
			break;
		case EAttributeType::Level:
			setValue = AttributeComponent->GetLevel() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetLevel(setValue);
			break;
		case EAttributeType::EXP:
			//UE_LOGFMT(LogTemp,Warning,"경험치 추가 : {0}",Effect.ApplyValue);
			setValue = AttributeComponent->GetEXP() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetEXP(setValue);
			break;
		case EAttributeType::PoisonResist:
			setValue = AttributeComponent->GetPoisonResist() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPoisonResist(FMath::Clamp(setValue, 0,INT_MAX));
			break;
		case EAttributeType::DeadlyPoisonResist:
			setValue = AttributeComponent->GetDeadlyPoisonResist() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetDeadlyPoisonResist(FMath::Clamp(setValue, 0,INT_MAX));
			break;
		case EAttributeType::BurnResist:
			setValue = AttributeComponent->GetBurnResist() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetBurnResist(FMath::Clamp(setValue, 0,INT_MAX));
			break;
		case EAttributeType::ChillResist:
			setValue = AttributeComponent->GetChillResist() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetChillResist(FMath::Clamp(setValue, 0,INT_MAX));
			break;
		case EAttributeType::BleedResist:
			setValue = AttributeComponent->GetBleedResist() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetBleedResist(FMath::Clamp(setValue, 0,INT_MAX));
			break;
		case EAttributeType::PetrifactionResist:
			setValue = AttributeComponent->GetPetrifactionResist() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPetrifactionResist(FMath::Clamp(setValue, 0,INT_MAX));
			break;
		case EAttributeType::PoisonAcc:
			setValue = AttributeComponent->GetPoisonAcc() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPoisonAcc(FMath::Clamp(setValue, 0, AttributeComponent->GetPoisonResist()));
			break;
		case EAttributeType::DeadlyPoisonAcc:
			setValue = AttributeComponent->GetDeadlyPoisonAcc() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetDeadlyPoisonAcc(
				FMath::Clamp(setValue, 0, AttributeComponent->GetDeadlyPoisonResist()));
			break;
		case EAttributeType::BurnAcc:
			setValue = AttributeComponent->GetBurnAcc() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetBurnAcc(FMath::Clamp(setValue, 0, AttributeComponent->GetBurnResist()));
			break;
		case EAttributeType::ChillAcc:
			setValue = AttributeComponent->GetChillAcc() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetChillAcc(FMath::Clamp(setValue, 0, AttributeComponent->GetChillResist()));
			break;
		case EAttributeType::BleedAcc:
			setValue = AttributeComponent->GetBleedAcc() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetBleedAcc(FMath::Clamp(setValue, 0, AttributeComponent->GetBleedResist()));
			break;
		case EAttributeType::PetrifactionAcc:
			setValue = AttributeComponent->GetPetrifactionAcc() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPetrifactionAcc(
				FMath::Clamp(setValue, 0, AttributeComponent->GetPetrifactionResist()));
			break;
		case EAttributeType::MoveSpeed:
			setValue = AttributeComponent->GetMoveSpeed() + Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetMoveSpeed(FMath::Clamp(setValue, 0,INT_MAX));
			AttributeComponent->OnChangedMoveSpeedAttribute.Broadcast();
			break;
		case EAttributeType::MAX:
			break;
		default: ;
		}
	}

	UpdateCharacterInfoToWidget(Effect, AttributeComponent);

	if (AdditionalInformation != nullptr && AdditionalInformation->IsA<UAbilityEffectAdditionalInformation>())
	{
		const auto addInfo = Cast<UAbilityEffectAdditionalInformation>(AdditionalInformation);
		AttributeComponent->OnAddAttributeEffectAdditionalInformation.Broadcast(Effect, addInfo, DeltaTime);
	}
	return setValue;
}

float UAttributeProcessor_Remove::ProcessAttributeEffect(UAttributeComponent* AttributeComponent,
                                                         const FAttributeEffect& Effect, float DeltaTime,
                                                         UObject* AdditionalInformation)
{
	Super::ProcessAttributeEffect(AttributeComponent, Effect, DeltaTime, AdditionalInformation);
	
	float setValue = 0;
	if (AttributeComponent)
	{
		switch (Effect.Attribute)
		{
		case EAttributeType::NONE:
			break;
		case EAttributeType::HP:
			UE_LOGFMT(LogTemp, Warning, "{0} 에게 {1}만큼의 피해를 주려 합니다.",
			          AttributeComponent->GetOwner()->GetActorNameOrLabel(), Effect.ApplyValue * DeltaTime);
			setValue = FMath::Clamp(AttributeComponent->GetHP() - Effect.ApplyValue * DeltaTime, 0,
			                        AttributeComponent->GetMaxHP());
			AttributeComponent->SetHP(setValue);
			AttributeComponent->BroadcastHPEvent();
			break;
		case EAttributeType::SP:
#if WITH_EDITOR

			if (DeltaTime == 1)
			{
				UE_LOGFMT(LogTemp, Log, "SP 감소량 : {0}", Effect.ApplyValue);
			}
#endif
		//UE_LOGFMT(LogTemp,Log,"초당 감소량 : {0}, 델타타임 계산값 : {1}",Effect.ApplyValue,Effect.ApplyValue * DeltaTime);
			setValue = FMath::Clamp(AttributeComponent->GetSP() - Effect.ApplyValue * DeltaTime, 0,
			                        AttributeComponent->GetMaxSP());
			AttributeComponent->SetSP(setValue);
			AttributeComponent->BroadcastSPEvent();
			break;
		case EAttributeType::MP:
			setValue = FMath::Clamp(AttributeComponent->GetMP() - Effect.ApplyValue * DeltaTime, 0,
			                        AttributeComponent->GetMaxMP());
			AttributeComponent->SetMP(setValue);
			AttributeComponent->BroadcastMPEvent();
			break;
		case EAttributeType::MaxHP:
			setValue = AttributeComponent->GetMaxHP() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetMaxHP(setValue);
			AttributeComponent->SetHP(AttributeComponent->GetHP() - Effect.ApplyValue * DeltaTime);
			AttributeComponent->BroadcastMaxHPEvent();
			AttributeComponent->BroadcastHPEvent();
			break;
		case EAttributeType::MaxSP:
			setValue = AttributeComponent->GetMaxSP() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetMaxSP(setValue);
			AttributeComponent->SetSP(AttributeComponent->GetSP() - Effect.ApplyValue * DeltaTime);
			AttributeComponent->BroadcastMaxSPEvent();
			AttributeComponent->BroadcastSPEvent();
			break;
		case EAttributeType::MaxMP:
			setValue = AttributeComponent->GetMaxMP() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetMaxMP(setValue);
			AttributeComponent->SetMP(AttributeComponent->GetMP() - Effect.ApplyValue * DeltaTime);
			AttributeComponent->BroadcastMaxMPEvent();
			AttributeComponent->BroadcastMPEvent();
			break;
		case EAttributeType::RecoverHP:
			setValue = AttributeComponent->GetRecoverHP() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetRecoverHP(setValue);
			break;
		case EAttributeType::RecoverSP:
			setValue = AttributeComponent->GetRecoverSP() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetRecoverSP(setValue);
			break;
		case EAttributeType::RecoverMP:
			setValue = AttributeComponent->GetRecoverHitMP() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetRecoverHitMP(setValue);
			break;
		case EAttributeType::PhysicalAttack:
			setValue = AttributeComponent->GetPhysicalAttack() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPhysicalAttack(
				setValue);
			break;
		case EAttributeType::MagicalAttack:
			setValue = AttributeComponent->GetMagicalAttack() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->
				SetMagicalAttack(setValue);
			break;
		case EAttributeType::PhysicalDefense:
			setValue = AttributeComponent->GetPhysicalDefense() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPhysicalDefense(
				setValue);
			break;
		case EAttributeType::MagicalDefense:
			setValue = AttributeComponent->GetMagicalDefense() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetMagicalDefense(
				setValue);
			break;
		case EAttributeType::Endurance:
			setValue = AttributeComponent->GetEndurance() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetEndurance(setValue);
			break;
		case EAttributeType::Strength:
			setValue = AttributeComponent->GetStrength() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetStrength(setValue);
			break;
		case EAttributeType::Dexterity:
			setValue = AttributeComponent->GetDexterity() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetDexterity(setValue);
			break;
		case EAttributeType::Intelligence:
			setValue = AttributeComponent->GetIntelligence() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetIntelligence(setValue);
			break;
		case EAttributeType::Willpower:
			setValue = AttributeComponent->GetWillpower() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetWillpower(setValue);
			break;
		case EAttributeType::ActionSpeed:
			setValue = AttributeComponent->GetActionSpeed() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetActionSpeed(setValue);
			break;
		case EAttributeType::Level:
			setValue = AttributeComponent->GetLevel() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetLevel(setValue);
			break;
		case EAttributeType::EXP:
			setValue = AttributeComponent->GetEXP() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetEXP(setValue);
			break;
		case EAttributeType::PoisonResist:
			setValue = AttributeComponent->GetPoisonResist() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPoisonResist(setValue);
			break;
		case EAttributeType::DeadlyPoisonResist:
			setValue = AttributeComponent->GetDeadlyPoisonResist() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetDeadlyPoisonResist(setValue);
			break;
		case EAttributeType::BurnResist:
			setValue = AttributeComponent->GetBurnResist() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetBurnResist(setValue);
			break;
		case EAttributeType::ChillResist:
			setValue = AttributeComponent->GetChillResist() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetChillResist(setValue);
			break;
		case EAttributeType::BleedResist:
			setValue = AttributeComponent->GetBleedResist() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetBleedResist(setValue);
			break;
		case EAttributeType::PetrifactionResist:
			setValue = AttributeComponent->GetPetrifactionResist() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPetrifactionResist(setValue);
			break;
		case EAttributeType::PoisonAcc:
			setValue = AttributeComponent->GetPoisonAcc() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPoisonAcc(FMath::Clamp(setValue, 0, AttributeComponent->GetPoisonResist()));
			break;
		case EAttributeType::DeadlyPoisonAcc:
			setValue = AttributeComponent->GetDeadlyPoisonAcc() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetDeadlyPoisonAcc(
				FMath::Clamp(setValue, 0, AttributeComponent->GetDeadlyPoisonResist()));
			break;
		case EAttributeType::BurnAcc:
			setValue = AttributeComponent->GetBurnAcc() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetBurnAcc(FMath::Clamp(setValue, 0, AttributeComponent->GetBurnResist()));
			break;
		case EAttributeType::ChillAcc:
			setValue = AttributeComponent->GetChillAcc() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetChillAcc(FMath::Clamp(setValue, 0, AttributeComponent->GetChillResist()));
			break;
		case EAttributeType::BleedAcc:
			setValue = AttributeComponent->GetBleedAcc() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetBleedAcc(FMath::Clamp(setValue, 0, AttributeComponent->GetBleedResist()));
			break;
		case EAttributeType::PetrifactionAcc:
			setValue = AttributeComponent->GetPetrifactionAcc() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPetrifactionAcc(
				FMath::Clamp(setValue, 0, AttributeComponent->GetPetrifactionResist()));
			break;
		case EAttributeType::MoveSpeed:
			setValue = AttributeComponent->GetMoveSpeed() - Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetMoveSpeed(FMath::Clamp(setValue, 0,INT_MAX));
			AttributeComponent->OnChangedMoveSpeedAttribute.Broadcast();
			break;
		case EAttributeType::MAX:
			break;
		default: ;
		}
	}
	UpdateCharacterInfoToWidget(Effect, AttributeComponent);
	if (AdditionalInformation != nullptr && AdditionalInformation->IsA<UAbilityEffectAdditionalInformation>())
	{
		const auto addInfo = Cast<UAbilityEffectAdditionalInformation>(AdditionalInformation);
		AttributeComponent->OnRemoveAttributeEffectAdditionalInformation.Broadcast(Effect, addInfo, DeltaTime);
	}

	return setValue;
}

float UAttributeProcessor_Multiplication::ProcessAttributeEffect(UAttributeComponent* AttributeComponent,
                                                                 const FAttributeEffect& Effect, float DeltaTime,
                                                                 UObject* AdditionalInformation)
{
	Super::ProcessAttributeEffect(AttributeComponent, Effect, DeltaTime, AdditionalInformation);

	float setValue = 0;
	if (AttributeComponent)
	{
		switch (Effect.Attribute)
		{
		case EAttributeType::NONE:
			break;
		case EAttributeType::HP:
			setValue = FMath::Clamp(AttributeComponent->GetHP() * Effect.ApplyValue * DeltaTime, 0,
			                        AttributeComponent->GetMaxHP());
			AttributeComponent->SetHP(setValue);
			AttributeComponent->BroadcastHPEvent();
			break;
		case EAttributeType::SP:
			//UE_LOGFMT(LogTemp,Log,"초당 회복량 : {0}",Effect.ApplyValue);
			setValue = FMath::Clamp(AttributeComponent->GetSP() * Effect.ApplyValue * DeltaTime, 0,
			                        AttributeComponent->GetMaxSP());
			AttributeComponent->SetSP(setValue);
			AttributeComponent->BroadcastSPEvent();
			break;
		case EAttributeType::MP:
			setValue = FMath::Clamp(AttributeComponent->GetMP() * Effect.ApplyValue * DeltaTime, 0,
			                        AttributeComponent->GetMaxMP());
			AttributeComponent->SetMP(setValue);
			AttributeComponent->BroadcastMPEvent();
			break;
		case EAttributeType::MaxHP:
			setValue = AttributeComponent->GetMaxHP() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetMaxHP(setValue);
			AttributeComponent->BroadcastMaxHPEvent();
			AttributeComponent->BroadcastHPEvent();
			break;
		case EAttributeType::MaxSP:
			setValue = AttributeComponent->GetMaxSP() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetMaxSP(setValue);
			AttributeComponent->BroadcastMaxSPEvent();
			AttributeComponent->BroadcastSPEvent();
			break;
		case EAttributeType::MaxMP:
			setValue = AttributeComponent->GetMaxMP() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetMaxMP(setValue);
			AttributeComponent->BroadcastMaxMPEvent();
			AttributeComponent->BroadcastMPEvent();
			break;
		case EAttributeType::RecoverHP:
			setValue = AttributeComponent->GetRecoverHP() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetRecoverHP(setValue);
			break;
		case EAttributeType::RecoverSP:
			setValue = AttributeComponent->GetRecoverSP() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetRecoverSP(setValue);
			break;
		case EAttributeType::RecoverMP:
			setValue = AttributeComponent->GetRecoverHitMP() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetRecoverHitMP(setValue);
			break;
		case EAttributeType::PhysicalAttack:
			setValue = AttributeComponent->GetPhysicalAttack() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPhysicalAttack(
				setValue);
			break;
		case EAttributeType::MagicalAttack:
			setValue = AttributeComponent->GetMagicalAttack() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->
				SetMagicalAttack(setValue);
			break;
		case EAttributeType::PhysicalDefense:
			setValue = AttributeComponent->GetPhysicalDefense() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPhysicalDefense(
				setValue);
			break;
		case EAttributeType::MagicalDefense:
			setValue = AttributeComponent->GetMagicalDefense() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetMagicalDefense(
				setValue);
			break;
		case EAttributeType::Endurance:
			setValue = AttributeComponent->GetEndurance() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetEndurance(setValue);
			break;
		case EAttributeType::Strength:
			setValue = AttributeComponent->GetStrength() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetStrength(setValue);
			break;
		case EAttributeType::Dexterity:
			setValue = AttributeComponent->GetDexterity() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetDexterity(setValue);
			break;
		case EAttributeType::Intelligence:
			setValue = AttributeComponent->GetIntelligence() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetIntelligence(setValue);
			break;
		case EAttributeType::Willpower:
			setValue = AttributeComponent->GetWillpower() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetWillpower(setValue);
			break;
		case EAttributeType::ActionSpeed:
			setValue = AttributeComponent->GetActionSpeed() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetActionSpeed(setValue);
			break;
		case EAttributeType::Level:
			setValue = AttributeComponent->GetLevel() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetLevel(setValue);
			break;
		case EAttributeType::EXP:
			setValue = AttributeComponent->GetEXP() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetEXP(setValue);
			break;
		case EAttributeType::PoisonResist:
			setValue = AttributeComponent->GetPoisonResist() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPoisonResist(setValue);
			break;
		case EAttributeType::DeadlyPoisonResist:
			setValue = AttributeComponent->GetDeadlyPoisonResist() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetDeadlyPoisonResist(setValue);
			break;
		case EAttributeType::BurnResist:
			setValue = AttributeComponent->GetBurnResist() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetBurnResist(setValue);
			break;
		case EAttributeType::ChillResist:
			setValue = AttributeComponent->GetChillResist() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetChillResist(setValue);
			break;
		case EAttributeType::BleedResist:
			setValue = AttributeComponent->GetBleedResist() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetBleedResist(setValue);
			break;
		case EAttributeType::PetrifactionResist:
			setValue = AttributeComponent->GetPetrifactionResist() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPetrifactionResist(setValue);
			break;
		case EAttributeType::PoisonAcc:
			setValue = AttributeComponent->GetPoisonAcc() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPoisonAcc(FMath::Clamp(setValue, 0, AttributeComponent->GetPoisonResist()));
			break;
		case EAttributeType::DeadlyPoisonAcc:
			setValue = AttributeComponent->GetDeadlyPoisonAcc() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetDeadlyPoisonAcc(
				FMath::Clamp(setValue, 0, AttributeComponent->GetDeadlyPoisonResist()));
			break;
		case EAttributeType::BurnAcc:
			setValue = AttributeComponent->GetBurnAcc() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetBurnAcc(FMath::Clamp(setValue, 0, AttributeComponent->GetBurnResist()));
			break;
		case EAttributeType::ChillAcc:
			setValue = AttributeComponent->GetChillAcc() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetChillAcc(FMath::Clamp(setValue, 0, AttributeComponent->GetChillResist()));
			break;
		case EAttributeType::BleedAcc:
			setValue = AttributeComponent->GetBleedAcc() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetBleedAcc(FMath::Clamp(setValue, 0, AttributeComponent->GetBleedResist()));
			break;
		case EAttributeType::PetrifactionAcc:
			setValue = AttributeComponent->GetPetrifactionAcc() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPetrifactionAcc(
				FMath::Clamp(setValue, 0, AttributeComponent->GetPetrifactionResist()));
			break;
		case EAttributeType::MoveSpeed:
			setValue = AttributeComponent->GetMoveSpeed() * Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetMoveSpeed(FMath::Clamp(setValue, 0,INT_MAX));
			AttributeComponent->OnChangedMoveSpeedAttribute.Broadcast();
			break;
		case EAttributeType::MAX:
			break;
		default: ;
		}
	}
	UpdateCharacterInfoToWidget(Effect, AttributeComponent);
	if (AdditionalInformation != nullptr && AdditionalInformation->IsA<UAbilityEffectAdditionalInformation>())
	{
		const auto addInfo = Cast<UAbilityEffectAdditionalInformation>(AdditionalInformation);
		AttributeComponent->OnMultiplicationAttributeEffectAdditionalInformation.Broadcast(Effect, addInfo, DeltaTime);
	}

	return setValue;
}

float UAttributeProcessor_Division::ProcessAttributeEffect(UAttributeComponent* AttributeComponent,
                                                           const FAttributeEffect& Effect, float DeltaTime,
                                                           UObject* AdditionalInformation)
{
	Super::ProcessAttributeEffect(AttributeComponent, Effect, DeltaTime, AdditionalInformation);

	float setValue = 0;
	if (AttributeComponent)
	{
		switch (Effect.Attribute)
		{
		case EAttributeType::NONE:
			break;
		case EAttributeType::HP:
			setValue = FMath::Clamp(AttributeComponent->GetHP() / Effect.ApplyValue * DeltaTime, 0,
			                        AttributeComponent->GetMaxHP());
			AttributeComponent->SetHP(setValue);
			AttributeComponent->BroadcastHPEvent();
			break;
		case EAttributeType::SP:
			//UE_LOGFMT(LogTemp,Log,"초당 회복량 : {0}",Effect.ApplyValue);
			setValue = FMath::Clamp(AttributeComponent->GetSP() / Effect.ApplyValue * DeltaTime, 0,
			                        AttributeComponent->GetMaxSP());
			AttributeComponent->SetSP(setValue);
			AttributeComponent->BroadcastSPEvent();
			break;
		case EAttributeType::MP:
			setValue = FMath::Clamp(AttributeComponent->GetMP() / Effect.ApplyValue * DeltaTime, 0,
			                        AttributeComponent->GetMaxMP());
			AttributeComponent->SetMP(setValue);
			AttributeComponent->BroadcastMPEvent();
			break;
		case EAttributeType::MaxHP:
			setValue = AttributeComponent->GetMaxHP() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetMaxHP(setValue);
			AttributeComponent->BroadcastMaxHPEvent();
			AttributeComponent->BroadcastHPEvent();
			break;
		case EAttributeType::MaxSP:
			setValue = AttributeComponent->GetMaxSP() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetMaxSP(setValue);
			AttributeComponent->BroadcastMaxSPEvent();
			AttributeComponent->BroadcastSPEvent();
			break;
		case EAttributeType::MaxMP:
			setValue = AttributeComponent->GetMaxMP() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetMaxMP(setValue);
			AttributeComponent->BroadcastMaxMPEvent();
			AttributeComponent->BroadcastMPEvent();
			break;
		case EAttributeType::RecoverHP:
			setValue = AttributeComponent->GetRecoverHP() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetRecoverHP(setValue);
			break;
		case EAttributeType::RecoverSP:
			setValue = AttributeComponent->GetRecoverSP() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetRecoverSP(setValue);
			break;
		case EAttributeType::RecoverMP:
			setValue = AttributeComponent->GetRecoverHitMP() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetRecoverHitMP(setValue);
			break;
		case EAttributeType::PhysicalAttack:
			setValue = AttributeComponent->GetPhysicalAttack() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPhysicalAttack(
				setValue);
			break;
		case EAttributeType::MagicalAttack:
			setValue = AttributeComponent->GetMagicalAttack() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->
				SetMagicalAttack(setValue);
			break;
		case EAttributeType::PhysicalDefense:
			setValue = AttributeComponent->GetPhysicalDefense() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPhysicalDefense(
				setValue);
			break;
		case EAttributeType::MagicalDefense:
			setValue = AttributeComponent->GetMagicalDefense() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetMagicalDefense(
				setValue);
			break;
		case EAttributeType::Endurance:
			setValue = AttributeComponent->GetEndurance() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetEndurance(setValue);
			break;
		case EAttributeType::Strength:
			setValue = AttributeComponent->GetStrength() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetStrength(setValue);
			break;
		case EAttributeType::Dexterity:
			setValue = AttributeComponent->GetDexterity() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetDexterity(setValue);
			break;
		case EAttributeType::Intelligence:
			setValue = AttributeComponent->GetIntelligence() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetIntelligence(setValue);
			break;
		case EAttributeType::Willpower:
			setValue = AttributeComponent->GetWillpower() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetWillpower(setValue);
			break;
		case EAttributeType::ActionSpeed:
			setValue = AttributeComponent->GetActionSpeed() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetActionSpeed(setValue);
			break;
		case EAttributeType::Level:
			setValue = AttributeComponent->GetLevel() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetLevel(setValue);
			break;
		case EAttributeType::EXP:
			setValue = AttributeComponent->GetEXP() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetEXP(setValue);
			break;
		case EAttributeType::PoisonResist:
			setValue = AttributeComponent->GetPoisonResist() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPoisonResist(setValue);
			break;
		case EAttributeType::DeadlyPoisonResist:
			setValue = AttributeComponent->GetDeadlyPoisonResist() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetDeadlyPoisonResist(setValue);
			break;
		case EAttributeType::BurnResist:
			setValue = AttributeComponent->GetBurnResist() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetBurnResist(setValue);
			break;
		case EAttributeType::ChillResist:
			setValue = AttributeComponent->GetChillResist() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetChillResist(setValue);
			break;
		case EAttributeType::BleedResist:
			setValue = AttributeComponent->GetBleedResist() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetBleedResist(setValue);
			break;
		case EAttributeType::PetrifactionResist:
			setValue = AttributeComponent->GetPetrifactionResist() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPetrifactionResist(setValue);
			break;
		case EAttributeType::PoisonAcc:
			setValue = AttributeComponent->GetPoisonAcc() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPoisonAcc(setValue);
			break;
		case EAttributeType::DeadlyPoisonAcc:
			setValue = AttributeComponent->GetDeadlyPoisonAcc() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetDeadlyPoisonAcc(setValue);
			break;
		case EAttributeType::BurnAcc:
			setValue = AttributeComponent->GetBurnAcc() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetBurnAcc(setValue);
			break;
		case EAttributeType::ChillAcc:
			setValue = AttributeComponent->GetChillAcc() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetChillAcc(setValue);
			break;
		case EAttributeType::BleedAcc:
			setValue = AttributeComponent->GetBleedAcc() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetBleedAcc(setValue);
			break;
		case EAttributeType::PetrifactionAcc:
			setValue = AttributeComponent->GetPetrifactionAcc() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetPetrifactionAcc(setValue);
			break;
		case EAttributeType::MoveSpeed:
			setValue = AttributeComponent->GetMoveSpeed() / Effect.ApplyValue * DeltaTime;
			AttributeComponent->SetMoveSpeed(FMath::Clamp(setValue, 0,INT_MAX));
			AttributeComponent->OnChangedMoveSpeedAttribute.Broadcast();
			break;
		case EAttributeType::MAX:
			break;
		default: ;
		}
	}
	UpdateCharacterInfoToWidget(Effect, AttributeComponent);
	if (AdditionalInformation != nullptr && AdditionalInformation->IsA<UAbilityEffectAdditionalInformation>())
	{
		const auto addInfo = Cast<UAbilityEffectAdditionalInformation>(AdditionalInformation);
		AttributeComponent->OnDivisionAttributeEffectAdditionalInformation.Broadcast(Effect, addInfo, DeltaTime);
	}

	return setValue;
}
