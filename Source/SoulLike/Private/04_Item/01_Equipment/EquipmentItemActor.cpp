// Fill out your copyright notice in the Description page of Project Settings.


#include "04_Item/01_Equipment/EquipmentItemActor.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "02_Ability/00_AttributeProcessor/AttributeProcessor.h"
#include "99_Subsystem/AttributeProcessSubsystem.h"
#include "99_Subsystem/EnhancementSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "SoulLike/SoulLike.h"
#define LOCTEXT_NAMESPACE "Equipment"

void AEquipmentItemActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//강화에 대한 기본 정보를 저장합니다. 이 행동으로 최종 강화수치나 강화에 필요한 아이템 정보를 이 액터가 기억하게 됩니다.
	if (const auto data = static_cast<const FEquipmentInformation*>(GetItemInformation()))
	{
		Enhancement = data->Enhancement;
	}
}

FText AEquipmentItemActor::GetEnhancedLevelDescription()
{
	FText enhancedLevelDescription = NSLOCTEXT("Equipment", "EnhancedLevelDescriptionText", "강화단계");
	FText enhancedMaxLevelDescription = NSLOCTEXT("Equipment", "EnhancedMaxLevelDescriptionText", "최대");
	FText enhancedMaxLevel = NSLOCTEXT("Equipment", "EnhancedLevelText", "단계");

	return FText::Format(
		NSLOCTEXT("Equipment", "EnhancedLevelDescription", "<item.desc>{0} : {1}({2} {3}{4})</>"),
		enhancedLevelDescription, FText::AsNumber(Enhancement.CurEnhancement), enhancedMaxLevelDescription,
		FText::AsNumber(Enhancement.GetMaxEnhancement()), enhancedMaxLevel);
	/*
	return TEXT("<item.desc>강화 단계 : ") + FString::FormatAsNumber(Enhancement.CurEnhancement) +
		TEXT(" (최대") + FString::FormatAsNumber(Enhancement.GetMaxEnhancement()) + TEXT("단계)</>");*/
}

FText AEquipmentItemActor::GetAttributeDescription()
{
	if (auto info = static_cast<const FEquipmentInformation*>(GetItemInformation()))
	{
		auto attribute = info->EquipmentAttribute;
		FText defaultStatText = NSLOCTEXT("Equipment", "DefaultStatText", "기본 능력치");

		FString description = "\n";
		if (attribute.Num() > 0)
		{
			for (const auto iter : attribute)
			{
				if (iter.Value != 0)
				{
					//값이 정수인 경우
					if (FMath::IsNearlyEqual(FMath::RoundToFloat(iter.Value), iter.Value))
					{
						description += (AttributeTypeToImageString(iter.AttributeType) + " <item.desc>" +
							GetFloatAsStringWithPrecision(
								iter.Value, 0) + "</>\n");
					}
					//정수가 아닌 경우
					else
					{
						description += (AttributeTypeToImageString(iter.AttributeType) + " <item.desc>" +
							GetFloatAsStringWithPrecision(
								iter.Value, 2) + "</>\n");
					}
				}
			}
		}
		else
		{
			description += "<item.desc>-</>\n";
		}

		return FText::Format(
			NSLOCTEXT("Equipment", "AttributeDescription", "<item.desc>{0}</>{1}"), defaultStatText,
			FText::FromString(description));
	}

	return FText::FromString("ERROR");
}

FText AEquipmentItemActor::GetEnhancedAttributeDescription()
{
	if (auto info = static_cast<const FEquipmentInformation*>(GetItemInformation()))
	{
		FText addStatText = NSLOCTEXT("Equipment", "AddStatText", "추가 능력치");
		//강화로 추가된 능력치 정보를 표시합니다.
		FString description = /*TEXT("<item.desc>추가 능력치</>\n");*/"\n";
		/*UKismetSystemLibrary::PrintString(this,TEXT("현 강화수치 :") + FString::FormatAsNumber(Enhancement.CurEnhancement));*/
		if (Enhancement.CurEnhancement > 0)
		{
			if (auto system = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UEnhancementSubsystem>())
			{
				for (auto iter : Enhancement.EnhancementMap)
				{
					if (iter.Value > 0)
					{
						const float value = system->GetAttributeValuePerEnhancement(iter.Key) * iter.Value;

						if (FMath::IsNearlyEqual(FMath::RoundToFloat(value), value))
						{
							description += (AttributeTypeToImageString(iter.Key) + " <item.desc>" +
								GetFloatAsStringWithPrecision(
									value, 0) + "</>\n");
						}
						else
						{
							description += (AttributeTypeToImageString(iter.Key) + " <item.desc>" +
								GetFloatAsStringWithPrecision(
									value, 2) + "</>\n");
						}
					}
				}

				return FText::Format(
					NSLOCTEXT("Equipment", "EnhancedAttributeDescription", "<item.desc>{0}</>\n{1}"), addStatText,
					FText::FromString(description));
			}
		}
		else
		{
			return FText::Format(
				NSLOCTEXT("Equipment", "EnhancedAttributeNoDescription", "<item.desc>{0}</>\n{1}"), addStatText,
				FText::FromString("-"));
		}
	}

	return FText::FromString("ERROR");
}

