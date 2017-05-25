#include "UT4WebAdmin.h"
#include "UT4WebAdminHttpServer.h"

UUT4WebAdmin::UUT4WebAdmin(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	_HttpServer = nullptr;
	GameMode = nullptr;
}


void UUT4WebAdmin::Start()
{
	// Don't garbage collect me
	SetFlags(RF_MarkAsRootSet);
	
	_HttpServer = NewObject<UUT4WebAdminHttpServer>();
}


void UUT4WebAdmin::Stop()
{
	if (_HttpServer) {
		_HttpServer->Stop();
	}
}



