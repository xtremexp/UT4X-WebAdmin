#include "UT4WebAdmin.h"
#include "UT4WebAdminHttpServer.h"

AUT4WebAdmin::AUT4WebAdmin(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	_HttpServer = nullptr;
	GameMode = nullptr;
}


void AUT4WebAdmin::Start()
{
	// Don't garbage collect me
	SetFlags(RF_MarkAsRootSet);
	
	_HttpServer = NewObject<UUT4WebAdminHttpServer>();
}


void AUT4WebAdmin::Stop()
{
	if (_HttpServer) {
		_HttpServer->Stop();
	}
}