FText AEquipmentItemActor::GetFormattedDescription_Implementation()
{
	if (auto info = static_cast<const FEquipmentInformation*>(GetItemInformation()))
	{
		auto defaultDescription = info->Item_Description;

		/*
		auto attribute = info->EquipmentAttribute;

		FString description = TEXT("<item.desc>강화 단계 : ") + FString::FormatAsNumber(info->Enhancement.CurEnhancement) +
			TEXT(" (최대") + FString::FormatAsNumber(Enhancement.GetMaxEnhancement()) + TEXT("단계)</>\n\n");

		description += GetAttributeDescription() + "\n";

		//강화로 추가된 능력치 정보를 표시합니다.
		if (Enhancement.CurEnhancement > 0)
		{
			if (auto system = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UEnhancementSubsystem>())
			{
				description += TEXT("<item.desc>추가 능력치</>\n");

				for (auto iter : Enhancement.EnhancementMap)
				{
					if (iter.Value > 0)
					{
						const float value = system->GetAttributeValuePerEnhancement(iter.Key) * iter.Value;

						if (FMath::IsNearlyEqual(FMath::RoundToFloat(value), value))
						{
							description += (AttributeTypeToImageString(iter.Key) + " <item.desc>" +
								GetFloatAsStringWithPrecision(
									value, 0) + "</>\n");
						}
						else
						{
							description += (AttributeTypeToImageString(iter.Key) + " <item.desc>" +
								GetFloatAsStringWithPrecision(
									value, 2) + "</>\n");
						}
					}
				}
			}
		}

		if (info->Item_Description.IsEmpty() == false)
		{
			description += "\n";
			description += "<item.desc>" + info->Item_Description + "</>";
		}
		*/

		return defaultDescription;
	}

	return Super::GetFormattedDescription_Implementation();
}

void AEquipmentItemActor::OnOverrideSelfEffectEvent_Implementation(const TArray<UAbilityEffect*>& SelfEffect,
                                                                   AActor* Target)
{
	Super::OnOverrideSelfEffectEvent_Implementation(SelfEffect, Target);

	if (auto info = GetItemInformation())
	{
		auto attribute = static_cast<const FEquipmentInformation*>(info)->EquipmentAttribute;

		for (auto e : SelfEffect)
		{
			//장비 정보에 저장된 맵을 돌면서 영향을 줄 이팩트 배열을 새로 생성합니다.
			TArray<FAttributeEffect> attributeEffects;
			for (auto iter : attribute)
			{
				attributeEffects.Add(FAttributeEffect(iter.AttributeType, EAttributeApplyMethod::ADD, iter.Value));
				UE_LOGFMT(LogTemp, Warning, "{0}이 덮어쓴 어트리뷰트 이팩트 : {1} : {2}", ItemTag.ToString(),
				          StaticEnum<EAttributeType>()->GetNameStringByIndex(static_cast<int32>(iter.AttributeType)),
				          iter.Value);
			}

			//새로 생성한 배열을 추가합니다.
			e->AttributeEffects.Append(attributeEffects);
		}
	}
}

void AEquipmentItemActor::UnEquip_Implementation(AActor* Target, const FGuid& ThisItemsUniqueID)
{
	UE_LOGFMT(LogTemp, Log, "장비 장착 해제 시도 : {0}", ItemTag.ToString());
	ForceEndAbility(Target);
	ApplyEnhance(Target, EAttributeApplyMethod::REMOVE);
	UpdateCharacterInfo(Target);
}

bool AEquipmentItemActor::UseItem_Implementation(AActor* Target, const FGuid& ThisItemsUniqueID)
{
	Super::UseItem_Implementation(Target, ThisItemsUniqueID);

	ApplyEnhance(Target, EAttributeApplyMethod::ADD);
	UpdateCharacterInfo(Target);

	return true;
}

