#include "UT4WebAdmin.h"
#include "Base64.h"


#define UT4WA_PLUGIN_FOLDER "UT4WebAdmin"
#define UT4WA_HTML_FOLDER "www"
#define LISTENING_PORT "8080"

int
WelcomeHandler(struct mg_connection *conn, void *cbdata)
{
	mg_printf(conn,
		"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: "
		"close\r\n\r\n");
	mg_printf(conn, "<html><body>");
	mg_printf(conn, "<h2>Welcome to UT4 Web Admin !</h2>");
	mg_printf(conn, "</body></html>\n");

	return 1;
}

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

	if (Port == 0)
	{
		Port = 8080;
	}

	// Set the document root
	FString DocumentRoot = FPaths::GamePluginsDir() / UT4WA_PLUGIN_FOLDER / UT4WA_HTML_FOLDER;

	// Set listening port 
	FString PortStr = FString::FromInt(Port);

	#if defined(USE_CIVETWEB)
		StartCivetWeb(DocumentRoot, PortStr);
	#endif
}

#if defined(USE_CIVETWEB)
void UUT4WebAdmin::StartCivetWeb(FString &DocumentRoot, FString &PortStr)
{
	/* Initialize the library */
	mg_init_library(0);

	const char *options[] = {
		"document_root", TCHAR_TO_ANSI(*DocumentRoot),
		"listening_ports", TCHAR_TO_ANSI(*PortStr),
		0
	};

	/* Start the server */
	ctx = mg_start(NULL, 0, options);

	if (ctx) {
		UE_LOG(UT4WebAdmin, Log, TEXT("=================="));
		UE_LOG(UT4WebAdmin, Log, TEXT("UT4WebAdmin Started:"));
		UE_LOG(UT4WebAdmin, Log, TEXT(" * Port           : %i"), Port);
		UE_LOG(UT4WebAdmin, Log, TEXT(" * Root Web Folder: %s"), *DocumentRoot);
		UE_LOG(UT4WebAdmin, Log, TEXT("=================="));

		/* Add some handler */
		mg_set_request_handler(ctx, "/", WelcomeHandler, "Hello world");
	}
	else {
		UE_LOG(UT4WebAdmin, Log, TEXT("=================="));
		UE_LOG(UT4WebAdmin, Log, TEXT("UT4WebAdmin could not be started"));
		UE_LOG(UT4WebAdmin, Log, TEXT("=================="));
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
