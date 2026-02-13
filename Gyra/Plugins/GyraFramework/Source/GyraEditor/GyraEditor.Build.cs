// Copyright 2025 Huagang Liu. All Rights Reserved.

using UnrealBuildTool;

public class GyraEditor : ModuleRules
{
	public GyraEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"$(ModuleDir)/Public",
				"$(ModuleDir)/Public/AssetType",				
				"$(ModuleDir)/Public/Factory",			
				"$(ModuleDir)/Public/Inventory",						
				"$(ModuleDir)/Public/Style",			
				"$(ModuleDir)/Public/Validation",			
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"GyraEditor/Public",
				"GyraEditor/Public/AssetType",				
				"GyraEditor/Public/Factory",			
				"GyraEditor/Public/Inventory",						
				"GyraEditor/Public/Style",			
				"GyraEditor/Public/Validation",				
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"EditorFramework",
				"UnrealEd",
				"PhysicsCore",
				"GameplayTagsEditor",
				"GameplayTasksEditor",
				"GameplayAbilities",
				"GameplayAbilitiesEditor",
				"StudioTelemetry",
				"GyraGame",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"AssetTools",
				"EditorScriptingUtilities",
				"Blutility",
				"InputCore",
				"Slate",
				"SlateCore",
				"ToolMenus",
				"EditorStyle",
				"DataValidation",
				"MessageLog",
				"Projects",
				"DeveloperToolSettings",
				"CollectionManager",
				"SourceControl",
				"Chaos",
				"UMG"
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
