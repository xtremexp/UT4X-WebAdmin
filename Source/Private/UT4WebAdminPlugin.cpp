#include "UT4WebAdmin.h"

#include "Core.h"
#include "Engine.h"
#include "ModuleManager.h"
#include "ModuleInterface.h"

class FUT4WebAdminPlugin : public IModuleInterface
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	UUT4WebAdmin* _WebAdmin;
};

IMPLEMENT_MODULE(FUT4WebAdminPlugin, UT4WebAdmin)

void FUT4WebAdminPlugin::StartupModule()
{
	_WebAdmin = NewObject<UUT4WebAdmin>();
	_WebAdmin->Init();
}


void FUT4WebAdminPlugin::ShutdownModule()
{
	_WebAdmin->Stop();
}



