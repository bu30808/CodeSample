// Fill out your copyright notice in the Description page of Project Settings.


#include "04_Item/01_Equipment/03_Orb/EquipmentItemActor_OrbFragment.h"

#include "GameplayTagsManager.h"
#include "NiagaraComponent.h"
#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "02_Ability/AbilityBase.h"
#include "02_Ability/00_AttributeProcessor/AttributeProcessor.h"
#include "99_Subsystem/AttributeProcessSubsystem.h"
#include "99_Subsystem/ItemManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "NiagaraSystem.h"
#include "96_Library/ItemHelperLibrary.h"
#include "SoulLike/SoulLike.h"

DEFINE_LOG_CATEGORY(LogFragment)

AEquipmentItemActor_OrbFragment::AEquipmentItemActor_OrbFragment()
{
	{
		static ConstructorHelpers::FObjectFinder<UNiagaraSystem> system(TEXT(
			"/Script/Niagara.NiagaraSystem'/Game/Blueprints/04_Item/NS_Item_Orb.NS_Item_Orb'"));
		if (system.Succeeded())
		{
			NiagaraComponent->SetAsset(system.Object);
		}
	}
}

void AEquipmentItemActor_OrbFragment::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

#if WITH_EDITOR
void AEquipmentItemActor_OrbFragment::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	//CreateFragmentInfo();
}
#endif

void AEquipmentItemActor_OrbFragment::BeginPlay()
{
	Super::BeginPlay();
	//CreateFragmentInfo();
}

FText AEquipmentItemActor_OrbFragment::GetFormattedDescription_Implementation()
{
	FFormatOrderedArguments args;
	args.Add(FText::FromString("<orb.name>" + GetItemInformation()->Item_Name.ToString() + "</>\n")); // -1

	
		const auto frag = static_cast<const FOrbFragmentInformation*>(GetItemInformation());

		args.Add(SlotTypeToDecoText(frag->SlotType)); //0
		args.Add(FText::FromString(OrbMatrixSlotTypeToText(frag->SlotType).ToString() + "</>\n")); //1
		args.Add(RarityToDecoText(frag->Rarity)); //2
		args.Add(FText::FromString(RarityToText(frag->Rarity).ToString() + "</>\n")); //3
	
		FText additionalAbilityText = NSLOCTEXT("FragmentToolTipHelper", "FragmentAdditionalAbilityText", "추가 특성");
		args.Add(FText::FromString(additionalAbilityText.ToString() + "\n")); //4

	if (frag->Abilities.Num() > 0)
	{
		FString abilitys = "";

		for (auto ab : frag->Abilities)
		{
			abilitys += "<orb.talent>" + ab.GetDefaultObject()->GetAbilityInformation().AbilityName.ToString() +
				"</>\n";
			abilitys += "<orb.talent.desc>" + ab.GetDefaultObject()->GetAbilityInformation().AbilityDescription.
												 ToString() + "</>\n";
		}

		args.Add(FText::FromString(abilitys + "\n")); //5
	}
	else
	{
		args.Add(FText::FromString("-\n")); //5
	}

	if (GetItemInformation()->bSellable)
	{
		FString sell = "\n" + AttributeTypeToImageString(EAttributeType::EXP) + " <price>" +
			FString::FormatAsNumber(GetItemInformation()->Item_Price) + "</>";
		args.Add(FText::FromString(sell)); //6
		/*
		resultMsg += "\n" + AttributeTypeToImageString(EAttributeType::EXP) + " <price>" +
			FString::FormatAsNumber(itemInfo->Item_Price) + "</>";*/
	}
	else
	{
		FText notSellableText = NSLOCTEXT("FragmentToolTipHelper", "NotSellableText", "판매불가");
		args.Add(FText::FromString("\n<error>" + notSellableText.ToString() + "</>")); //6
		//resultMsg += TEXT("\n<error>판매불가</>");
	}

	return FText::Format(NSLOCTEXT("FragmentToolTipHelper", "FragmentToolTipText",
							   "{0}{1}{2}{3}{4}{5}{6}"), args);
}

