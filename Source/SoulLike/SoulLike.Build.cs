// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SoulLike : ModuleRules
{
	public SoulLike(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "GFur", "MoviePlayer" });
		
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "UMG",
			"Slate",
			"SlateCore",
			"GameplayTags",
			"GameplayTasks",
			"Niagara",
			"AnimGraphRuntime",
			"AIModule",
			"PhysicsCore",
			"NavigationSystem",
			"NiagaraUIRenderer",
			"Json",
			"JsonUtilities", "NiagaraAnimNotifies", "NiagaraAnimNotifies"
		});
	}
}