// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/01_Component/AttributeComponent.h"

#include "ComponentReregisterContext.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "02_Ability/AbilityEffect.h"
#include "02_Ability/01_Task/GameplayTask_LaunchEvent.h"
#include "Logging/StructuredLog.h"
#include "03_Widget/03_LevelUp/AttributePreviewWidget.h"
#include "96_Library/AbilityHelperLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

UStatusEffectValueHandler::UStatusEffectValueHandler()
{
	ReduceValueDefineMap.Add(EStatusEffect::POISON, 1.f);
	ReduceValueDefineMap.Add(EStatusEffect::DEADLY_POISON, 1.f);
	ReduceValueDefineMap.Add(EStatusEffect::BURN, 1.f);
	ReduceValueDefineMap.Add(EStatusEffect::CHILL, 1.f);
	ReduceValueDefineMap.Add(EStatusEffect::BLEED, 1.f);
	ReduceValueDefineMap.Add(EStatusEffect::PETRIFACTION, 0);
}

void UStatusEffectValueHandler::Init(UAttributeComponent* AttComp, EStatusEffect StatusEffect)
{
	TargetStatusEffect = StatusEffect;
	AttributeComponent = AttComp;
}

void UStatusEffectValueHandler::ReduceAccValue(float DeltaTime)
{
	if (AttributeComponent)
	{
		//UE_LOGFMT(LogTemp, Log, "{0}에 대한 감소 효과", StaticEnum<EStatusEffect>()->GetValueAsString(TargetStatusEffect));

		float curValue = 0;
		float resistValue = 0;
		const float reducePerSec = ReduceValueDefineMap[TargetStatusEffect];


		switch (TargetStatusEffect)
		{
		case EStatusEffect::POISON:
			resistValue = AttributeComponent->GetPoisonResist();
			AttributeComponent->SetPoisonAcc(
				FMath::Clamp(AttributeComponent->GetPoisonAcc() - (reducePerSec * DeltaTime), 0,INT_MAX));
			curValue = AttributeComponent->GetPoisonAcc();
			break;
		case EStatusEffect::DEADLY_POISON:
			resistValue = AttributeComponent->GetDeadlyPoisonResist();
			AttributeComponent->SetDeadlyPoisonAcc(
				FMath::Clamp(AttributeComponent->GetDeadlyPoisonAcc() - (reducePerSec * DeltaTime), 0,INT_MAX));
			curValue = AttributeComponent->GetDeadlyPoisonAcc();
			break;
		case EStatusEffect::BURN:
			resistValue = AttributeComponent->GetBurnResist();
			AttributeComponent->SetBurnAcc(FMath::Clamp(AttributeComponent->GetBurnAcc() - (reducePerSec * DeltaTime),
			                                            0,INT_MAX));
			curValue = AttributeComponent->GetBurnAcc();
			break;
		case EStatusEffect::CHILL:
			resistValue = AttributeComponent->GetChillResist();
			AttributeComponent->SetChillAcc(FMath::Clamp(AttributeComponent->GetChillAcc() - (reducePerSec * DeltaTime),
			                                             0,INT_MAX));
			curValue = AttributeComponent->GetChillAcc();
			break;
		case EStatusEffect::FREEZE:
			break;
		case EStatusEffect::BLEED:
			resistValue = AttributeComponent->GetBleedResist();
			AttributeComponent->SetBleedAcc(FMath::Clamp(AttributeComponent->GetBleedAcc() - (reducePerSec * DeltaTime),
			                                             0,INT_MAX));
			curValue = AttributeComponent->GetBleedAcc();
			break;
		case EStatusEffect::PETRIFACTION:
			resistValue = AttributeComponent->GetPetrifactionResist();
			AttributeComponent->SetPetrifactionAcc(
				FMath::Clamp(AttributeComponent->GetPetrifactionAcc() - (reducePerSec * DeltaTime), 0,INT_MAX));
			curValue = AttributeComponent->GetPetrifactionAcc();
			break;
		case EStatusEffect::MAX:
			break;
		}

		AttributeComponent->OnUpdateStatusEffect.Broadcast(TargetStatusEffect, curValue, resistValue);
	}
	else
	{
		UE_LOGFMT(LogTemp, Error, "어트리뷰트 컴포넌트가 더 이상 유효하지 않습니다.");
	}
}



// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...

	{
		static ConstructorHelpers::FObjectFinder<UDataTable> table(TEXT(
			"/Script/Engine.DataTable'/Game/Blueprints/00_Character/03_Attribute/DT_AttributeValuePerPoint.DT_AttributeValuePerPoint'"));
		if (table.Succeeded())
		{
			AttributePerPointDefineTable = table.Object;
		}
	}
	{
		///Script/SoulLike.StatusEffectData'/Game/Blueprints/00_Character/DA_StatusEffect.DA_StatusEffect'
		static ConstructorHelpers::FObjectFinder<UStatusEffectData> asset(TEXT(
			"Script/SoulLike.StatusEffectData'/Game/Blueprints/00_Character/DA_StatusEffect.DA_StatusEffect'"));
		if (asset.Succeeded())
		{
			StatusEffectData = asset.Object;
		}
	}
}

// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	OnUpdateStatusEffect.AddUniqueDynamic(this, &UAttributeComponent::OnUpdateStatusEffectEvent);

	if(GetOwner())
	{
		GetOwner<ACharacter>()->GetCharacterMovement()->MaxWalkSpeed = GetMoveSpeed();
	}
}

void UAttributeComponent::PostInitProperties()
{
	Super::PostInitProperties();

	InitAttributes();

	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::Level, &Level);
	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::EXP, &EXP);

	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::HP, &HP);
	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::SP, &SP);
	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::MP, &MP);

	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::MaxHP, &MaxHP);
	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::MaxSP, &MaxSP);
	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::MaxMP, &MaxMP);

	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::RecoverHP, &RecoverHP);
	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::RecoverSP, &RecoverSP);
	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::RecoverMP, &RecoverHitMP);


	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::PhysicalAttack, &PhysicalAttack);
	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::MagicalAttack, &MagicalAttack);

	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::PhysicalDefense, &PhysicalDefense);
	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::MagicalDefense, &MagicalDefense);

	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::ActionSpeed, &ActionSpeed);

	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::Endurance, &Endurance);

	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::Strength, &Strength);
	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::Dexterity, &Dexterity);
	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::Intelligence, &Intelligence);
	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::Willpower, &Willpower);

	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::PoisonResist, &PoisonResist);
	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::DeadlyPoisonResist, &DeadlyPoisonResist);
	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::BurnResist, &BurnResist);
	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::ChillResist, &ChillResist);
	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::BleedResist, &BleedResist);
	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::PetrifactionResist, &PetrifactionResist);

	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::PoisonAcc, &PoisonAcc);
	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::DeadlyPoisonAcc, &DeadlyPoisonAcc);
	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::BurnAcc, &BurnAcc);
	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::ChillAcc, &ChillAcc);
	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::BleedAcc, &BleedAcc);
	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::PetrifactionAcc, &PetrifactionAcc);

	AttributesNotIncludeLevelUpPoint.Add(EAttributeType::MoveSpeed, &MoveSpeed);

	//레벨업시 투자 가능한 스텟부분 초기화
	LevelUpPointAttributes.Add(EAttributeType::Strength, &StrengthPoint);
	LevelUpPointAttributes.Add(EAttributeType::Dexterity, &DexterityPoint);
	LevelUpPointAttributes.Add(EAttributeType::Intelligence, &IntelligencePoint);
	LevelUpPointAttributes.Add(EAttributeType::Willpower, &WillpowerPoint);
	LevelUpPointAttributes.Add(EAttributeType::MaxHP, &MaxHPPoint);
	LevelUpPointAttributes.Add(EAttributeType::MaxMP, &MaxMPPoint);
	LevelUpPointAttributes.Add(EAttributeType::MaxSP, &MaxSPPoint);
}

