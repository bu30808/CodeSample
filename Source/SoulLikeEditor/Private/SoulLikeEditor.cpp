#include "SoulLikeEditor.h"

#include "AssetToolsModule.h"
#include "01_Factory/OrbMatrixFactory.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "FSoulLikeEditorModule"

TSharedPtr<FSlateStyleSet> FSoulLikeEditorModule::abilityCueIconInstance = nullptr;
TSharedPtr<FSlateStyleSet> FSoulLikeEditorModule::abilityIconInstance = nullptr;
TSharedPtr<FSlateStyleSet> FSoulLikeEditorModule::abilityEffectIconInstance = nullptr;
TSharedPtr<FSlateStyleSet> FSoulLikeEditorModule::abilityRequirementIconInstance = nullptr;
TSharedPtr<FSlateStyleSet> FSoulLikeEditorModule::itemIconInstance = nullptr;




void FSoulLikeEditorModule::StartupModule()
{
	SetAbilityIcon();
	SetAbilityEffectIcon();
	SetAbilityRequirementIcon();
	SetCueIcon();


	SetItemIcon();


	auto& Tool = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	const auto Category = Tool.RegisterAdvancedAssetCategory(
		TEXT("OrbMatrix"), FText::FromString(TEXT("신력구슬 메트릭스")));

	Tool.RegisterAssetTypeActions(MakeShareable(new FOrbMatrixAssetActions(Category)));


}

void FSoulLikeEditorModule::ShutdownModule()
{


	if (abilityIconInstance.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*abilityIconInstance.Get());
		ensure(abilityIconInstance.IsUnique());
		abilityIconInstance.Reset();
	}

	if (abilityEffectIconInstance.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*abilityEffectIconInstance.Get());
		ensure(abilityEffectIconInstance.IsUnique());
		abilityEffectIconInstance.Reset();
	}

	if (abilityRequirementIconInstance.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*abilityRequirementIconInstance.Get());
		ensure(abilityRequirementIconInstance.IsUnique());
		abilityRequirementIconInstance.Reset();
	}

	if (abilityCueIconInstance.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*abilityCueIconInstance.Get());
		ensure(abilityCueIconInstance.IsUnique());
		abilityCueIconInstance.Reset();
	}

	if (itemIconInstance.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*itemIconInstance.Get());
		ensure(itemIconInstance.IsUnique());
		itemIconInstance.Reset();
	}
}

void FSoulLikeEditorModule::SetAbilityIcon()
{
	if (abilityIconInstance.IsValid() == false)
	{
		abilityIconInstance = MakeShareable(new FSlateStyleSet("AbilityIcon"));

		FString ThumbnailFile = FPaths::ProjectDir() / TEXT("IconResource") / TEXT("Ability.png");
		// ClassThumbnail.MyActor에서 MyActor 부분에 원하시는 클래스명을 넣으시면 됩니다.
		abilityIconInstance->Set(FName(TEXT("ClassThumbnail.AbilityBase")),
		                         new FSlateImageBrush(ThumbnailFile, FVector2D(64.f, 64.f)));

		// ClassIcon.MyActor에서 MyActor 부분에 원하시는 클래스명을 넣으시면 됩니다.
		FString IconFile = FPaths::ProjectDir() / TEXT("IconResource") / TEXT("Ability.png");
		abilityIconInstance->Set(FName(TEXT("ClassIcon.AbilityBase")),
		                         new FSlateImageBrush(IconFile, FVector2D(16.f, 16.f)));

		FSlateStyleRegistry::RegisterSlateStyle(*abilityIconInstance.Get());
	}
}

void FSoulLikeEditorModule::SetAbilityEffectIcon()
{
	if (abilityEffectIconInstance.IsValid() == false)
	{
		abilityEffectIconInstance = MakeShareable(new FSlateStyleSet("AbilityEffectIcon"));

		FString ThumbnailFile = FPaths::ProjectDir() / TEXT("IconResource") / TEXT("AbilityEffect.png");
		// ClassThumbnail.MyActor에서 MyActor 부분에 원하시는 클래스명을 넣으시면 됩니다.
		abilityEffectIconInstance->Set(FName(TEXT("ClassThumbnail.AbilityEffect")),
		                               new FSlateImageBrush(ThumbnailFile, FVector2D(64.f, 64.f)));

		// ClassIcon.MyActor에서 MyActor 부분에 원하시는 클래스명을 넣으시면 됩니다.
		FString IconFile = FPaths::ProjectDir() / TEXT("IconResource") / TEXT("AbilityEffect.png");
		abilityEffectIconInstance->Set(FName(TEXT("ClassIcon.AbilityEffect")),
		                               new FSlateImageBrush(IconFile, FVector2D(16.f, 16.f)));

		FSlateStyleRegistry::RegisterSlateStyle(*abilityEffectIconInstance.Get());
	}
}

