// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RetroTest : ModuleRules
{
	public RetroTest(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"AIModule",
			"Slate",
			"SlateCore",
			"UMG",
			"DeveloperSettings",
		});
	}
}