#if WITH_EDITOR
void UAttributeComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	InitAttributes();
}
#endif

void UAttributeComponent::InitAttributes()
{
	InitAttributePerPoint();
	InitDefaultAttribute();

	Level.Init(1.f);
	EXP.Init(0);
}

void UAttributeComponent::LoadAttributeNotIncludeLevelUpPoint(const TMap<EAttributeType, FAttribute>& SavedAttribute, bool ShouldUpdateProgressBar, bool bIsRespawn)
{
	for (auto iter : SavedAttribute)
	{
		AttributesNotIncludeLevelUpPoint[iter.Key]->Load(iter.Value);
	}

	if(bIsRespawn)
	{
		SetHP(GetMaxHP());
	}
	
	if(ShouldUpdateProgressBar)
	{
		InitProgressWidget();
	}
	OnCharacterInformationUpdate.Broadcast();
}

void UAttributeComponent::LoadLevelUpPointAttributes(const TMap<EAttributeType, FAttribute>& LevelUpPoint)
{
	for (auto iter : LevelUpPoint)
	{
		LevelUpPointAttributes[iter.Key]->Load(iter.Value);
	}
}

void UAttributeComponent::InitAttributePerPoint()
{
	if (AttributePerPointDefineTable)
	{
		{
			FString contextString;

			const FAttributePerPoint* attPerPoint = nullptr;
			attPerPoint = AttributePerPointDefineTable->FindRow<FAttributePerPoint>("1", contextString);
			/*if (GetOwner()->IsA<APlayerCharacter>())
			{
				attPerPoint = AttributePerPointDefineTable->FindRow<FAttributePerPoint>("1", contextString);
			}
			else if (GetOwner()->IsA<ABaseMonster>())
			{
				attPerPoint = AttributePerPointDefineTable->FindRow<FAttributePerPoint>(
					GetOwner<ABaseMonster>()->GetMonsterTag().GetTagName(), contextString);
			}*/

			if (attPerPoint == nullptr)
			{
				UE_LOGFMT(LogActorComponent, Error, "{0} : 포인트당 증가 스텟을 가져오지 못했습니다.", GetOwner()->GetActorNameOrLabel());
				return;
			}

			AttributePerPoint = *attPerPoint;

			for (auto i = 0; i < static_cast<int32>(EAttributeType::MAX); i++)
			{
				AttributePerPointMap.Add(static_cast<EAttributeType>(i), TArray<TPair<EAttributeType, float>>());
			}

			AttributePerPointMap[EAttributeType::Strength].Add(
				TPair<EAttributeType, float>(EAttributeType::Strength, 1));
			AttributePerPointMap[EAttributeType::Strength].Add(
				TPair<EAttributeType, float>(EAttributeType::MaxHP, AttributePerPoint.MaxHPPerStrength));
			/*AttributePerPointMap[EAttributeType::Strength].Add(
				TPair<EAttributeType, float>(EAttributeType::RecoverHP, AttributePerPoint.RecoverHPPerStrength));*/
			AttributePerPointMap[EAttributeType::Strength].Add(
				TPair<EAttributeType, float>(EAttributeType::PhysicalAttack,
				                             AttributePerPoint.PhysicalAttackPerStrength));

			AttributePerPointMap[EAttributeType::Dexterity].Add(
				TPair<EAttributeType, float>(EAttributeType::Dexterity, 1));
			AttributePerPointMap[EAttributeType::Dexterity].Add(
				TPair<EAttributeType, float>(EAttributeType::MaxHP, AttributePerPoint.MaxHPPerDexterity));
			AttributePerPointMap[EAttributeType::Dexterity].Add(
				TPair<EAttributeType, float>(EAttributeType::MaxSP, AttributePerPoint.MaxSPPerDexterity));
			AttributePerPointMap[EAttributeType::Dexterity].Add(
				TPair<EAttributeType, float>(EAttributeType::PhysicalAttack,
				                             AttributePerPoint.PhysicalAttackPerDexterity));
			AttributePerPointMap[EAttributeType::Dexterity].Add(
				TPair<EAttributeType, float>(EAttributeType::MagicalAttack,
				                             AttributePerPoint.MagicalAttackPerDexterity));
			/*AttributePerPointMap[EAttributeType::Dexterity].Add(
				TPair<EAttributeType, float>(EAttributeType::RecoverSP, AttributePerPoint.RecoverSPPerDexterity));*/
			AttributePerPointMap[EAttributeType::Dexterity].Add(
				TPair<EAttributeType, float>(EAttributeType::ActionSpeed,
				                             AttributePerPoint.ActionSpeedPerDexterity));

			AttributePerPointMap[EAttributeType::Intelligence].Add(
				TPair<EAttributeType, float>(EAttributeType::Intelligence, 1));
			AttributePerPointMap[EAttributeType::Intelligence].Add(
				TPair<EAttributeType, float>(EAttributeType::MaxMP, AttributePerPoint.MaxMPPerIntelligence));
			AttributePerPointMap[EAttributeType::Intelligence].Add(
				TPair<EAttributeType, float>(EAttributeType::MagicalAttack,
				                             AttributePerPoint.MagicalAttackPerIntelligence));
			/*AttributePerPointMap[EAttributeType::Intelligence].Add(
				TPair<EAttributeType, float>(EAttributeType::RecoverMP,
				                             AttributePerPoint.RecoverHitMPPerIntelligence));*/

			AttributePerPointMap[EAttributeType::Willpower].Add(
				TPair<EAttributeType, float>(EAttributeType::Willpower, 1));
			AttributePerPointMap[EAttributeType::Willpower].Add(
				TPair<EAttributeType, float>(EAttributeType::MaxHP, AttributePerPoint.MaxHPPerWillpower));
			AttributePerPointMap[EAttributeType::Willpower].Add(
				TPair<EAttributeType, float>(EAttributeType::MaxMP, AttributePerPoint.MaxMPPerWillpower));
			AttributePerPointMap[EAttributeType::Willpower].Add(
				TPair<EAttributeType, float>(EAttributeType::MaxSP, AttributePerPoint.MaxSPPerWillpower));
			/*AttributePerPointMap[EAttributeType::Willpower].Add(
				TPair<EAttributeType, float>(EAttributeType::RecoverHP, AttributePerPoint.RecoverHPPerWillpower));
			AttributePerPointMap[EAttributeType::Willpower].Add(
				TPair<EAttributeType, float>(EAttributeType::RecoverSP, AttributePerPoint.RecoverSPPerWillpower));
			AttributePerPointMap[EAttributeType::Willpower].Add(
				TPair<EAttributeType, float>(EAttributeType::RecoverMP,
				                             AttributePerPoint.RecoverHitMPPerWillpower));*/
			AttributePerPointMap[EAttributeType::Willpower].Add(
				TPair<EAttributeType, float>(EAttributeType::PhysicalDefense,
				                             AttributePerPoint.PhysicalDefensePerWillpower));
			AttributePerPointMap[EAttributeType::Willpower].Add(
				TPair<EAttributeType, float>(EAttributeType::MagicalDefense,
				                             AttributePerPoint.MagicalDefensePerWillpower));
			AttributePerPointMap[EAttributeType::Willpower].Add(
				TPair<EAttributeType, float>(EAttributeType::ActionSpeed,
				                             AttributePerPoint.ActionSpeedPerWillpower));
			AttributePerPointMap[EAttributeType::Willpower].Add(
				TPair<EAttributeType, float>(EAttributeType::Endurance, AttributePerPoint.EndurancePerWillpower));
			AttributePerPointMap[EAttributeType::Willpower].Add(
				TPair<EAttributeType, float>(EAttributeType::PoisonResist, AttributePerPoint.ResistPerWillpower));
			AttributePerPointMap[EAttributeType::Willpower].Add(
				TPair<EAttributeType, float>(EAttributeType::DeadlyPoisonResist, AttributePerPoint.ResistPerWillpower));
			AttributePerPointMap[EAttributeType::Willpower].Add(
				TPair<EAttributeType, float>(EAttributeType::BurnResist, AttributePerPoint.ResistPerWillpower));
			AttributePerPointMap[EAttributeType::Willpower].Add(
				TPair<EAttributeType, float>(EAttributeType::ChillResist, AttributePerPoint.ResistPerWillpower));
			AttributePerPointMap[EAttributeType::Willpower].Add(
				TPair<EAttributeType, float>(EAttributeType::BleedResist, AttributePerPoint.ResistPerWillpower));
			AttributePerPointMap[EAttributeType::Willpower].Add(
				TPair<EAttributeType, float>(EAttributeType::PetrifactionResist, AttributePerPoint.ResistPerWillpower));


			AttributePerPointMap[EAttributeType::MaxHP].Add(
				TPair<EAttributeType, float>(EAttributeType::MaxHP, AttributePerPoint.MaxHPPerPoint));
			AttributePerPointMap[EAttributeType::MaxSP].Add(
				TPair<EAttributeType, float>(EAttributeType::MaxSP, AttributePerPoint.MaxSPPerPoint));
			AttributePerPointMap[EAttributeType::MaxMP].Add(
				TPair<EAttributeType, float>(EAttributeType::MaxMP, AttributePerPoint.MaxMPPerPoint));
		}
	}
	else
	{
		if (!GIsEditor)
		{
			if (GetOwner() != nullptr && GetOwner()->IsA<APlayerCharacter>())
			{
				UE_LOGFMT(LogActorComponent, Error, "{0}에셋의 포인트당 속성 증가치 테이블이 비었습니다.",
				          GetOwner()->GetActorNameOrLabel());
			}
		}
	}
}

