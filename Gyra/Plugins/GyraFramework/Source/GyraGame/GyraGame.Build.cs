// Copyright 2025 Huagang Liu. All Rights Reserved.

using UnrealBuildTool;

public class GyraGame : ModuleRules
{
	public GyraGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
                "$(ModuleDir)/Public",
                "$(ModuleDir)/Public/AbilitySystem",
                "$(ModuleDir)/Public/AbilitySystem/Abilities",
                "$(ModuleDir)/Public/AbilitySystem/Attributes",
                "$(ModuleDir)/Public/AbilitySystem/Executions",
                "$(ModuleDir)/Public/AbilitySystem/Phases",
                "$(ModuleDir)/Public/Animation",
                "$(ModuleDir)/Public/Animation/AnimNotify",
                "$(ModuleDir)/Public/Animation/AnimNotifyState",
                "$(ModuleDir)/Public/Archive",
                "$(ModuleDir)/Public/Audio",
                "$(ModuleDir)/Public/Camera",
                "$(ModuleDir)/Public/Character",   
                "$(ModuleDir)/Public/Cosmetics",
                "$(ModuleDir)/Public/DataTable",
                "$(ModuleDir)/Public/Development",
                "$(ModuleDir)/Public/Equipment",
                "$(ModuleDir)/Public/Feedback",
                "$(ModuleDir)/Public/Feedback/ContextEffects",
                "$(ModuleDir)/Public/Feedback/NumberPops",
                "$(ModuleDir)/Public/GameFeatures",
                "$(ModuleDir)/Public/GameModes",
                "$(ModuleDir)/Public/Input",
                "$(ModuleDir)/Public/Interaction",
                "$(ModuleDir)/Public/Interaction/Abilities",
                "$(ModuleDir)/Public/Interaction/Tasks",
                "$(ModuleDir)/Public/Inventory",
                "$(ModuleDir)/Public/Inventory/Fragments",
                "$(ModuleDir)/Public/Messages",
                "$(ModuleDir)/Public/Messages/MessageProcessors",
                "$(ModuleDir)/Public/Performance",
                "$(ModuleDir)/Public/Physics",
                "$(ModuleDir)/Public/Player",
                "$(ModuleDir)/Public/Settings",
                "$(ModuleDir)/Public/Settings/CustomSettings",
                "$(ModuleDir)/Public/Settings/Screens",
                "$(ModuleDir)/Public/Settings/Widgets",
                "$(ModuleDir)/Public/System",
                "$(ModuleDir)/Public/Teams",
                "$(ModuleDir)/Public/UI",
                "$(ModuleDir)/Public/UI/Basic",
                "$(ModuleDir)/Public/UI/Common",
                "$(ModuleDir)/Public/UI/Foundation",
                "$(ModuleDir)/Public/UI/Frontend",
                "$(ModuleDir)/Public/UI/IndicatorSystem",
                "$(ModuleDir)/Public/UI/PerformanceStats",
                "$(ModuleDir)/Public/UI/Subsystem",
                "$(ModuleDir)/Public/UI/Weapons",
                "$(ModuleDir)/Public/Weapons",
            }
        );



        PrivateIncludePaths.AddRange(
            new string[] {
                "GyraGame/Public",
                "GyraGame/Public/AbilitySystem",
                "GyraGame/Public/AbilitySystem/Abilities",
                "GyraGame/Public/AbilitySystem/Attributes",
                "GyraGame/Public/AbilitySystem/Executions",
                "GyraGame/Public/AbilitySystem/Phases",
                "GyraGame/Public/Animation",
                "GyraGame/Public/Animation/AnimNotify",
                "GyraGame/Public/Animation/AnimNotifyState",
                "GyraGame/Public/Archive",
                "GyraGame/Public/Audio",
                "GyraGame/Public/Camera",
                "GyraGame/Public/Character",
                "GyraGame/Public/Cosmetics",
                "GyraGame/Public/DataTable",
                "GyraGame/Public/Development",
                "GyraGame/Public/Equipment",
                "GyraGame/Public/Feedback",
                "GyraGame/Public/Feedback/ContextEffects",
                "GyraGame/Public/Feedback/NumberPops",
                "GyraGame/Public/GameFeatures",
                "GyraGame/Public/GameModes",
                "GyraGame/Public/Input",
                "GyraGame/Public/Interaction",
                "GyraGame/Public/Interaction/Abilities",
                "GyraGame/Public/Interaction/Tasks",
                "GyraGame/Public/Inventory",
                "GyraGame/Public/Messages",
                "GyraGame/Public/Messages/MessageProcessors",
                "GyraGame/Public/Performance",
                "GyraGame/Public/Physics",
                "GyraGame/Public/Player",
                "GyraGame/Public/Settings",
                "GyraGame/Public/Settings/CustomSettings",
                "GyraGame/Public/Settings/Screens",
                "GyraGame/Public/Settings/Widgets",
                "GyraGame/Public/System",
                "GyraGame/Public/Teams",
                "GyraGame/Public/UI",
                "GyraGame/Public/UI/Basic",
                "GyraGame/Public/UI/Common",
                "GyraGame/Public/UI/Foundation",
                "GyraGame/Public/UI/Frontend",
                "GyraGame/Public/UI/IndicatorSystem",
                "GyraGame/Public/UI/PerformanceStats",
                "GyraGame/Public/UI/Subsystem",
                "GyraGame/Public/UI/Weapons",
                "GyraGame/Public/Weapons",
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreOnline",
                "CoreUObject",
                "ApplicationCore",
                "Engine",
                "PhysicsCore",
                "GameplayTags",
                "GameplayTasks",
                "GameplayAbilities",
                "AIModule",
                "ModularGameplay",
                "ModularGameplayActors",
                "DataRegistry",
                "ReplicationGraph",
                "GameFeatures",
                "SignificanceManager",
                "Hotfix",
                "CommonLoadingScreen",
                "Niagara",
                "AsyncMixin",
                "ControlFlows",
                "PropertyPath",
                "NavigationSystem",
                "CustomizableObject",
                "GameplayMessageRuntime",
                "MeleeTrace"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "InputCore",
                "Slate",
                "SlateCore",
                "RenderCore",
                "DeveloperSettings",
                "EnhancedInput",
                "NetCore",
                "RHI",
                "Projects",
                "Gauntlet",
                "UMG",
                "CommonUI",
                "CommonInput",
                "GameSettings",
                "CommonGame",
                "CommonUser",
                "GameSubtitles",
                "GameplayMessageRuntime",
                "AudioMixer",
                "NetworkReplayStreaming",
                "UIExtension",
                "ClientPilot",
                "AudioModulation",
                "EngineSettings",
                "DTLSHandlerComponent",
            }
        );

        DynamicallyLoadedModuleNames.AddRange(
            new string[] {
                
                
            }
        );

        // Generate compile errors if using DrawDebug functions in test/shipping builds.
        // 生成编译错误,如果使用在测试或者发行的构建中
        PublicDefinitions.Add("SHIPPING_DRAW_DEBUG_ERROR=1");
      
        //ForceIncludeFiles.Add("GyraGame/Public/GyraGameMacro.h");
        
        SetupGameplayDebuggerSupport(Target);
        SetupIrisSupport(Target);

    }
	
}
