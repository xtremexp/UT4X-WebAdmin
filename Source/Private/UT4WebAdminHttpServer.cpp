
//#include "UT4WebAdmin.h"
//#include "UTBaseGameMode.h"
#include "UT4WebAdminGameInfo.h"
#include "UT4WebAdminServerInfo.h"
//#include "UT4WebAdminUtils.h"

#define UT4WA_WWW_FOLDER "www"

#define IS_DEBUG 1

#define PAGE1 "<html><head><title>File not found</title></head><body>File not found</body></html>"
#define DENIED "<html><head><title>Acces denied</title></head><body>Access denied</body></html>"
#define OPAQUE1 "11733b200778ce33060f31c9af70a870ba96ddd4"

/*
UUT4WebAdminHttpServer::UUT4WebAdminHttpServer(int32 InPort, const UUT4WebAdminSQLite* SQLiteServer)
	: Super()*/
/*
UUT4WebAdminHttpServer::UUT4WebAdminHttpServer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	{
	// don't garbace collect me
	//SetFlags(RF_MarkAsRootSet);

	StopRequested.Reset();
	WorkerThread = FRunnableThread::Create(this, TEXT("UUT4WebAdminHttpServer"), 16 * 1024, TPri_AboveNormal);
}
*/
UUT4WebAdminHttpServer::UUT4WebAdminHttpServer(int32 InPort, UUT4WebAdminSQLite* SQLiteServer)
{
	this->WebHttpPort = InPort;
	this->_SQLite = SQLiteServer;

	StopRequested.Reset();
	WorkerThread = FRunnableThread::Create(this, TEXT("UUT4WebAdminHttpServer"), 16 * 1024, TPri_AboveNormal);
}

const FString wwwStr = FPaths::GamePluginsDir() + UT4WA_PLUGIN_FOLDER + "/" + UT4WA_WWW_FOLDER + "/";

// a object of this type is associated by libwebsocket to every http session.
struct PerSessionData
{
	// data being received.
	TArray<uint8> In;
	// data being sent out.
	TArray<uint8> Out;
};


static struct lws_protocols Protocols[] = {
	/* first protocol must always be HTTP handler */
	{ "http-only",
		UUT4WebAdminHttpServer::CallBack_HTTP,
		sizeof(struct PerSessionData),
		15 * 1024,
		0,
		NULL
	},
	{ NULL, NULL, 0, 0, 0, NULL }
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

	char Header[1024];

	if (IsGameInstanceServer()) {
		sprintf(Header, "HTTP/1.1 200 OK\x0d\x0a"
			"Server: UT4Webadmin Server\x0d\x0a"
			"Connection: close\x0d\x0a"
			"Content-Type: application/json\x0d\x0a"
			// allow http parent hub server to query json to child hub instance server
			"Access-Control-Allow-Origin: http://localhost:8080" // TODO use real server hostname and configured port
			"Content-Length: %d\x0d\x0a\x0d\x0a", (int)strlen(jsonChar));
	}
	else {
		sprintf(Header, "HTTP/1.1 200 OK\x0d\x0a"
			"Server: UT4Webadmin Server\x0d\x0a"
			"Connection: close\x0d\x0a"
			"Content-Type: application/json\x0d\x0a"
			"Content-Length: %d\x0d\x0a\x0d\x0a", (int)strlen(jsonChar));
	}

	char *Response = NULL;
	Response = (char *) malloc(strlen(Header) + strlen(jsonChar));
	sprintf(Response, "%s%s", Header, jsonChar);

	return lws_write_http(wsi, Response, strlen(Response));
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

	PerSessionData* BufferInfo = (PerSessionData*) user;
	struct lws_context *Context = lws_get_context(wsi);
	UUT4WebAdminHttpServer* Server = (UUT4WebAdminHttpServer*)lws_context_user(Context);

	switch (reason) {


	case LWS_CALLBACK_HTTP: {

		lws_set_timeout(wsi, NO_PENDING_TIMEOUT, 2); // TODO useful? delete?

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
				lws_close_reason(wsi, LWS_CLOSE_STATUS_NORMAL, (unsigned char *)"seeya", 5);
				break;
			}
			else if (FCString::Strcmp(ANSI_TO_TCHAR(requested_uri), TEXT("/serverinfo")) == 0) {

				TSharedPtr<FJsonObject> serverInfoJson = GetServerInfoJSON();
				ServeJsonObject(wsi, serverInfoJson);
				lws_close_reason(wsi, LWS_CLOSE_STATUS_NORMAL, (unsigned char *)"seeya", 5);
				break;
			}
			else if (FCString::Strcmp(ANSI_TO_TCHAR(requested_uri), TEXT("/chatinfo")) == 0) {
				TSharedPtr<FJsonObject> chatInfoJson = GetChatInfoJSON(Server->_SQLite);
				ServeJsonObject(wsi, chatInfoJson);
				lws_close_reason(wsi, LWS_CLOSE_STATUS_NORMAL, (unsigned char *)"seeya", 5);
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
			UE_LOG(UT4WebAdmin, Warning, TEXT(" POST REQUEST !!!"));
			// we got a post request!, queue up a write callback.
			lws_callback_on_writable(wsi);
		}

		//lws_close_reason(wsi, LWS_CLOSE_STATUS_NORMAL, (unsigned char *)"seeya", 5);
		break;
	}
	