void UAttributeComponent::InitDefaultAttribute()
{
	if (AttributeInitTable)
	{
		FString contextString;

		const FAttributeInit* init = nullptr;
		if (GetOwner()->IsA<APlayerCharacter>())
		{
			init = AttributeInitTable->FindRow<FAttributeInit>("1", contextString);
		}
		else if (GetOwner()->IsA<ABaseMonster>())
		{
			init = AttributeInitTable->FindRow<FAttributeInit>(
				GetOwner<ABaseMonster>()->GetMonsterTag().GetTagName(), contextString);
		}

		if (init == nullptr)
		{
			UE_LOGFMT(LogActorComponent, Error, "{0} : 기본 디폴트 스텟을 가져오지 못했습니다.", GetOwner()->GetActorNameOrLabel());
			return;
		}


		InitStrength(init->Strength);
		InitDexterity(init->Dexterity);
		InitIntelligence(init->Intelligence);
		InitWillpower(init->Willpower);


		InitMaxHP(
			init->MaxHP + (GetStrength() * AttributePerPoint.MaxHPPerStrength) + (GetDexterity() *
				AttributePerPoint.MaxHPPerDexterity) + (GetWillpower() * AttributePerPoint.MaxHPPerWillpower));
		InitMaxMP(
			init->MaxMP + (GetIntelligence() * AttributePerPoint.MaxMPPerIntelligence) + (GetWillpower() *
				AttributePerPoint.MaxMPPerWillpower));
		InitMaxSP(
			init->MaxSP + (GetDexterity() * AttributePerPoint.MaxSPPerDexterity) + (GetWillpower() *
				AttributePerPoint.MaxSPPerWillpower));


		InitActionSpeed(
			init->ActionSpeed + (GetDexterity() * AttributePerPoint.ActionSpeedPerDexterity) + (GetWillpower() *
				+AttributePerPoint.ActionSpeedPerWillpower));


		InitHP(GetMaxHP());
		InitSP(GetMaxSP());
		InitMP(GetMaxMP());


		InitPhysicalAttack(
			init->PhysicalAttack + (GetStrength() * AttributePerPoint.PhysicalAttackPerStrength) + (
				GetDexterity() * AttributePerPoint.PhysicalAttackPerDexterity));
		InitMagicalAttack(
			init->MagicalAttack + (GetDexterity() * AttributePerPoint.MagicalAttackPerDexterity) + (
				GetIntelligence() * AttributePerPoint.MagicalAttackPerIntelligence));

		InitPhysicalDefense(
			init->PhysicalDefense + (GetWillpower() * AttributePerPoint.PhysicalDefensePerWillpower));
		InitMagicalDefense(
			init->MagicalDefense + (GetWillpower() * AttributePerPoint.MagicalDefensePerWillpower));

		InitEndurance(init->Endurance + (GetWillpower() * AttributePerPoint.EndurancePerWillpower));

		InitPoisonResist(init->PoisonResist + (GetWillpower() * AttributePerPoint.ResistPerWillpower));
		InitDeadlyPoisonResist(init->DeadlyPoisonResist + (GetWillpower() * AttributePerPoint.ResistPerWillpower));
		InitBurnResist(init->BurnResist + (GetWillpower() * AttributePerPoint.ResistPerWillpower));
		InitChillResist(init->ChillResist + (GetWillpower() * AttributePerPoint.ResistPerWillpower));
		InitBleedResist(init->BleedResist + (GetWillpower() * AttributePerPoint.ResistPerWillpower));
		InitPetrifactionResist(init->PetrifactionResist + (GetWillpower() * AttributePerPoint.ResistPerWillpower));

		InitMoveSpeed(init->MoveSpeed);
	}
}

