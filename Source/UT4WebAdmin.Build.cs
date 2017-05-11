
namespace UnrealBuildTool.Rules
{
	public class UT4WebAdmin : ModuleRules
	{
		public UT4WebAdmin(TargetInfo Target)
        {
            PrivateIncludePaths.Add("UT4WebAdmin/Private");
            PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
                    "Engine",
                    "UnrealTournament",
					"Json", 
					"JsonUtilities"
				}
				);
		}
	}
}