	case LWS_CALLBACK_HTTP_BODY: {
		UE_LOG(UT4WebAdmin, Warning, TEXT(" INCOMING POST REQUEST !!!"));
		// post data is coming in, push it on to our incoming buffer.
		//UE_LOG(LogFileServer, Log, TEXT("Incoming HTTP Partial Body Size %d, total size  %d"), Len, Len + BufferInfo->In.Num());
		// FIXME BufferInfo crashing
		//BufferInfo->In.Append((uint8*) in, len);
		// we received some data - update time out.
		lws_set_timeout(wsi, NO_PENDING_TIMEOUT, 60);
		break;
	}
								 
	// POST REQUEST ENDED FROM CLIENT
	// SEND BACK
	case LWS_CALLBACK_HTTP_BODY_COMPLETION:
	{
		UE_LOG(UT4WebAdmin, Warning, TEXT(" INCOMING LWS_CALLBACK_HTTP_BODY_COMPLETION REQUEST !!!"));
		// we have all the post data from the client.
		// create archives and process them.
		UE_LOG(UT4WebAdmin, Log, TEXT("Incoming HTTP total size  %d"), BufferInfo->In.Num());
		FMemoryReader Reader(BufferInfo->In);
		TArray<uint8> Writer;

		//FNetworkFileServerHttp::Process(Reader, Writer, Server);

		ANSICHAR Json[1024];
		int LengthJson = FCStringAnsi::Sprintf(
			(ANSICHAR*)Json,
			"{json:sucess,\r\ndata:'fuck'}"//,
											  //Writer.Num()
		);

		// even if we have 0 data to push, tell the client that we don't any data.
		ANSICHAR Header[1024];
		int Length = FCStringAnsi::Sprintf(
			(ANSICHAR*)Header,
			"HTTP/1.1 200 OK\x0d\x0a"
			"Server: Unreal File Server\x0d\x0a"
			"Connection: close\x0d\x0a"
			"Content-Type: application/json\x0d\x0a",
			//"Content-Type: application/octet-stream \x0d\x0a",
			"Content-Length: %d\x0d\x0a\x0d\x0a",
			LengthJson
			//Writer.Num()
		);

		

		// Add Http Header
		BufferInfo->Out.Append((uint8*)Header, Length);
		// Add Binary Data.
		BufferInfo->Out.Append((uint8*)Json, LengthJson);

		// we have enqueued data increase timeout and push a writable callback.
		lws_set_timeout(wsi, NO_PENDING_TIMEOUT, 60);
		lws_callback_on_writable(wsi);

		break;
	}

	case LWS_CALLBACK_CLOSED_HTTP: {
		UE_LOG(UT4WebAdmin, Warning, TEXT(" LWS_CALLBACK_CLOSED_HTTP"));


		// client went away or
		//clean up.
		BufferInfo->In.Empty();
		BufferInfo->Out.Empty();

		break;
	}

	case LWS_CALLBACK_PROTOCOL_DESTROY: {
		// we are going away.
		UE_LOG(UT4WebAdmin, Warning, TEXT(" LWS_CALLBACK_PROTOCOL_DESTROY"));
		break;
	}

	case LWS_CALLBACK_HTTP_WRITEABLE: {

		// get rid of superfluous write callbacks.
		if (BufferInfo == NULL)
			break;

		// we have data o send out.
		if (BufferInfo->Out.Num())
		{
			UE_LOG(UT4WebAdmin, Warning, TEXT(" LWS_CALLBACK_HTTP_WRITEABLE"));
			int SentSize = lws_write(wsi, (unsigned char*)BufferInfo->Out.GetData(), BufferInfo->Out.Num(), LWS_WRITE_HTTP);
			UE_LOG(UT4WebAdmin, Warning, TEXT(" Sent bytes: %d"), SentSize);
			// get rid of the data that has been sent.
			BufferInfo->Out.RemoveAt(0, SentSize);
		}

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
bool startIt = false;

bool UUT4WebAdminHttpServer::Init()
{
	startIt = true;

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

	bool isGameInstance = IsGameInstanceServer();

	if (isGameInstance) {
		WebHttpPort += 100;
		/*
		FString addressUrl = GWorld->GetAddressURL();

		TArray<FString> PathItemList;
		// get current port from master server
		addressUrl.ParseIntoArray(PathItemList, TEXT(":"), true);

		WebHttpPort = FCString::Atoi(*PathItemList[PathItemList.Num() - 1]) + 100;
		*/
	}


	ContextInfo.port = WebHttpPort;
	ContextInfo.iface = NULL;
	ContextInfo.protocols = Protocols;
	ContextInfo.uid = -1;
	ContextInfo.gid = -1;
	ContextInfo.options = 0;
	ContextInfo.user = this;
	ContextInfo.extensions = NULL;

	// disabled - FIXME http server won't work properly if enabled
	/*
	if (WebHttpsEnabled && !isGameInstance) {
		//ContextInfo.port = 443;
		ContextInfo.ssl_cert_filepath = "D:\\server_certificate.pem";
		ContextInfo.ssl_private_key_filepath = "D:\\server_key.pem";
		ContextInfo.ssl_cipher_list =  nullptr;
		ContextInfo.options |= LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT;
	}*/

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