using UnrealBuildTool;

public class SoulLikeEditor : ModuleRules
{
	public SoulLikeEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core", "AssetTools", "UMG", "UMGEditor", "GameplayTags", "ToolMenus", "ToolMenusEditor", "SoulLike",
				"Blutility"
				// "Core", "AssetTools", "Blutility", "UMG", "UMGEditor", "UnrealEd","LevelEditor"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"SoulLike", "EditorScriptingUtilities", "Blutility","ScriptableEditorWidgets"
			}
		);
	}
}