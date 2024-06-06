// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SoulLikeTarget : TargetRules
{
	public SoulLikeTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("SoulLike");
		
		bLegacyParentIncludePaths = false;
		CppStandard = CppStandardVersion.Default;
		WindowsPlatform.bStrictConformanceMode = true;
	}
} 