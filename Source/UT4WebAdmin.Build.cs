
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
                    "OpenSSL",
                    "libWebSockets"
                }
			);

            
            var UT4_WEBADMIN_SRC_PATH = "../../UnrealTournament/Plugins/UT4WebAdmin/Source";
            var LIB_WEBSOCKETS_PATH = UT4_WEBADMIN_SRC_PATH + "/ThirdParty/LibWebSockets";
            PublicIncludePaths.Add(LIB_WEBSOCKETS_PATH + "/include");
        }
	}
}