void AEquipmentItemActor::ApplyEnhance(AActor* Target, EAttributeApplyMethod Method)
{
	/*const FEquipmentInformation* ItemInfo = static_cast<const FEquipmentInformation*>(GetItemInformation());
	
	if (!ItemInfo)
	{
		// 아이템 정보가 잘못된 경우 처리
		return;
	}*/

	ABaseCharacter* OwningCharacter = Cast<ABaseCharacter>(Target);
	if (!OwningCharacter)
	{
		// 소유 캐릭터를 찾을 수 없는 경우 처리
		return;
	}

	UAttributeComponent* AttributeComponent = OwningCharacter->GetAttributeComponent();
	if (!AttributeComponent)
	{
		// 소유 캐릭터의 속성 컴포넌트를 찾을 수 없는 경우 처리
		return;
	}

	UAttributeProcessSubsystem* AttributeSubsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<
		UAttributeProcessSubsystem>();
	if (!AttributeSubsystem)
	{
		// 속성 서브시스템을 찾을 수 없는 경우 처리
		return;
	}

	UEnhancementSubsystem* EnhancementSubsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<
		UEnhancementSubsystem>();
	if (!EnhancementSubsystem)
	{
		// 강화 서브시스템을 찾을 수 없는 경우 처리
		return;
	}

	// 강화가 된 능력치에 대해서 처리
	for (const auto& iter : Enhancement.EnhancementMap)
	{
		if (iter.Value > 0)
		{
			UE_LOGFMT(LogTemp, Warning, "장비 능력치 설정 : {0}을 {1}만큼 {2} 합니다.",
			          StaticEnum<EAttributeType>()->GetValueAsString(iter.Key), iter.Value,
			          StaticEnum<EAttributeApplyMethod>()->GetValueAsString(Method));
			FAttributeEffect Effect(iter.Key, Method,
			                        EnhancementSubsystem->GetAttributeValuePerEnhancement(iter.Key) * iter.Value);
			AttributeSubsystem->GetProcessor(Method)->ProcessAttributeEffect(AttributeComponent, Effect);
		}
	}
}

void AEquipmentItemActor::UpdateCharacterInfo(AActor* Target)
{
	if (const auto player = Cast<APlayerCharacter>(Target))
	{
		if (const auto attComp = player->GetAttributeComponent())
		{
			//업데이트 이후 레벨업 위젯 및 캐릭터 상태정보창을 업데이트 합니다.
			attComp->OnCharacterInformationUpdate.Broadcast();
		}
	}
}

void AEquipmentItemActor::ForceEndAbility(AActor* Target)
{
	if (const auto player = Cast<APlayerCharacter>(Target))
	{
		if (const auto info = GetItemInformation())
		{
			for (auto ab : info->Abilities)
			{
				const auto& tag = ab.GetDefaultObject()->GetAbilityTag();
				UE_LOGFMT(LogTemp, Log, "ForceEndAbility : {0}", tag.ToString());
				//등록했던 어빌리티를 종료시킵니다. 아이템은 내부적으로 어빌리티 종료시 아이템이 적용한 어빌리티를 컴포넌트에서 제거합니다.
				player->GetAbilityComponent()->ForceEndAbility(tag);
			}
		}
	}
}

void AEquipmentItemActor::ForceApplyAbility(AActor* Target)
{
	UE_LOGFMT(LogTemp, Log, "ForceApplyAbility : {0}", ItemUniqueID.ToString());
	UseItem(Target, ItemUniqueID);

	if (auto attComp = Cast<ABaseCharacter>(Target)->GetAttributeComponent())
	{
		UE_LOGFMT(LogTemp, Log, "재 사용후 체력수치 : {0}", attComp->GetMaxHP());
	}

	/*UseItem(Target, RealItemUniqueIDFromInventory);*/
}

void AEquipmentItemActor::IncreaseEnhance(EAttributeType IncreaseAttribute)
{
	Enhancement.IncreaseEnhance(IncreaseAttribute);

	UE_LOGFMT(LogTemp, Log, "강화 수치가 증가했습니다 : {0}", Enhancement.CurEnhancement);
	for (auto iter : Enhancement.EnhancementMap)
	{
		UE_LOGFMT(LogTemp, Log, "강화 상태 : {0} {1}", StaticEnum<EAttributeType>()->GetValueAsString(iter.Key),
		          iter.Value);
	}
}

#undef LOCTEXT_NAMESPACE
