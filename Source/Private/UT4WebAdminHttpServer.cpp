#include "UT4WebAdmin.h"
#include "UT4WebAdminGameInfo.h"
#include "UT4WebAdminServerInfo.h"
#include "UT4WebAdminUtils.h"

#define UT4WA_WWW_FOLDER "www"

#define IS_DEBUG 1

#define PAGE1 "<html><head><title>File not found</title></head><body>File not found</body></html>"
#define DENIED "<html><head><title>Acces denied</title></head><body>Access denied</body></html>"
#define OPAQUE1 "11733b200778ce33060f31c9af70a870ba96ddd4"

UUT4WebAdminHttpServer::UUT4WebAdminHttpServer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// don't garbace collect me
	//SetFlags(RF_MarkAsRootSet);

	StopRequested.Reset();
	WorkerThread = FRunnableThread::Create(this, TEXT("UUT4WebAdminHttpServer"), 16 * 1024, TPri_SlightlyBelowNormal);
}

const FString wwwStr = FPaths::GamePluginsDir() + UT4WA_PLUGIN_FOLDER + "/" + UT4WA_WWW_FOLDER + "/";

static struct lws_protocols Protocols[] = {
	/* first protocol must always be HTTP handler */
	{ "http-only", UUT4WebAdminHttpServer::CallBack_HTTP, 0 },
	{ NULL, NULL, 0 }
};

uint32 UUT4WebAdminHttpServer::Run()
{
	while (!StopRequested.GetValue())
	{
		lws_service(Context, 10);
		lws_callback_on_writable_all_protocol(Context, &Protocols[0]);
	}

	UE_LOG(UT4WebAdmin, Log, TEXT("UT4 WebAdmin Http Server is now Shutting down "));
	return true;
}

// Called internally by FRunnableThread::Kill.
void UUT4WebAdminHttpServer::Stop()
{
	StopRequested.Set(true);
}

void UUT4WebAdminHttpServer::Exit()
{
	lws_context_destroy(Context);
	Context = NULL;
}

int UUT4WebAdminHttpServer::ServeJsonObject(struct lws *wsi, TSharedPtr<FJsonObject> json)
{
	FString JsonText;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JsonText);
	FJsonSerializer::Serialize(json.ToSharedRef(), Writer);

	std::string tmpStr = TCHAR_TO_ANSI(*JsonText);
	char *jsonChar = &tmpStr[0u];

	char *Header = (char *) "HTTP/1.1 200 OK\r\nServer: UT4Webadmin Server\r\nConnection: close\r\nContent-Type: application/json\r\n\r\n";

	char *Response = NULL;
	Response = (char *) malloc(strlen(Header) + strlen(jsonChar));
	sprintf(Response, "%s%s", Header, jsonChar);

	return lws_write_http(wsi, Response, strlen(Response));

	// TODO make it back work
	/*
	// allow cross request from instanced http server http://<hostname>:(<InstancePort>+100) to lobby parent
	AUTBaseGameMode* BaseGameMode = Cast<AUTBaseGameMode>(GWorld->GetAuthGameMode());
	if (BaseGameMode && BaseGameMode->IsGameInstanceServer()) {
	// TODO set real hostname and port
	MHD_add_response_header(response, MHD_HTTP_HEADER_ACCESS_CONTROL_ALLOW_ORIGIN, "http://localhost:8080");
	}
	*/
}

static char* getMimeFromExtension(char* extension) {
	char *mime;

	// choose mime type based on the file extension
	if (extension == NULL) {
		mime = (char*) "text/plain";
	}
	else if (strcmp(extension, ".png") == 0) {
		mime = (char*) "image/png";
	}
	else if (strcmp(extension, ".jpg") == 0) {
		mime = (char*) "image/jpg";
	}
	else if (strcmp(extension, ".gif") == 0) {
		mime = (char*) "image/gif";
	}
	else if (strcmp(extension, ".html") == 0) {
		mime = (char*) "text/html";
	}
	else if (strcmp(extension, ".css") == 0) {
		mime = (char*) "text/css";
	}
	else if (strcmp(extension, ".ico") == 0) {
		mime = (char*) "image/x-icon";
	}
	else {
		mime = (char*) "text/plain";
	}

	return mime;
}