void UAttributeComponent::AddLevelUpPoint(EAttributePointType AttributePointType, const int32& Point)
{
	if (AttributeInitTable)
	{
		FString contextString;
		if (const auto init = AttributeInitTable->FindRow<FAttributeInit>("1", contextString))
		{
			switch (AttributePointType)
			{
			case EAttributePointType::LEVEL:
				Level.Init(Level.GetBase() + Point);
				break;
			case EAttributePointType::MAXHP:
				MaxHPPoint.Init(MaxHPPoint.GetBase() + Point);
				MaxHP.LevelUp(AttributePerPoint.MaxHPPerPoint * Point);
				break;
			case EAttributePointType::MAXSP:
				MaxSPPoint.Init(MaxSPPoint.GetBase() + Point);
				MaxSP.LevelUp(AttributePerPoint.MaxSPPerPoint * Point);
				break;
			case EAttributePointType::MAXMP:
				MaxMPPoint.Init(MaxMPPoint.GetBase() + Point);
				MaxMP.LevelUp(AttributePerPoint.MaxMPPerPoint * Point);
				break;
			case EAttributePointType::STR:
				StrengthPoint.Init(StrengthPoint.GetBase() + Point);
				Strength.LevelUp(Point);
				MaxHP.LevelUp(AttributePerPoint.MaxHPPerStrength * Point);
				PhysicalAttack.LevelUp(AttributePerPoint.PhysicalAttackPerStrength * Point);
				break;
			case EAttributePointType::DEX:
				DexterityPoint.Init(DexterityPoint.GetBase() + Point);
				Dexterity.LevelUp(Point);
				MaxHP.LevelUp(AttributePerPoint.MaxHPPerDexterity * Point);
				MaxSP.LevelUp(AttributePerPoint.MaxSPPerDexterity * Point);
				ActionSpeed.LevelUp(AttributePerPoint.ActionSpeedPerDexterity * Point);
				PhysicalAttack.LevelUp(AttributePerPoint.PhysicalAttackPerDexterity * Point);
				MagicalAttack.LevelUp(AttributePerPoint.MagicalAttackPerDexterity * Point);
				break;
			case EAttributePointType::INT:
				IntelligencePoint.Init(IntelligencePoint.GetBase() + Point);
				Intelligence.LevelUp(Point);
				MaxMP.LevelUp(AttributePerPoint.MaxMPPerIntelligence * Point);
				MagicalAttack.LevelUp(AttributePerPoint.MagicalAttackPerIntelligence * Point);
				break;
			case EAttributePointType::WILL:
				WillpowerPoint.Init(WillpowerPoint.GetBase() + Point);
				Willpower.LevelUp(Point);
				MaxHP.LevelUp(AttributePerPoint.MaxHPPerWillpower * Point);
				MaxMP.LevelUp(AttributePerPoint.MaxMPPerWillpower * Point);
				MaxSP.LevelUp(AttributePerPoint.MaxSPPerWillpower * Point);
				ActionSpeed.LevelUp(AttributePerPoint.ActionSpeedPerWillpower * Point);
				PhysicalDefense.LevelUp(AttributePerPoint.PhysicalDefensePerWillpower * Point);
				MagicalDefense.LevelUp(AttributePerPoint.MagicalDefensePerWillpower * Point);
				Endurance.LevelUp(AttributePerPoint.EndurancePerWillpower * Point);

				PoisonResist.LevelUp(AttributePerPoint.ResistPerWillpower);
				DeadlyPoisonResist.LevelUp(AttributePerPoint.ResistPerWillpower);
				BurnResist.LevelUp(AttributePerPoint.ResistPerWillpower);
				ChillResist.LevelUp(AttributePerPoint.ResistPerWillpower);
				BleedResist.LevelUp(AttributePerPoint.ResistPerWillpower);
				PetrifactionResist.LevelUp(AttributePerPoint.ResistPerWillpower);
				break;
			case EAttributePointType::MAX:
				break;
			default: ;
			}
			//업데이트 이후 레벨업 위젯 및 캐릭터 상태정보창을 업데이트 합니다.
			OnCharacterInformationUpdate.Broadcast();
			OnChangeMaxHPValue.Broadcast(GetMaxHP());
			OnChangeMaxMPValue.Broadcast(GetMaxMP());
			OnChangeMaxSPValue.Broadcast(GetMaxSP());

		}
	}
}