void FSoulLikeEditorModule::SetAbilityRequirementIcon()
{
	if (abilityRequirementIconInstance.IsValid() == false)
	{
		abilityRequirementIconInstance = MakeShareable(new FSlateStyleSet("AbilityRequirementIcon"));

		FString ThumbnailFile = FPaths::ProjectDir() / TEXT("IconResource") / TEXT("AbilityRequirement.png");
		// ClassThumbnail.MyActor에서 MyActor 부분에 원하시는 클래스명을 넣으시면 됩니다.
		abilityRequirementIconInstance->Set(FName(TEXT("ClassThumbnail.AbilityRequirement")),
		                                    new FSlateImageBrush(ThumbnailFile, FVector2D(64.f, 64.f)));

		// ClassIcon.MyActor에서 MyActor 부분에 원하시는 클래스명을 넣으시면 됩니다.
		FString IconFile = FPaths::ProjectDir() / TEXT("IconResource") / TEXT("AbilityRequirement.png");
		abilityRequirementIconInstance->Set(FName(TEXT("ClassIcon.AbilityRequirement")),
		                                    new FSlateImageBrush(IconFile, FVector2D(16.f, 16.f)));

		FSlateStyleRegistry::RegisterSlateStyle(*abilityRequirementIconInstance.Get());
	}
}

void FSoulLikeEditorModule::SetCueIcon()
{
	if (abilityCueIconInstance.IsValid() == false)
	{
		abilityCueIconInstance = MakeShareable(new FSlateStyleSet("AbilityCueIcon"));

		FString ThumbnailFile = FPaths::ProjectDir() / TEXT("IconResource") / TEXT("AbilityCue.png");
		// ClassThumbnail.MyActor에서 MyActor 부분에 원하시는 클래스명을 넣으시면 됩니다.
		abilityCueIconInstance->Set(FName(TEXT("ClassThumbnail.AbilityCue")),
		                            new FSlateImageBrush(ThumbnailFile, FVector2D(64.f, 64.f)));

		// ClassIcon.MyActor에서 MyActor 부분에 원하시는 클래스명을 넣으시면 됩니다.
		FString IconFile = FPaths::ProjectDir() / TEXT("IconResource") / TEXT("AbilityCue.png");
		abilityCueIconInstance->Set(FName(TEXT("ClassIcon.AbilityCue")),
		                            new FSlateImageBrush(IconFile, FVector2D(16.f, 16.f)));

		FSlateStyleRegistry::RegisterSlateStyle(*abilityCueIconInstance.Get());
	}
}

void FSoulLikeEditorModule::SetItemIcon()
{
	if (itemIconInstance.IsValid() == false)
	{
		itemIconInstance = MakeShareable(new FSlateStyleSet("ItemIcon"));

		FString ThumbnailFile = FPaths::ProjectDir() / TEXT("IconResource") / TEXT("Item.png");
		// ClassThumbnail.MyActor에서 MyActor 부분에 원하시는 클래스명을 넣으시면 됩니다.
		itemIconInstance->Set(FName(TEXT("ClassThumbnail.ItemActor")),
		                      new FSlateImageBrush(ThumbnailFile, FVector2D(64.f, 64.f)));

		// ClassIcon.MyActor에서 MyActor 부분에 원하시는 클래스명을 넣으시면 됩니다.
		FString IconFile = FPaths::ProjectDir() / TEXT("IconResource") / TEXT("Item.png");
		itemIconInstance->Set(FName(TEXT("ClassIcon.ItemActor")),
		                      new FSlateImageBrush(IconFile, FVector2D(16.f, 16.f)));

		FSlateStyleRegistry::RegisterSlateStyle(*itemIconInstance.Get());
	}
}

void FSoulLikeEditorModule::RegisterCreateAbilityRequirementMenuButton()
{
	/*auto& Tool = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	const auto Category = Tool.RegisterAdvancedAssetCategory(
		TEXT("AbilityRequirment"), FText::FromString(TEXT("어빌리티 요구사항")));

	Tool.RegisterAssetTypeActions(MakeShareable(new FAbilityRequirementAssetActions(Category)));*/
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSoulLikeEditorModule, SoulLikeEditor)
