
namespace UnrealBuildTool.Rules
{
	public class UT4WebAdmin : ModuleRules
	{
		public UT4WebAdmin(TargetInfo Target)
        {
            PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
                    "Engine",
                    "UnrealTournament",
					"PakFile",
					"Json", 
					"JsonUtilities",
                    "WebSockets",
                    "libWebSockets"
                }
			);
        }
	}
}