const FAttribute* UAttributeComponent::GetAttributeByType(EAttributeType Type)
{
	if (AttributesNotIncludeLevelUpPoint.Contains(Type))
	{
		return AttributesNotIncludeLevelUpPoint[Type];
	}

	return nullptr;
}

const FAttribute* UAttributeComponent::GetLevelUpAttributeByType(EAttributeType Type)
{
	if (LevelUpPointAttributes.Contains(Type))
	{
		return LevelUpPointAttributes[Type];
	}

	return nullptr;
}


void UAttributeComponent::InitProgressWidget() const
{
	BroadcastHPEvent();
	BroadcastMPEvent();
	BroadcastSPEvent();

	BroadcastMaxHPEvent();
	BroadcastMaxMPEvent();
	BroadcastMaxSPEvent();
}

void UAttributeComponent::BroadcastHPEvent() const
{
	OnChangeHPValue.Broadcast(GetHP(), GetMaxHP());
}

void UAttributeComponent::BroadcastMPEvent() const
{
	OnChangeMPValue.Broadcast(GetMP(), GetMaxMP());
}

void UAttributeComponent::BroadcastSPEvent() const
{
	OnChangeSPValue.Broadcast(GetSP(), GetMaxSP());
}

void UAttributeComponent::BroadcastMaxHPEvent() const
{
	OnChangeMaxHPValue.Broadcast(GetMaxHP());
}


