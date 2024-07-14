// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SoulLike : ModuleRules
{
	public SoulLike(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new[] { "GFur", "MoviePlayer" });

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
			"JsonUtilities", "NiagaraAnimNotifies", "NiagaraAnimNotifies"/*,"AsyncLoadingScreen"*/,"DialoguePlugin",
			"SkeletalMerging",
			"RHI"
		});
		
		//PrivateDependencyModuleNames.AddRange(new string[] { "AIModule"});
		
		
		/*if ((Target.IsInPlatformGroup(UnrealPlatformGroup.Windows)))
		{
			// Uses DXGI to query GPU hardware
			// This is what will allow us to get GPU usage statistics at runtime
			PublicSystemLibraries.Add("DXGI.lib");
		}*/
	}
}