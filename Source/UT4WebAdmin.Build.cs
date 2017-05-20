
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
					"JsonUtilities"
				}
			);

            var UT4_WEBADMIN_SRC_PATH = "../../UnrealTournament/Plugins/UT4WebAdmin/Source";
            var LIB_MICROHTTPD_PATH = UT4_WEBADMIN_SRC_PATH + "/ThirdParty/Libmicrohttpd";

            PublicIncludePaths.Add(LIB_MICROHTTPD_PATH + "/include");

            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                PublicLibraryPaths.Add(LIB_MICROHTTPD_PATH + "/lib/x64/Windows");
                PublicAdditionalLibraries.Add(LIB_MICROHTTPD_PATH + "/lib/x64/Windows/libmicrohttpd-dll.lib");
            }
            else if (Target.Platform == UnrealTargetPlatform.Linux)
            {
                PublicLibraryPaths.Add(LIB_MICROHTTPD_PATH + "/lib/x64/Linux");
                // not working atm compiler can't link the file
                PublicAdditionalLibraries.Add(LIB_MICROHTTPD_PATH + "/lib/x64/Linux/libmicrohttpd.a");
            }
        }
	}
}