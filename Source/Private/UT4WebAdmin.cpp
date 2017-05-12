#include "UT4WebAdmin.h"
#include "Base64.h"

#include "civetweb.h"
#include "CivetServer.h"

#define DOCUMENT_ROOT "."
#define PORT "8080"

UUT4WebAdmin::UUT4WebAdmin(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	//me = nullptr;
	//ctx = nullptr;
	GameMode = nullptr;
}


void UUT4WebAdmin::Init()
{

	// Don't garbage collect me
	SetFlags(RF_MarkAsRootSet);

	// = User + TEXT(":") + Password;

	if (Port == 0)
	{
		Port = 8080;
	}

	//const char *options[] = { "listening_ports", "8080", NULL };
	
	const char *options[] = {
		"document_root", ".", "listening_ports", "8080", 0 };

	std::vector<std::string> cpp_options;
	for (int i = 0; i<(sizeof(options) / sizeof(options[0]) - 1); i++) {
		cpp_options.push_back(options[i]);
	}

	//CivetServer server(options); // <-- C style start
	
	CivetServer server(cpp_options); // <-- C++ style start

	UE_LOG(UT4WebAdmin, Log, TEXT("=================="));
	UE_LOG(UT4WebAdmin, Log, TEXT("UT4WebAdmin Started"));
	UE_LOG(UT4WebAdmin, Log, TEXT("=================="));
}

void UUT4WebAdmin::Stop()
{
	//mg_stop(ctx);
}

void UUT4WebAdmin::Tick(float DeltaTime)
{
	// TODO
}

TStatId UUT4WebAdmin::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UUT4WebAdmin, STATGROUP_Tickables);
}
