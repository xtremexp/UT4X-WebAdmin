#include "UT4WebAdmin.h"
#include "UT4WebAdminGameInfo.h"
#include "UT4WebAdminServerInfo.h"

#define UT4WA_PLUGIN_FOLDER "UT4WebAdmin"
#define UT4WA_WWW_FOLDER "www"

#define PAGE1 "<html><head><title>File not found</title></head><body>File not found</body></html>"
#define DENIED "<html><head><title>Acces denied</title></head><body>Access denied</body></html>"
#define OPAQUE1 "11733b200778ce33060f31c9af70a870ba96ddd4"

UUT4WebAdminHttpServer::UUT4WebAdminHttpServer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BaseGameMode = Cast<AUTBaseGameMode>(GWorld->GetAuthGameMode());

	// don't garbace collect me
	SetFlags(RF_MarkAsRootSet);
}

// Method to serve json file to client
int serve_json_file(struct MHD_Connection *connection, TSharedPtr<FJsonObject> json)
{
	int ret;
	struct MHD_Response *response;

	FString JsonText;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JsonText);
	FJsonSerializer::Serialize(json.ToSharedRef(), Writer);

	std::string tmpStr = TCHAR_TO_ANSI(*JsonText);

	char *jsonChar = new char[tmpStr.length() + 1];
	strcpy(jsonChar, tmpStr.c_str());


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
		return serve_json_file(connection, matchInfoJson);
	}
	else {
		// TODO handle post request for kick/ban/modify server info ...
	}

	return MHD_NO;
}


int handle_server_info(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data,
	size_t *upload_data_size, void **con_cls)
{
	if (strcmp(method, MHD_HTTP_METHOD_GET) == 0) {
		AUTBaseGameMode* BaseGameMode = Cast<AUTBaseGameMode>(GWorld->GetAuthGameMode());
		TSharedPtr<FJsonObject> serverInfoJson = GetServerInfoJSON(BaseGameMode);
		return serve_json_file(connection, serverInfoJson);
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
	int ret;
	struct MHD_Response *response;
	char *username;
	const char *realm = "someusername";
	const char *password = "somepassword";
	username = MHD_digest_auth_get_username(connection);

	// username authentification
	if (NULL == username) {
		response = MHD_create_response_from_buffer(strlen(DENIED), DENIED, MHD_RESPMEM_PERSISTENT);
		ret = MHD_queue_auth_fail_response(connection, realm,
			realm,
			response,
			MHD_NO);
		MHD_destroy_response(response);
		return ret;
	}

	// check username / password OK
	ret = MHD_digest_auth_check(connection, realm,
		username,
		password,
		300);

	FString usernameFStr = ANSI_TO_TCHAR(username);

	// bad username / password return invalid authentification
	if ((ret == MHD_INVALID_NONCE) ||
		(ret == MHD_NO))
	{
		response = MHD_create_response_from_buffer(strlen(DENIED),
			DENIED,
			MHD_RESPMEM_PERSISTENT);
		if (NULL == response)
			return MHD_NO;
		ret = MHD_queue_auth_fail_response(connection, realm,
			OPAQUE1,
			response,
			(ret == MHD_INVALID_NONCE) ? MHD_YES : MHD_NO);
		MHD_destroy_response(response);

		UE_LOG(UT4WebAdmin, Warning, TEXT("User '%s' failed to authenticate."), *usernameFStr);
		return ret;
	}
	else {
		UE_LOG(UT4WebAdmin, Log, TEXT("User '%s' authenticated."), *usernameFStr);
	}

	// USER AUTHENTICATED LET'S GO !

	// TODO handle POST methods in the future
	if ((0 != strcmp(method, MHD_HTTP_METHOD_GET)) &&
		(0 != strcmp(method, MHD_HTTP_METHOD_HEAD)))
		return MHD_NO;

	if (strcmp(url, "/gameinfo") == 0) {
		return handle_game_info(cls, connection, url, method, version, upload_data, upload_data_size, con_cls);
	}
	else if (strcmp(url, "/serverinfo") == 0) {
		return handle_server_info(cls, connection, url, method, version, upload_data, upload_data_size, con_cls);
	}
	else {
		return handle_serve_file(cls, connection, url, method, version, upload_data, upload_data_size, con_cls);
	}
}

void UUT4WebAdminHttpServer::Start()
{
	//AUTBaseGameMode* BaseGameMode;
	//BaseGameMode = Cast<AUTBaseGameMode>(GWorld->GetAuthGameMode());
	bool IsGameInstanceServer = false;

	// TODO if isGameInstance start http server on another port than default
	if (BaseGameMode) {
		IsGameInstanceServer = BaseGameMode->IsGameInstanceServer();
	}

	// SSL not working yet need some more investigation
	if (WebHttpsEnabled && !IsGameInstanceServer) {

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

void UUT4WebAdminHttpServer::Stop()
{
	if (NULL != daemon) {
		MHD_stop_daemon(daemon);
	}
}

void UUT4WebAdminHttpServer::Tick(float DeltaTime)
{
	// TODO
}

TStatId UUT4WebAdminHttpServer::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UUT4WebAdmin, STATGROUP_Tickables);
}