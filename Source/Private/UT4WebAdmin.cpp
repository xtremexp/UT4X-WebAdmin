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
	/*
	if (WebHttpPort == 0)
	{
		WebHttpPort = 8080;
	}

	if (WebHttpsPort == 0)
	{
		WebHttpsPort = 443;
	}
	*/

	FString WebRootFolder = FPaths::GamePluginsDir() / UT4WA_PLUGIN_FOLDER / UT4WA_ROOT_FOLDER;

	#if defined(USE_CIVETWEB)
		StartCivetWeb();
	#endif
}

TSharedPtr<FJsonObject> GetGameInfoJSON()
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);;

	// Get a reference of any object using the UTBaseGameMode
	AUTBaseGameMode* BaseGameMode;
	BaseGameMode = Cast<AUTBaseGameMode>(GWorld->GetAuthGameMode());

	if (BaseGameMode) {

		// GameMode
		JsonObject->SetBoolField(TEXT("IsMatchInProgress"), BaseGameMode->IsMatchInProgress());
		JsonObject->SetBoolField(TEXT("HasMatchEnded"), BaseGameMode->HasMatchEnded());
		JsonObject->SetNumberField(TEXT("NumTravellingPlayers"), BaseGameMode->NumTravellingPlayers);
		JsonObject->SetStringField(TEXT("NetworkNumber"), BaseGameMode->GetNetworkNumber());
		JsonObject->SetStringField(TEXT("MatchState"), BaseGameMode->GetMatchState().ToString());
		
		// UTBaseGameMode
		JsonObject->SetStringField(TEXT("ServerInstanceID"), BaseGameMode->ServerInstanceID);
		JsonObject->SetStringField(TEXT("ServerInstanceGUID"), BaseGameMode->ServerInstanceGUID.ToString()); // The Unique ID for this game instance.
		JsonObject->SetStringField(TEXT("ContextGUID"), BaseGameMode->ContextGUID.ToString()); // ?
		JsonObject->SetStringField(TEXT("ServerPassword"), BaseGameMode->ServerPassword);
		JsonObject->SetStringField(TEXT("SpectatePassword"), BaseGameMode->SpectatePassword);
		JsonObject->SetBoolField(TEXT("bRequirePassword"), BaseGameMode->bRequirePassword);
		JsonObject->SetStringField(TEXT("DisplayName"), BaseGameMode->DisplayName.ToString());
		JsonObject->SetNumberField(TEXT("MinAllowedRank"), BaseGameMode->MinAllowedRank);
		JsonObject->SetNumberField(TEXT("MaxAllowedRank"), BaseGameMode->MaxAllowedRank);
		JsonObject->SetBoolField(TEXT("bTrainingGround"), BaseGameMode->bTrainingGround);
		JsonObject->SetNumberField(TEXT("NumPlayers"), BaseGameMode->GetNumPlayers());
		JsonObject->SetNumberField(TEXT("NumMatches"), BaseGameMode->GetNumMatches()); // 1 if dedi server else [0, .., X] range for hubs/lobbies
		JsonObject->SetNumberField(TEXT("CurrentPlaylistId"), BaseGameMode->CurrentPlaylistId); // no idea what this is about
		JsonObject->SetBoolField(TEXT("CurrentPlaylistId"), BaseGameMode->bPrivateMatch);
		JsonObject->SetStringField(TEXT("RankedLeagueName"), BaseGameMode->GetRankedLeagueName()); // always empty for the moment
		JsonObject->SetBoolField(TEXT("SupportsInstantReplay"), BaseGameMode->SupportsInstantReplay());
		JsonObject->SetBoolField(TEXT("bIsLANGame"), BaseGameMode->bIsLANGame);

		if (BaseGameMode->IsLobbyServer())
		{
			AUTLobbyGameMode* LobbyGameMode;
			LobbyGameMode = GWorld->GetAuthGameMode<AUTLobbyGameMode>();

			if (LobbyGameMode)
			{
				JsonObject->SetStringField(TEXT("ServType"), TEXT("dedis"));
				// TODO get instance data
			}
		}
	}


	return JsonObject;
}