/*
FString AEquipmentItemActor_OrbFragment::GetFormattedDescription_Implementation()
{
	auto description =  Super::GetFormattedDescription_Implementation();
	description+="\n\n";
	if(FragmentAbility.Num()>0)
	{
		description += TEXT("<item.desc>추가 특성</>\n");
		for(auto f : FragmentAbility)
		{
			description+= "<orb.talent>" + f.GetDefaultObject()->GetAbilityInformation().AbilityName+"</>\n";
			description+= "<orb.talent.desc>" + f.GetDefaultObject()->GetAbilityInformation().AbilityDescription+"</>\n";
		}
		description+="\n";
	}


	return description;
}*/


/*
void AEquipmentItemActor_OrbFragment::CreateFragmentInfo()
{
	if (bCreateInfo)
	{
		if (FragmentInformation.IsValid())
		{
			UE_LOGFMT(LogTemp, Warning, "이미 정보가 할당되어 있습니다.");
			return;
		}


		if (bRandomFragment)
		{
			UE_LOGFMT(LogTemp, Warning, "랜덤 파편 정보를 생성합니다.");
			FragmentInformation = MakeShared<FOrbFragmentInformation>();

			if (const auto data = FragmentInformation)
			{
				data->ItemActorObject = StaticClass();
				data->Item_Name = TEXT("신력 파편");
				data->Item_Description = TEXT("흩어진 신력이 응축되어 생긴 파편. 응축된 신력에 따라 다른 힘을 가진다고 한다.");
				data->Item_Tag = FGameplayTag::RequestGameplayTag("Item.Equipment.Orb.Fragment");


				data->OrbType = EOrbType::FRAGMENT;
				data->Rarity = GetRandomRarity();
				data->SlotType = GetRandomSlotType();

				data->EquipmentAttribute = GetRandomEquipAttribute(data->SlotType, data->Rarity);

				data->Abilities = GetRandomAbilities(data->SlotType);
			}
		}
	}
	else
	{
		//파편 기본정보만 설정해 줍니다.
		if (FragmentInformation.IsValid() == false)
		{
			FragmentInformation = MakeShared<FOrbFragmentInformation>();

			if (const auto data = FragmentInformation)
			{
				data->ItemActorObject = LoadClass<AEquipmentItemActor_OrbFragment>(GetWorld(), *GetPathName());
				data->Item_Name = TEXT("파편");
				data->Item_Description = TEXT("어떤 힘을 가지고 있을지 모른다.");
				data->Item_Tag = FGameplayTag::RequestGameplayTag("Item.Equipment.Orb.Fragment");


				data->OrbType = EOrbType::FRAGMENT;
			}
		}
	}
}
*/

const FItemInformation* AEquipmentItemActor_OrbFragment::GetItemInformation()
{
	/*if (bRandomFragment)
	{
		return FragmentInformation.Get();
	}
	*/

	if (auto instance = UGameplayStatics::GetGameInstance(this))
	{
		if (auto system = instance->GetSubsystem<UItemManagerSubsystem>())
		{
			//여기서 무엇인가 하세요.
			return system->GetOrbFragmentItemInformation(ItemTag);
		}
	}
#ifndef WITH_EDITOR
	UE_LOGFMT(LogFragment,Error,"서브시스템을 가져올 수 없습니다 : {0} {1}",__FUNCTION__,__LINE__);
#endif

	return nullptr;
}


bool AEquipmentItemActor_OrbFragment::UseItem_Implementation(AActor* Target, const FGuid& ThisItemsUniqueID)
{
	UE_LOGFMT(LogFragment, Log, "프래그먼트 장착 : {0}", ThisItemsUniqueID.ToString());
	/*if (bRandomFragment)
	{
		ApplyRandomFragment();
	}
	else
	{
		//ApplyFragmentTalent();
	}*/

	return Super::UseItem_Implementation(Target, ThisItemsUniqueID);
}