void UAttributeComponent::BroadcastMaxMPEvent() const
{
	OnChangeMaxMPValue.Broadcast(GetMaxMP());
}

void UAttributeComponent::BroadcastMaxSPEvent() const
{
	OnChangeMaxSPValue.Broadcast(GetMaxSP());
}

void UAttributeComponent::OnUpdateStatusEffectEvent(EStatusEffect StatusEffect, float Value, float ResistValue)
{
	//UKismetSystemLibrary::PrintString(this,FString::Printf(TEXT("%s 를 %f만큼 증가시킴, 저항치 %f"),*StaticEnum<EStatusEffect>()->GetValueAsString(StatusEffect),Value,ResistValue));
	if (Value <= 0.f)
	{
		if (DecreaseAccTasks.Contains(StatusEffect))
		{
			if (DecreaseAccTasks[StatusEffect].IsValid())
			{
				if (auto task = DecreaseAccTasks[StatusEffect].Get())
				{
					task->EndTask();
				}
			}
		}

		//만약 상태이상이 걸려있었다면, 해제합니다.
		//상태이상을 외부에서 해제하려면, 누적값을 0으로 만든 뒤, 이 델리게이트를 호출합니다.
		if (auto abComp = GetOwner<ABaseCharacter>()->GetAbilityComponent())
		{
			const auto& tag = UAbilityHelperLibrary::GetStatusEffectTag(StatusEffect);
			if (abComp->HasEffectTag(tag))
			{
				abComp->EndEffectByTag(tag);
			}
		}
		return;
	}

	if (!DecreaseAccTasks.Contains(StatusEffect))
	{
		UStatusEffectValueHandler* handler = NewObject<UStatusEffectValueHandler>(this);
		handler->Init(this, StatusEffect);
		DecreaseAccTasks.Add(StatusEffect, UGameplayTask_LaunchEvent::LaunchEvent(GetOwner(), handler));
	}

	if (!DecreaseAccTasks[StatusEffect].IsValid())
	{
		UStatusEffectValueHandler* handler = NewObject<UStatusEffectValueHandler>(this);
		handler->Init(this, StatusEffect);
		DecreaseAccTasks[StatusEffect] = UGameplayTask_LaunchEvent::LaunchEvent(GetOwner(), handler);
	}

	if (auto task = DecreaseAccTasks[StatusEffect].Get())
	{
		if (!task->IsActive())
		{
			task->OnTaskTickWithDelta.Clear();
			task->OnTaskTickWithDelta.AddUniqueDynamic(task->GetAdditionalObject<UStatusEffectValueHandler>(),
			                                           &UStatusEffectValueHandler::ReduceAccValue);
			task->ReadyForActivation();
		}
	}

	//한기는 즉시 적용
	if (StatusEffect == EStatusEffect::CHILL)
	{
		if (Value > 0)
		{
			//상태이상 적용함.
			if (auto abComp = GetOwner<ABaseCharacter>()->GetAbilityComponent())
			{
				{
					const auto& tag = UAbilityHelperLibrary::GetStatusEffectTag(StatusEffect);
					if (!abComp->HasEffectTag(tag))
					{
						if (StatusEffectData != nullptr)
						{
							abComp->K2_ApplyEffect(StatusEffectData->StatusEffect[StatusEffect], GetOwner(),
							                       FOnEffectExpired(), nullptr);
						}
					}
				}


				//한기가 최대치에 달했을때, 동결 부여
				if (Value >= ResistValue)
				{
					{
						UKismetSystemLibrary::PrintString(
							this,TEXT("빙결 적용 : ") + FString::SanitizeFloat(Value) + "/" + FString::SanitizeFloat(
								ResistValue));
						const auto& tag = UAbilityHelperLibrary::GetStatusEffectTag(EStatusEffect::FREEZE);
						if (!abComp->HasEffectTag(tag))
						{
							if (StatusEffectData != nullptr)
							{
								abComp->K2_ApplyEffect(StatusEffectData->StatusEffect[EStatusEffect::FREEZE],
								                       GetOwner(), FOnEffectExpired(), nullptr);
							}
						}
					}
				}
			}
		}
	}


	if (Value >= ResistValue)
	{
		if (StatusEffect != EStatusEffect::CHILL)
		{
			//상태이상 적용함.
			if (auto abComp = GetOwner<ABaseCharacter>()->GetAbilityComponent())
			{
				const auto& tag = UAbilityHelperLibrary::GetStatusEffectTag(StatusEffect);
				if (!abComp->HasEffectTag(tag))
				{
					if (StatusEffectData != nullptr)
					{
						abComp->K2_ApplyEffect(StatusEffectData->StatusEffect[StatusEffect], GetOwner(),
						                       FOnEffectExpired(), nullptr);
					}
				}
			}
		}
	}
}
