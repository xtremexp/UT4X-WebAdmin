#include "HTTPAdmin.h"

#include "Core.h"
#include "Engine.h"
#include "ModuleManager.h"
#include "ModuleInterface.h"

class FUT4WebAdminPlugin : public IModuleInterface
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FUT4WebAdminPlugin, HTTPAdmin)

void FUT4WebAdminPlugin::StartupModule()
{
		UUT4WebAdmin* UT4WebAdmin = Cast<UUT4WebAdmin>(StaticConstructObject(UUT4WebAdmin::StaticClass()));
		UT4WebAdmin->Init();

}


void FHTTPAdminPlugin::ShutdownModule()
{

}



