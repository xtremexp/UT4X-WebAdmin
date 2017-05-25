#include "UT4WebAdmin.h"
#include "UT4WebAdminHttpServer.h"

UUT4WebAdmin::UUT4WebAdmin(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	_HttpServer = nullptr;
	GameMode = nullptr;
}


void UUT4WebAdmin::Init()
{
	// Don't garbage collect me
	SetFlags(RF_MarkAsRootSet);

	_HttpServer = NewObject<UUT4WebAdminHttpServer>();
	
	if (_HttpServer) {
		_HttpServer.Start();
	}
}


void UUT4WebAdmin::Stop()
{
	if (NULL != _HttpServer) {
		_HttpServer.Stop();
	}
}

void UUT4WebAdmin::Tick(float DeltaTime)
{
	if (NULL != _HttpServer) {
		_HttpServer.Tick();
	}
}

TStatId UUT4WebAdmin::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UUT4WebAdmin, STATGROUP_Tickables);
}



