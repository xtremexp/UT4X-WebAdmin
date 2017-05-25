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
	
	// http server start at tick
	// because GWorld is NULL at this stage
	// so won't be able to get gamemode and set properly 
	// http port if it's a lobby instance server
}


void UUT4WebAdmin::Stop()
{
	if (_HttpServer) {
		_HttpServer->Stop();
	}
}

void UUT4WebAdmin::Tick(float DeltaTime)
{
	if (_HttpServer) {
		_HttpServer->Tick(DeltaTime);
	}
	// waiting until GWorld loads so we can load properly HttpServer
	else if (GWorld != NULL) {
		_HttpServer = NewObject<UUT4WebAdminHttpServer>();

		if (_HttpServer) {
			_HttpServer->Start();
		}
	}
}

TStatId UUT4WebAdmin::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UUT4WebAdmin, STATGROUP_Tickables);
}



