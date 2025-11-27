// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectRedC : ModuleRules
{
	public ProjectRedC(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(new string[]
		{
			"ProjectRedC"
		});
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine",						// Engine Core
			"AnimGraphRuntime",										// Animation
			"InputCore", "EnhancedInput",							// Input
			"UMG", "Slate", "SlateCore","ModelViewViewModel",		// Widget
			"GameplayAbilities", "GameplayTags", "GameplayTasks",	// GAS
			"AIModule", "NavigationSystem",							// AI
			"Sockets",  "NetCore",									// Networking
			"MotionWarping", "ContextualAnimation",					// Motion Warping
			"Niagara"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