void AEquipmentItemActor_OrbFragment::UnEquip_Implementation(AActor* Target, const FGuid& ThisItemsUniqueID)
{
	Super::UnEquip_Implementation(Target, ThisItemsUniqueID);
	UE_LOGFMT(LogFragment, Log, "프래그먼트 장착 해제 : {0}", ThisItemsUniqueID.ToString());

	/*if (bRandomFragment)
	{
		UnApplyRandomFragment();
	}*/
}

/*
void AEquipmentItemActor_OrbFragment::OverrideInfo(const FItemInformation* ItemInfo)
{
	if (FragmentInformation.IsValid())
	{
		if (const auto data = FragmentInformation)
		{
			if (auto frag = static_cast<const FOrbFragmentInformation*>(ItemInfo))
			{
				//data->ItemActorObject = frag->ItemActorObject;
				data->Item_Name = frag->Item_Name;
				data->Item_Description = frag->Item_Description;
				data->Item_Tag = frag->Item_Tag;


				data->OrbType = frag->OrbType;
				data->Rarity = frag->Rarity;
				data->SlotType = frag->SlotType;

				data->EquipmentAttribute = frag->EquipmentAttribute;

				data->Abilities = frag->Abilities;
			}
		}
	}
}
*/

void AEquipmentItemActor_OrbFragment::SpawnPreSetting()
{
	Super::SpawnPreSetting();
	bCreateInfo = false;
}

