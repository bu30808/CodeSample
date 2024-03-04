// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SoulLikeEditorTarget : TargetRules
{
	public SoulLikeEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("SoulLike");

		RegisterModulesCreatedByRider();
	}

	private void RegisterModulesCreatedByRider()
	{
		ExtraModuleNames.AddRange(new[] { "SoulLikeEditor" });
	}
}