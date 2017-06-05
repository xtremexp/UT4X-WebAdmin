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
	
	DisplayName = FText::FromString("UT4 WebAdmin 0.0.1a");
	Author = FText::FromString("Thomas 'XtremeXp/Winter' P.");
	Description = FText::FromString("Web administration mutator for Unreal Tournament 4 (2017) ");

	_HttpServer = NewObject<UUT4WebAdminHttpServer>();
}


void AUT4WebAdmin::Stop()
{
	if (_HttpServer) {
		_HttpServer->Stop();
	}
}