/*
TArray<TSubclassOf<UAbilityBase>> AEquipmentItemActor_OrbFragment::GetRandomAbilities(EOrbMatrixSlotType SlotType)
{
	TArray<TSubclassOf<UAbilityBase>> arr;

	if (RandomAbilityDataAsset != nullptr && RandomAbilityDataAsset->RandomAbility.Contains(SlotType) &&
		FMath::RandBool())
	{
		const auto& abList = RandomAbilityDataAsset->RandomAbility[SlotType].Ability;

		const auto index = FMath::RandRange(0, abList.Num() - 1);
		if (abList.IsValidIndex(index))
		{
			arr.Add(abList[index]);
		}
	}

	return arr;
}

void AEquipmentItemActor_OrbFragment::ApplyRandomFragment()
{
	if (bRandomFragment)
	{
		if (FragmentInformation.IsValid())
		{
			if (auto attComp = GetOwner<ABaseCharacter>()->GetAttributeComponent())
			{
				if (auto subsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<
					UAttributeProcessSubsystem>())
				{
					for (auto equipAttribute : FragmentInformation->EquipmentAttribute)
					{
						FAttributeEffect effect(equipAttribute.AttributeType, EAttributeApplyMethod::ADD,
												equipAttribute.Value);
						subsystem->GetProcessor(EAttributeApplyMethod::ADD)->ProcessAttributeEffect(attComp, effect);
					}

					attComp->OnCharacterInformationUpdate.Broadcast();
				}
			}

			if (auto abComp = GetOwner<ABaseCharacter>()->GetAbilityComponent())
			{
				for (auto ab : FragmentInformation->Abilities)
				{
					if (abComp->GiveAbility(ab))
					{
						GivenRandomAbilitiesByFragment.Add(ab.GetDefaultObject()->GetAbilityTag());
					}
				}
			}
		}
	}
}

void AEquipmentItemActor_OrbFragment::UnApplyRandomFragment()
{
	if (bRandomFragment)
	{
		if (FragmentInformation.IsValid())
		{
			if (const auto attComp = GetOwner<ABaseCharacter>()->GetAttributeComponent())
			{
				if (const auto subsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<
					UAttributeProcessSubsystem>())
				{
					for (const auto equipAttribute : FragmentInformation->EquipmentAttribute)
					{
						FAttributeEffect effect(equipAttribute.AttributeType, EAttributeApplyMethod::REMOVE,
												equipAttribute.Value);
						subsystem->GetProcessor(EAttributeApplyMethod::REMOVE)->ProcessAttributeEffect(attComp, effect);
					}
					attComp->OnCharacterInformationUpdate.Broadcast();
				}
			}

			if (const auto abComp = GetOwner<ABaseCharacter>()->GetAbilityComponent())
			{
				for (const auto tag : GivenRandomAbilitiesByFragment)
				{
					abComp->ForceEndAbility(tag);
					abComp->RemoveAbility(tag);
				}
			}
		}
	}
}


EItemRarity AEquipmentItemActor_OrbFragment::GetRandomRarity()
{
	return static_cast<EItemRarity>(FMath::RandRange(static_cast<int32>(EItemRarity::Common),
	                                                 static_cast<int32>(EItemRarity::Legendary)));
}

EOrbMatrixSlotType AEquipmentItemActor_OrbFragment::GetRandomSlotType()
{
	return static_cast<EOrbMatrixSlotType>(FMath::RandRange(static_cast<int32>(EOrbMatrixSlotType::PHYSICAL),
	                                                        static_cast<int32>(EOrbMatrixSlotType::FREE)));
}

int32 AEquipmentItemActor_OrbFragment::GetAttributeCountByRarity(EItemRarity Rarity)
{
	switch (Rarity)
	{
	case EItemRarity::Common:
		return FMath::RandRange(0, 1);
	case EItemRarity::Rare:
		return FMath::RandRange(1, 2);
	case EItemRarity::Epic:
		return FMath::RandRange(2, 3);
	case EItemRarity::Legendary:
		return FMath::RandRange(3, 4);
	default: ;
	}

	return -1;
}

float AEquipmentItemActor_OrbFragment::GetRandomAttributeValue(EAttributeType RandAttribute, EItemRarity Rarity)
{
	float result = 0;
	switch (RandAttribute)
	{
	case EAttributeType::MaxHP:
		result = FMath::RandRange(50, 100);
		break;
	case EAttributeType::MaxMP:
		result = FMath::RandRange(1, 5);
		break;
	case EAttributeType::MaxSP:
		result = FMath::RandRange(2, 10);
		break;
	case EAttributeType::RecoverHP:
	case EAttributeType::RecoverSP:
	case EAttributeType::RecoverMP:
		result = FMath::RandRange(0.01f, 0.05f);
		break;
	case EAttributeType::PhysicalAttack:
	case EAttributeType::MagicalAttack:
		result = FMath::RandRange(1, 5);
		break;
	case EAttributeType::PhysicalDefense:
	case EAttributeType::MagicalDefense:
		result = FMath::RandRange(1, 5);
		break;
	case EAttributeType::ActionSpeed:
		result = FMath::RandRange(0.025f, 0.05f);
		break;
	case EAttributeType::Endurance:
	case EAttributeType::Strength:
	case EAttributeType::Dexterity:
	case EAttributeType::Intelligence:
	case EAttributeType::Willpower:
		result = FMath::RandRange(1, 5);
		break;
	default: ;
	}

	switch (Rarity)
	{
	case EItemRarity::Common:
		break;
	case EItemRarity::Rare:
		result *= 1.2f;
		break;
	case EItemRarity::Epic:
		result *= 1.35f;
		break;
	case EItemRarity::Legendary:
		result *= 1.5f;
		break;
	default: ;
	}

	//일부 타입에 따라서 정수로 반올림합니다.
	switch (RandAttribute)
	{
	case EAttributeType::MaxHP:
	case EAttributeType::MaxMP:
	case EAttributeType::MaxSP:
	case EAttributeType::PhysicalAttack:
	case EAttributeType::MagicalAttack:
	case EAttributeType::PhysicalDefense:
	case EAttributeType::MagicalDefense:
	case EAttributeType::Endurance:
	case EAttributeType::Strength:
	case EAttributeType::Dexterity:
	case EAttributeType::Intelligence:
	case EAttributeType::Willpower:
		result = FMath::RoundToInt(result);
		break;
	case EAttributeType::RecoverHP:
	case EAttributeType::RecoverSP:
	case EAttributeType::RecoverMP:
	case EAttributeType::ActionSpeed:
		result = FMath::RoundToInt(result * 100) / 100.f;
		break;
	default: ;
	}

	return result;
}

EAttributeType AEquipmentItemActor_OrbFragment::GetRandomAttribute()
{
	auto rand = static_cast<EAttributeType>(FMath::RandRange(static_cast<int32>(EAttributeType::NONE) + 1,
	                                                         static_cast<int32>(EAttributeType::MAX) - 1));

	if (rand == EAttributeType::HP)
	{
		return EAttributeType::MaxHP;
	}

	if (rand == EAttributeType::MP)
	{
		return EAttributeType::MaxMP;
	}

	if (rand == EAttributeType::SP)
	{
		return EAttributeType::MaxSP;
	}

	return rand;
}

EAttributeType AEquipmentItemActor_OrbFragment::GetRandomRelativeAttribute(EOrbMatrixSlotType SlotType)
{
	TArray<EAttributeType> attributeArr;

	switch (SlotType)
	{
	case EOrbMatrixSlotType::PHYSICAL:
		attributeArr.Add(EAttributeType::PhysicalAttack);
		attributeArr.Add(EAttributeType::Strength);
		attributeArr.Add(EAttributeType::HP);
		attributeArr.Add(EAttributeType::RecoverHP);
		break;
	case EOrbMatrixSlotType::DEFENCE:
		attributeArr.Add(EAttributeType::Dexterity);
		attributeArr.Add(EAttributeType::PhysicalAttack);
		attributeArr.Add(EAttributeType::MagicalAttack);
		attributeArr.Add(EAttributeType::SP);
		attributeArr.Add(EAttributeType::RecoverSP);
		attributeArr.Add(EAttributeType::ActionSpeed);
		attributeArr.Add(EAttributeType::PhysicalDefense);
		attributeArr.Add(EAttributeType::MagicalDefense);
		break;
	case EOrbMatrixSlotType::MAGICAL:
		attributeArr.Add(EAttributeType::Intelligence);
		attributeArr.Add(EAttributeType::MagicalAttack);
		attributeArr.Add(EAttributeType::RecoverMP);
		break;
	case EOrbMatrixSlotType::FREE:
		return GetRandomAttribute();
	default: ;
	}

	return attributeArr[FMath::RandRange(0, attributeArr.Num() - 1)];
}

TArray<FEquipmentAttribute> AEquipmentItemActor_OrbFragment::GetRandomEquipAttribute(EOrbMatrixSlotType SlotType,
	EItemRarity Rarity)
{
	const int32 addAttributeCount = GetAttributeCountByRarity(Rarity) + 1;


	TArray<FEquipmentAttribute> equipAttribute;

	for (auto i = 0; i < addAttributeCount; i++)
	{
		EAttributeType randAttribute;
		if (bRelativeRandom)
		{
			randAttribute = GetRandomRelativeAttribute(SlotType);
		}
		else
		{
			randAttribute = GetRandomAttribute();
		}

		const float randValue = GetRandomAttributeValue(randAttribute, Rarity);
		if (equipAttribute.Contains(randAttribute))
		{
			auto find = equipAttribute.FindByPredicate([randAttribute](const FEquipmentAttribute& Att)
			{
				return Att.AttributeType == randAttribute;
			});
			find->Value += randValue;
		}
		else
		{
			equipAttribute.Add(FEquipmentAttribute(randAttribute, randValue));
		}
	}

	return equipAttribute;
}
*/
