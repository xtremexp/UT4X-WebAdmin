#include "UT4WebAdmin.h"
#include "Base64.h"

#define UT4WA_PLUGIN_FOLDER "UT4WebAdmin"
#define UT4WA_WWW_FOLDER "www"

#define PAGE "<html><head><title>File not found</title></head><body>File not found</body></html>"


UUT4WebAdmin::UUT4WebAdmin(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	daemon = nullptr;
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


	StartMicroHttp();
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
		JsonObject->SetBoolField(TEXT("bPrivateMatch"), BaseGameMode->bPrivateMatch);
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


// Method to serve json file to client
int serve_json_file(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data,
	size_t *upload_data_size, void **con_cls, TSharedPtr<FJsonObject> json)
{
	int ret;
	struct MHD_Response *response;

	FString JsonText;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JsonText);
	FJsonSerializer::Serialize(json.ToSharedRef(), Writer);

	const char* jsonChar = TCHAR_TO_UTF8(*JsonText);

	response = MHD_create_response_from_buffer(strlen(jsonChar),
		(void*)jsonChar, MHD_RESPMEM_PERSISTENT);
	MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, "application/json");

	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);

	return ret;
}

int handle_game_info(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data,
	size_t *upload_data_size, void **con_cls)
{
	if (strcmp(method, MHD_HTTP_METHOD_GET) == 0) {
		TSharedPtr<FJsonObject> matchInfoJson = GetGameInfoJSON();
		return serve_json_file(cls, connection, url, method, version, upload_data, upload_data_size, con_cls, matchInfoJson);
	}
	else {
		// TODO handle post request for kick/ban/modify server info ...
	}

	return MHD_NO;
}

// Server files from root folder
const FString wwwStr = FPaths::GamePluginsDir() + UT4WA_PLUGIN_FOLDER + "/" + UT4WA_WWW_FOLDER + "/";


int handle_serve_file(void *cls,
	struct MHD_Connection *connection,
	const char *url,
	const char *method, // GET / POST / PUT ...
	const char *version, // HTML version 1.1
	const char *upload_data,
	size_t *upload_data_size, void **con_cls)
{
	struct MHD_Response *response;
	char *path = NULL;
	int ret;

	// redirect from http://myserver:port/ to http://myserver:port/index.html
	if (strcmp(url, "/") == 0) {
		path = "index.html";
	}

	if (NULL == path) {
		path = new char[strlen(&url[1]) + 1];
		strcpy(path, &url[1]);
	}

	const char *www = TCHAR_TO_ANSI(*wwwStr);

	// calculate the required buffer size (also accounting for the null terminator):
	int bufferSize = strlen(www) + strlen(path) + 1;

	// allocate enough memory for the concatenated string:
	char* concatString = new char[bufferSize];

	// copy strings one and two over to the new buffer:
	strcpy(concatString, www);
	strcat(concatString, path);


	FILE* f;
	f = fopen(concatString, "rb");

	if (f != NULL) {
		// Determine file size
		fseek(f, 0, SEEK_END);
		size_t size = ftell(f);
		char* where = new char[size];
		rewind(f);
		fread(where, sizeof(char), size, f);

		response = MHD_create_response_from_buffer(strlen(where),
			(void*)where, MHD_RESPMEM_PERSISTENT);

		fclose(f);
		ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	}
	else {
		const char *notExist = "<html><body>File not found !</body></html>";
		response = MHD_create_response_from_buffer(strlen(notExist),
			(void*)notExist, MHD_RESPMEM_PERSISTENT);
		ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
	}

	MHD_destroy_response(response);
	return ret;
}

int answer_to_connection(void *cls, 
	struct MHD_Connection *connection,
	const char *url,
	const char *method, // GET / POST / PUT ...
	const char *version, // HTML version 1.1
	const char *upload_data,
	size_t *upload_data_size, void **con_cls)
{
	// TODO handle POST methods in the future
	if ((0 != strcmp(method, MHD_HTTP_METHOD_GET)) &&
		(0 != strcmp(method, MHD_HTTP_METHOD_HEAD)))
		return MHD_NO;

	// TODO check user authentification

	if (strcmp(url, "/gameinfo") == 0) {
		// TODO GET JSON GAME INFO
		return handle_game_info(cls, connection, url, method, version, upload_data, upload_data_size, con_cls);
	}
	else {
		return handle_serve_file(cls, connection, url, method, version, upload_data, upload_data_size, con_cls);
	}
}

void UUT4WebAdmin::StartMicroHttp()
{
	// SSL not working yet need some more investigation
	if (WebHttpsEnabled) {

		if (NULL == *WebServerCertificateFile || NULL == *WebServerKeyFile) {
			UE_LOG(UT4WebAdmin, Warning, TEXT("Server key or certificate file is not set."));
			return;
		}

		// openssl req -days 365 -out server.pem -new -x509 -key server.key
		FILE* fcert_file = fopen(TCHAR_TO_ANSI(*WebServerCertificateFile), "r");

		// openssl genrsa -out server.key 1024
		FILE* fkey_file = fopen(TCHAR_TO_ANSI(*WebServerKeyFile), "r");

		if (NULL != fcert_file && NULL != fkey_file) {

			fseek(fcert_file, 0, SEEK_END);
			size_t size = ftell(fcert_file);
			char* cert_pem = new char[size];
			rewind(fcert_file);
			fread(cert_pem, sizeof(char), size, fcert_file);


			fseek(fkey_file, 0, SEEK_END);
			size_t size2 = ftell(fkey_file);
			char* key_pem = new char[size2];
			rewind(fkey_file);
			fread(key_pem, sizeof(char), size2, fkey_file);

			daemon = MHD_start_daemon(MHD_USE_AUTO | MHD_USE_SELECT_INTERNALLY | MHD_USE_TLS | MHD_USE_DEBUG, WebHttpPort, NULL, NULL,
				&answer_to_connection, NULL,
				MHD_OPTION_HTTPS_MEM_KEY, key_pem,
				MHD_OPTION_HTTPS_MEM_CERT, cert_pem,
				MHD_OPTION_END);
		}
		

		if (NULL != fcert_file) {
			fclose(fcert_file);
		}

		if (NULL != fkey_file) {
			fclose(fkey_file);
		}
	}
	else {
		UE_LOG(UT4WebAdmin, Log, TEXT("Starting HTTP server without SSL"));
		daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG, WebHttpPort, NULL, NULL,
			&answer_to_connection, NULL, MHD_OPTION_END);
	}
	

	if (daemon == NULL) {
		UE_LOG(UT4WebAdmin, Warning, TEXT(" * * * * * * * * * * * * * * * * * * * * * * *"));
		UE_LOG(UT4WebAdmin, Warning, TEXT(" UT4WebAdmin failed to start http(s) server !"));
		UE_LOG(UT4WebAdmin, Warning, TEXT(" * * * * * * * * * * * * * * * * * * * * * * *"));
	}
	else {
		UE_LOG(UT4WebAdmin, Log, TEXT(" * * * * * * * * * * * * * * * * * * * * * * *"));
		UE_LOG(UT4WebAdmin, Log, TEXT(" UT4WebAdmin started at port: %i "), WebHttpPort);
		UE_LOG(UT4WebAdmin, Log, TEXT(" * * * * * * * * * * * * * * * * * * * * * * *"));
	}
}



void UUT4WebAdmin::Stop()
{
	if (NULL != daemon) {
		MHD_stop_daemon(daemon);
	}
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



