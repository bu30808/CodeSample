// Fill out your copyright notice in the Description page of Project Settings.


#include "00_EditorUtilityWidget/00_Ability/AbilityCreatorWidget_Req.h"

#include "02_Ability/AbilityRequirement.h"
#include "02_Library/BlueprintHelperLibrary.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/DetailsView.h"
#include "Components/EditableTextBox.h"
#include "Components/SinglePropertyView.h"
#include "Components/VerticalBox.h"
#include "Logging/StructuredLog.h"

void UAbilityCreatorWidget_Req::OnClicked()
{
	if (VerticalBox_Req->IsVisible())
	{
		VerticalBox_Req->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		VerticalBox_Req->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UAbilityCreatorWidget_Req::OnCheckCost(bool bIsChecked)
{
	if (bIsChecked)
	{
		DetailsView_Cost->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		if (DetailsView_Cost->GetObject())
		{
			DetailsView_Cost->GetObject()->ConditionalBeginDestroy();
		}
		DetailsView_Cost->SetObject(NewObject<UCostCreateInfo>());
	}
	else
	{
		UE_LOGFMT(LogTemp, Log, "코스트 체크 해제");
		DetailsView_Cost->SetVisibility(ESlateVisibility::Collapsed);

		if (DetailsView_Cost->GetObject())
		{
			DetailsView_Cost->GetObject()->ConditionalBeginDestroy();
		}
		DetailsView_Cost->SetObject(nullptr);
	}
}

void UAbilityCreatorWidget_Req::OnCheckCooldown(bool bIsChecked)
{
	if (bIsChecked)
	{
		DetailsView_Cooldown->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		if (DetailsView_Cooldown->GetObject())
		{
			DetailsView_Cooldown->GetObject()->ConditionalBeginDestroy();
		}

		DetailsView_Cooldown->SetObject(NewObject<UCooldownCreateInfo>());
	}
	else
	{
		UE_LOGFMT(LogTemp, Log, "쿨다운 체크 해제");
		DetailsView_Cooldown->SetVisibility(ESlateVisibility::Collapsed);
		if (DetailsView_Cooldown->GetObject())
		{
			DetailsView_Cooldown->GetObject()->ConditionalBeginDestroy();
		}
		DetailsView_Cooldown->SetObject(nullptr);
	}
}

void UAbilityCreatorWidget_Req::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (Button_Dropdown)
	{
		Button_Dropdown->OnClicked.AddUniqueDynamic(this, &UAbilityCreatorWidget_Req::OnClicked);
	}

	if (CheckBox_Cost)
	{
		CheckBox_Cost->OnCheckStateChanged.AddUniqueDynamic(this, &UAbilityCreatorWidget_Req::OnCheckCost);
	}

	if (CheckBox_Cooldown)
	{
		CheckBox_Cooldown->OnCheckStateChanged.AddUniqueDynamic(this, &UAbilityCreatorWidget_Req::OnCheckCooldown);
	}


	if (DetailsView_EffectTag)
	{
		DetailsView_EffectTag->SetObject(NewObject<UReqTagInfo>());
	}

	if (DetailsView_AbilityTag)
	{
		DetailsView_AbilityTag->SetObject(NewObject<UReqAbilityTagInfo>());
	}
}

TSubclassOf<UAbilityEffect> UAbilityCreatorWidget_Req::CreateCoolDown(const FString& Path, const FString& AbilityName)
{
	if (auto data = DetailsView_Cooldown->GetObject())
	{
		auto fullPath = Path + "/AE_" + AbilityName + "CoolDown";
		auto cooldownInfo = Cast<UCooldownCreateInfo>(data);
		FString msg;
		bool bSucc;
		auto bp = UBlueprintHelperLibrary::CreateBlueprint(fullPath, UAbilityEffect::StaticClass(), bSucc, msg);
		if (bSucc)
		{
			auto effect = Cast<UAbilityEffect>(bp->GeneratedClass.GetDefaultObject());
			effect->EffectApplyType = EEffectApplyType::Duration;
			effect->Time = cooldownInfo->DurationTime;
			effect->UniqueEffectTag = cooldownInfo->EffectTag;

			effect->MarkPackageDirty();

			return effect->GetClass();
		}
	}

	return nullptr;
}

TSubclassOf<UAbilityEffect> UAbilityCreatorWidget_Req::CreateCost(const FString& Path, const FString& AbilityName)
{
	if (auto data = DetailsView_Cost->GetObject())
	{
		auto fullPath = Path + "/AE_" + AbilityName + "Cost";
		auto costInfo = Cast<UCostCreateInfo>(data);
		FString msg;
		bool bSucc;
		auto bp = UBlueprintHelperLibrary::CreateBlueprint(fullPath, UAbilityEffect::StaticClass(), bSucc, msg);
		if (bSucc)
		{
			auto effect = Cast<UAbilityEffect>(bp->GeneratedClass.GetDefaultObject());
			effect->AttributeEffects = costInfo->AttributeEffects;
			effect->UniqueEffectTag = costInfo->EffectTag;
			effect->EffectApplyType = costInfo->EffectApplyType;

			effect->MarkPackageDirty();

			return effect->GetClass();
		}
	}

	return nullptr;
}

UAbilityRequirement* UAbilityCreatorWidget_Req::CreateAR(const FString& Path, const FString& AbilityName)
{
	auto fullPath = Path + "/AR_" + AbilityName;
	FString msg;
	bool bSucc;
	auto bp = UBlueprintHelperLibrary::CreateBlueprint(fullPath, UAbilityRequirement::StaticClass(), bSucc, msg);

	if (bSucc == false)
	{
		UBlueprintHelperLibrary::ShowAlertDialog(FText::FromString(msg));
		return nullptr;
	}

	if (bp)
	{
		auto req = Cast<UAbilityRequirement>(bp->GeneratedClass.GetDefaultObject());
		req->Cooldown = CreateCoolDown(Path, AbilityName);
		req->Cost = CreateCost(Path, AbilityName);


		if (auto tagInfo = Cast<UReqTagInfo>(DetailsView_EffectTag->GetObject()))
		{
			req->NeedEffectTags = tagInfo->NeedEffectTags;
			req->NotNeedEffectTags = tagInfo->NotNeedEffectTags;
		}

		if (auto abTagInfo = Cast<UReqAbilityTagInfo>(DetailsView_AbilityTag->GetObject()))
		{
			req->NeedActivatedAbilityTags = abTagInfo->NeedActivatedAbilityTags;
			req->NotNeedActivatedAbilityTags = abTagInfo->NotNeedActivatedAbilityTags;
		}

		req->MarkPackageDirty();

		return req;
	}

	return nullptr;
}
