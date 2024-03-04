// Fill out your copyright notice in the Description page of Project Settings.


#include "04_Item/01_Equipment/00_Spirit/EquipmentItemActor_Spirit.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "96_Library/AbilityHelperLibrary.h"
#include "99_Subsystem/ItemManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"


const FItemInformation* AEquipmentItemActor_Spirit::GetItemInformation()
{
	if (auto instance = UGameplayStatics::GetGameInstance(this))
	{
		if (auto system = instance->GetSubsystem<UItemManagerSubsystem>())
		{
			//여기서 무엇인가 하세요.
			return system->GetSpiritItemInformation(ItemTag);
		}
	}
#ifndef WITH_EDITOR
	UE_LOGFMT(LogTemp,Error,"서브시스템을 가져올 수 없습니다 : {0} {1}",__FUNCTION__,__LINE__);
#endif

	return nullptr;
}

bool AEquipmentItemActor_Spirit::UseItem_Implementation(AActor* Target, const FGuid& ThisItemsUniqueID)
{
	Super::UseItem_Implementation(Target, ThisItemsUniqueID);

	if (const auto info = static_cast<const FWeaponInformation*>(GetItemInformation()))
	{
		if (const auto player = Cast<APlayerCharacter>(Target))
		{
			ItemOwner = player;

			//애니메이션 블루프린트를 적용합니다.
			if (info->AnimationSet)
			{
				player->GetMesh()->SetAnimInstanceClass(info->AnimationSet);
			}
			else
			{
				UE_LOGFMT(LogTemp, Error, "장착하는 무기 아이템의 애니메이션 블루프린트가 nullptr입니다.");
			}

			//사용 가능한 어빌리티를 등록합니다.
			for (auto ab : info->GiveAbilities)
			{
				if (auto load = ab.LoadSynchronous())
				{
					player->GetAbilityComponent()->GiveAbility(load);
					const auto& abilityTag = load->GetDefaultObject<UAbilityBase>()->GetAbilityTag();
					GivenActiveAbilityTags.Add(abilityTag);

					UE_LOGFMT(LogTemp, Log, "등록하려는 어빌리티 태그 : {0}", abilityTag.ToString());
					if (UAbilityHelperLibrary::IsAttackAbilityTag(abilityTag))
					{
						if (UAbilityHelperLibrary::IsNormalAttackAbilityTag(abilityTag))
						{
							NormalAttackTag = abilityTag;
							UE_LOGFMT(LogTemp, Warning, "기본 공격 태그를 저장합니다 : {0}", NormalAttackTag.ToString());
							continue;
						}

						if (UAbilityHelperLibrary::IsStrongAttackAbilityTag(abilityTag))
						{
							StrongAttackTag = abilityTag;
							UE_LOGFMT(LogTemp, Warning, "강 공격 태그를 저장합니다 : {0}", StrongAttackTag.ToString());
							continue;
						}
					}

					if (UAbilityHelperLibrary::IsDodgeAbilityTag(abilityTag))
					{
						DodgeTag = abilityTag;
					}

					if (UAbilityHelperLibrary::IsDodgeContAbilityTag(abilityTag))
					{
						DodgeContTag = abilityTag;
					}
				}
			}

			return true;
		}
	}
	return false;
}

void AEquipmentItemActor_Spirit::UnEquip_Implementation(AActor* Target, const FGuid& ThisItemsUniqueID)
{
	Super::UnEquip_Implementation(Target, ThisItemsUniqueID);

	if (const auto info = static_cast<const FWeaponInformation*>(GetItemInformation()))
	{
		if (const auto player = Cast<APlayerCharacter>(Target))
		{
			//애니메이션 블루프린트를 되돌립니다.
			player->GetMesh()->SetAnimInstanceClass(player->GetDefaultAnimInstance());
			//등록했던 사용 가능한 어빌리티를 제거합니다.
			for (auto tag : GivenActiveAbilityTags)
			{
				player->GetAbilityComponent()->RemoveAbility(tag);
			}
		}
	}
}

void AEquipmentItemActor_Spirit::OnEndAbilityEvent_Implementation()
{
	if (ItemOwner.IsValid())
	{
		if (const auto info = static_cast<const FWeaponInformation*>(GetItemInformation()))
		{
			for (auto ab : info->GiveAbilities)
			{
				for (auto tag : GivenActiveAbilityTags)
				{
					ItemOwner->GetAbilityComponent()->RemoveAbility(tag);
				}
			}
		}
	}

	Super::OnEndAbilityEvent_Implementation();
}