int UUT4WebAdminHttpServer::CallBack_HTTP(
	struct lws *wsi,
	enum lws_callback_reasons reason,
	void *user,
	void *in, 
	size_t len
) {

	struct lws_context *Context = lws_get_context(wsi);

	switch (reason) {


	case LWS_CALLBACK_HTTP: {

		lws_set_timeout(wsi, NO_PENDING_TIMEOUT, 10); // TODO useful? delete?

		char *requested_uri = (char *)in;

		// not a post request
		if (!lws_hdr_total_length(wsi, WSI_TOKEN_POST_URI)) {


			char *path = NULL;
			const char *www = TCHAR_TO_ANSI(*wwwStr);

			// server index.html file
			if (FCString::Strcmp(ANSI_TO_TCHAR(requested_uri), TEXT("/")) == 0)
			{
				requested_uri = (char*) "index.html";
			}
			else if (FCString::Strcmp(ANSI_TO_TCHAR(requested_uri), TEXT("/gameinfo")) == 0) {
				TSharedPtr<FJsonObject> matchInfoJson = GetGameInfoJSON();
				ServeJsonObject(wsi, matchInfoJson);
				break;
			}
			else if (FCString::Strcmp(ANSI_TO_TCHAR(requested_uri), TEXT("/serverinfo")) == 0) {

				TSharedPtr<FJsonObject> serverInfoJson = GetServerInfoJSON();
				ServeJsonObject(wsi, serverInfoJson);
				break;
			}

			// serving normal file
			path = (char *)malloc(strlen(www) + strlen(requested_uri));

			sprintf(path, "%s%s", www, requested_uri);


			// FIXME SEC prevent reading files not in UT4XWebAdmin www folder
			char *extension = strrchr(path, '.');

			lws_serve_http_file(wsi, path, getMimeFromExtension(extension), NULL, 0);
		}
		// not handled yet
		else
		{
			// we got a post request!, queue up a write callback.
			//lws_callback_on_writable(wsi);
		}

		lws_close_reason(wsi, LWS_CLOSE_STATUS_NORMAL, (unsigned char *)"seeya", 5);
		break;
	}

	default:
		break;
	}


	return 0;
}



void lws_debugLog(int level, const char *line)
{
	UE_LOG(UT4WebAdmin, Warning, TEXT(" LibWebsocket: %s"), ANSI_TO_TCHAR(line));
}

bool started = false;

bool UUT4WebAdminHttpServer::Init()
{
	// FIXME for some unknow reason the init function is called 3 times then it crashes
	// this 'trick' prevents the crash
	if (started) {
		return false;
	}

	started = true;

	struct lws_context_creation_info ContextInfo = {};

	lws_set_log_level(LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_DEBUG, lws_debugLog);

	if (WebHttpPort == 0) {
		WebHttpPort = 8080;
	}

	

	// for lobby instance server open http port at (instance port + 100)

	/*
	BaseGameMode = Cast<AUTBaseGameMode>(GWorld->GetAuthGameMode());
	bool IsGameInstanceServer = BaseGameMode->IsGameInstanceServer();

	if (IsGameInstanceServer) {
		FString addressUrl = GWorld->GetAddressURL();

		TArray<FString> PathItemList;
		addressUrl.ParseIntoArray(PathItemList, TEXT(":"), true);

		httpPort = FCString::Atoi(*PathItemList[PathItemList.Num() - 1]) + 100;
	}
	*/


	ContextInfo.port = WebHttpPort;
	ContextInfo.iface = NULL;
	ContextInfo.protocols = Protocols;
	ContextInfo.uid = -1;
	ContextInfo.gid = -1;
	ContextInfo.options = 0;
	//ContextInfo.user = this;
	ContextInfo.extensions = NULL;


	if (WebHttpsEnabled) {
		//ContextInfo.port = 443;
		ContextInfo.ssl_cert_filepath = "D:\\server_certificate.pem";
		ContextInfo.ssl_private_key_filepath = "D:\\server_key.pem";
		ContextInfo.ssl_cipher_list =  nullptr;
		ContextInfo.options |= LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT;
	}

	//ContextInfo.options |= LWS_SERVER_OPTION_VALIDATE_UTF8;


	Context = lws_create_context(&ContextInfo);

	if (Context == NULL) {
		UE_LOG(UT4WebAdmin, Warning, TEXT(" * * * * * * * * * * * * * * * * * * * * * * *"));
		UE_LOG(UT4WebAdmin, Warning, TEXT(" UT4WebAdmin failed to start http(s) server at port: %i "), WebHttpPort);
		UE_LOG(UT4WebAdmin, Warning, TEXT(" * * * * * * * * * * * * * * * * * * * * * * *"));
		return false;
	}
	else {
		UE_LOG(UT4WebAdmin, Log, TEXT(" * * * * * * * * * * * * * * * * * * * * * * *"));
		UE_LOG(UT4WebAdmin, Log, TEXT(" UT4WebAdmin started at port: %i "), WebHttpPort);
		UE_LOG(UT4WebAdmin, Log, TEXT(" * * * * * * * * * * * * * * * * * * * * * * *"));
	}

	//Ready.Set(true);
	return true;
}

/*
void UUT4WebAdminHttpServer::Tick(float DeltaTime)
{
	
}

TStatId UUT4WebAdminHttpServer::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UUT4WebAdminHttpServer, STATGROUP_Tickables);
}*/