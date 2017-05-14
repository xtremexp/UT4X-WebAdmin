#include "UT4WebAdmin.h"
#include "Base64.h"


#define UT4WA_PLUGIN_FOLDER "UT4WebAdmin"
#define UT4WA_ROOT_FOLDER "www"


UUT4WebAdmin::UUT4WebAdmin(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	#if defined(USE_CIVETWEB)
		ctx = nullptr;
	#endif
	GameMode = nullptr;
}


void UUT4WebAdmin::Init()
{
	// Don't garbage collect me
	SetFlags(RF_MarkAsRootSet);

	if (WebHttpPort == 0)
	{
		WebHttpPort = 8080;
	}

	if (WebHttpsPort == 0)
	{
		WebHttpsPort = 443;
	}

	FString WebRootFolder = FPaths::GamePluginsDir() / UT4WA_PLUGIN_FOLDER / UT4WA_ROOT_FOLDER;

	#if defined(USE_CIVETWEB)
		StartCivetWeb();
	#endif
}

#if defined(USE_CIVETWEB)
void UUT4WebAdmin::StartCivetWeb()
{
	// Set listening port(s) 80,443
	FString WebPortsStr = FString::FromInt(WebHttpPort);

	if (WebHttpsEnabled) {
		WebPortsStr += "," + FString::FromInt(WebHttpPort) + "s";
	}

	/* Initialize the library */
	mg_init_library(0);

	// TODO handle ssl properties
	const char *options[] = {
		"document_root", "../../../UnrealTournament/Plugins/UT4WebAdmin/www", // Note TCHAR_TO_ANSI(*WebRootFolder) would not make root folder working !
		"listening_ports", TCHAR_TO_ANSI(*WebPortsStr),
		NULL
	};

	/* Start the server */
	//struct handler = { NULL };
	ctx = mg_start(NULL, 0, options);

	if (ctx) {
		UE_LOG(UT4WebAdmin, Log, TEXT("=================="));
		UE_LOG(UT4WebAdmin, Log, TEXT("UT4WebAdmin Started at :"));
		UE_LOG(UT4WebAdmin, Log, TEXT(" * Port(s)        : %s"), *WebPortsStr);
		UE_LOG(UT4WebAdmin, Log, TEXT(" * Root Folder    : ../../../UnrealTournament/Plugins/UT4WebAdmin/www"));
		UE_LOG(UT4WebAdmin, Log, TEXT("=================="));

		/* Add some handler */
		//mg_set_request_handler(ctx, "/admin", WelcomeHandler, "Web Administration");
	}
	else {
		UE_LOG(UT4WebAdmin, Warning, TEXT("=================="));
		UE_LOG(UT4WebAdmin, Warning, TEXT("UT4WebAdmin could not be started"));
		UE_LOG(UT4WebAdmin, Warning, TEXT("=================="));
	}
}

void UUT4WebAdmin::StopCivetWeb()
{
	if (ctx) {
		/* Stop the server */
		mg_stop(ctx);

		UE_LOG(UT4WebAdmin, Log, TEXT("UT4WebAdmin Stopped"));

		/* Un-initialize the library */
		mg_exit_library();
	}
}
#endif

void UUT4WebAdmin::Stop()
{
	#if defined(USE_CIVETWEB)
		StopCivetWeb();
	#endif
}

void UUT4WebAdmin::Tick(float DeltaTime)
{
	//poll(MGServer, 1);
	// TODO
}

TStatId UUT4WebAdmin::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UUT4WebAdmin, STATGROUP_Tickables);
}