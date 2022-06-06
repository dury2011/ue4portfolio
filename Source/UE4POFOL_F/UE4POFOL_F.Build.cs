using UnrealBuildTool;

public class UE4POFOL_F : ModuleRules
{
    public UE4POFOL_F(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore", 
            "Niagara",
            "GameplayTasks",
            "NavigationSystem",
            "AIModule"
        });

        PublicIncludePaths.Add(ModuleDirectory);
    }
}