int GameInfoHandler(struct mg_connection *conn, void *cbdata)
{

	//UE_LOG(UT4WebAdmin, Log, TEXT(" MatchInfoHandler "));
	const struct mg_request_info *request_info = mg_get_request_info(conn);

	if (strcmp(request_info->request_method, "GET") == 0) {

		FString JsonText = "";

		mg_printf(conn,
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: application/json\r\n"
			"Connection: close\r\n\r\n");

		TSharedPtr<FJsonObject> matchInfoJSON = GetGameInfoJSON();

		FString OutputString;
		TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(matchInfoJSON.ToSharedRef(), Writer);

		JsonText += OutputString;

		mg_printf(conn, TCHAR_TO_ANSI(*JsonText));

		return 1;
	}

	

	return 0;
}

#if defined(USE_CIVETWEB)
void UUT4WebAdmin::StartCivetWeb()
{
	// Set listening port(s) 80,443
	//FString WebPortsStr = FString::FromInt(WebHttpPort);

	// WebServer error log file - TODO move config ?
	FString WebErrorLogFile = "../../../UnrealTournament/Plugins/UT4WebAdmin/webserver-error.log";

	/* Initialize the library */
	mg_init_library(0);

	const char *docRoot = "../../../UnrealTournament/Plugins/UT4WebAdmin/www";
	const char *errLogFile = "../../../UnrealTournament/Plugins/UT4WebAdmin/webserver-error.log";
	FString sslFileStr = FString("../../../UnrealTournament/Plugins/UT4WebAdmin/ssl_certificate.pem");
	const char *sslFile = "../../../UnrealTournament/Plugins/UT4WebAdmin/ssl_certificate.pem";
	FString WebPortsStr;

	if (WebHttpsEnabled) {
		WebPortsStr = "8090, 443s";
		if (!FPaths::FileExists(*sslFileStr)) {
			UE_LOG(UT4WebAdmin, Warning, TEXT(" Https disabled. Certificate file %s does not exists!"), *sslFileStr);
		}
	}
	else {
		WebPortsStr = "8090";
	}

	
	// FIXME passing FString config properties to options does not work ! ...
	const char *options_with_ssl[]  = {
		"document_root", docRoot,
		"listening_ports", "8090, 443s",
		"error_log_file", errLogFile,
		"ssl_certificate", sslFile,
		NULL
	};

	const char *options[] = {
		"document_root", docRoot,
		"listening_ports", "8090",
		"error_log_file", errLogFile,
		NULL
	};

	/* Start the server */

	if (WebHttpsEnabled) {
		ctx = mg_start(NULL, 0, options_with_ssl);
	}
	else {
		ctx = mg_start(NULL, 0, options);
	}

	if (ctx) {
		UE_LOG(UT4WebAdmin, Log, TEXT("=================="));
		UE_LOG(UT4WebAdmin, Log, TEXT("UT4WebAdmin Started at :"));
		UE_LOG(UT4WebAdmin, Log, TEXT(" * Port(s)        : %s"), *WebPortsStr);
		UE_LOG(UT4WebAdmin, Log, TEXT(" * Root Folder    : ../../../UnrealTournament/Plugins/UT4WebAdmin/www"));
		UE_LOG(UT4WebAdmin, Log, TEXT("=================="));

		/* Add some handler */
		mg_set_request_handler(ctx, "/gameinfo", GameInfoHandler, "Web Administration - Match Info");
	}
	else {
		UE_LOG(UT4WebAdmin, Warning, TEXT("=================="));
		UE_LOG(UT4WebAdmin, Warning, TEXT("UT4WebAdmin could not be started"));
		UE_LOG(UT4WebAdmin, Warning, TEXT(" * Port(s)        : %s"), *WebPortsStr);
		if (WebHttpsEnabled) {
			UE_LOG(UT4WebAdmin, Warning, TEXT(" * SSL Certificate File        : %s"), *sslFileStr);// *WebSslCertificateFile);
		}
		UE_LOG(UT4WebAdmin, Warning, TEXT("See %s for more details"), *WebErrorLogFile);
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



