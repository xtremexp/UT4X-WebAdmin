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
	SetFlags(RF_MarkAsRootSet);
}


static int callback_http(
	struct lws *wsi,
	enum lws_callback_reasons reason,
	void *user,
	void *in, 
	size_t len
) {
	switch (reason) {
		// http://git.warmcat.com/cgi-bin/cgit/libwebsockets/tree/lib/libwebsockets.h#n260
	case LWS_CALLBACK_CLIENT_WRITEABLE:
		printf("connection established\n");

		// http://git.warmcat.com/cgi-bin/cgit/libwebsockets/tree/lib/libwebsockets.h#n281
	case LWS_CALLBACK_HTTP: {
		char *requested_uri = (char *)in;
		printf("requested URI: %s\n", requested_uri);

		if (strcmp(requested_uri, "/") == 0) {
			const char *universal_response = "Hello, World!";
			// http://git.warmcat.com/cgi-bin/cgit/libwebsockets/tree/lib/libwebsockets.h#n597
			lws_write_http(wsi, universal_response,
				strlen(universal_response));
			break;

		}
		else {

			const char *universal_response = "Hello, World 2!";
			// http://git.warmcat.com/cgi-bin/cgit/libwebsockets/tree/lib/libwebsockets.h#n597
			lws_write_http(wsi, universal_response,
				strlen(universal_response));
			break;

			// try to get current working directory
			//char cwd[1024];
			//char *resource_path;

			//if (getcwd(cwd, sizeof(cwd)) != NULL) {
				/*
				// allocate enough memory for the resource path
				resource_path = malloc(strlen(cwd)
					+ strlen(requested_uri));

				// join current working directory to the resource path
				sprintf(resource_path, "%s%s", cwd, requested_uri);
				printf("resource path: %s\n", resource_path);

				char *extension = strrchr(resource_path, '.');
			
				char *mime;

				// choose mime type based on the file extension
				if (extension == NULL) {
					mime = "text/plain";
				}
				else if (strcmp(extension, ".png") == 0) {
					mime = "image/png";
				}
				else if (strcmp(extension, ".jpg") == 0) {
					mime = "image/jpg";
				}
				else if (strcmp(extension, ".gif") == 0) {
					mime = "image/gif";
				}
				else if (strcmp(extension, ".html") == 0) {
					mime = "text/html";
				}
				else if (strcmp(extension, ".css") == 0) {
					mime = "text/css";
				}
				else {
					mime = "text/plain";
				}
				*/

				// by default non existing resources return code 400
				// for more information how this function handles 
				// headers see it's source code
				// http://git.warmcat.com/cgi-bin/cgit/libwebsockets/tree/lib/parsers.c#n1896

				/*
				lws_serve_http_file(struct lws *wsi, const char *file, const char *content_type,
				const char *other_headers, int other_headers_len);
				*/
				//(wsi, resource_path, "text/plain", NULL, 0);

			//}
		}

		// close connection
		lws_close_reason(
			wsi, LWS_CLOSE_STATUS_NORMAL, NULL, 0);
		break;
	}
	default:
		printf("unhandled callback\n");
		break;
	}

	return 0;
}

static struct lws_protocols protocols[] = {
	// first protocol must always be HTTP handler
	{
		"http-only",
		callback_http,
		0
	},
	{
		NULL, NULL, 0
	}
};

void UUT4WebAdminHttpServer::StartWithTLS(uint32 httpPort)
{
	if (NULL == *WebServerCertificateFile || NULL == *WebServerKeyFile) {
		UE_LOG(UT4WebAdmin, Warning, TEXT("Server key or certificate file is not set."));
		return;
	}

	// openssl req -days 365 -out server.pem -new -x509 -key server.key
	FILE* fcert_file = nullptr;
	//fopen_s(&fcert_file, TCHAR_TO_ANSI(*WebServerCertificateFile), "r");

	// openssl genrsa -out server.key 1024
	FILE* fkey_file = nullptr;
	//fopen_s(&fkey_file, TCHAR_TO_ANSI(*WebServerKeyFile), "r");

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

		const char *interfc = NULL;
		int opts = 0;

		struct lws_context_creation_info ContextInfo = {};


		ContextInfo.port = httpPort;
		ContextInfo.protocols = protocols;
		ContextInfo.uid = -1;
		ContextInfo.gid = -1;
		ContextInfo.options |= LWS_SERVER_OPTION_PEER_CERT_NOT_REQUIRED | LWS_SERVER_OPTION_DISABLE_OS_CA_CERTS;
		ContextInfo.ssl_cipher_list = nullptr;
		

		context = lws_create_context(&ContextInfo);
	}


	if (NULL != fcert_file) {
		fclose(fcert_file);
	}

	if (NULL != fkey_file) {
		fclose(fkey_file);
	}
}


void UUT4WebAdminHttpServer::StartWithoutTLS(uint32 httpPort)
{
	struct lws_context_creation_info ContextInfo = {};


	ContextInfo.port = httpPort;
	ContextInfo.protocols = protocols;
	ContextInfo.uid = -1;
	ContextInfo.gid = -1;
	ContextInfo.options |= LWS_SERVER_OPTION_PEER_CERT_NOT_REQUIRED | LWS_SERVER_OPTION_DISABLE_OS_CA_CERTS;
	ContextInfo.ssl_cipher_list = nullptr;


	context = lws_create_context(&ContextInfo);
}

void UUT4WebAdminHttpServer::Start()
{
	// should not occurs at this stage but who knows ...
	if (GWorld == NULL) {
		UE_LOG(UT4WebAdmin, Warning, TEXT(" Impossible to start UT4WebAdmin - GWorld is NULL."));
		return;
	}

	BaseGameMode = Cast<AUTBaseGameMode>(GWorld->GetAuthGameMode());

	bool IsGameInstanceServer = BaseGameMode->IsGameInstanceServer();

	int httpPort = WebHttpPort;

	if (httpPort == 0) {
		httpPort = 8080;
	}

	// for lobby instance server open http port at (instance port + 100)
	if (IsGameInstanceServer) {
		FString addressUrl = GWorld->GetAddressURL();

		TArray<FString> PathItemList;
		addressUrl.ParseIntoArray(PathItemList, TEXT(":"), /*InCullEmpty=*/true);

		httpPort = FCString::Atoi(*PathItemList[PathItemList.Num() - 1]) + 100;
	}

	// SSL not working yet need some more investigation
	if (WebHttpsEnabled && !IsGameInstanceServer) {
		StartWithTLS(httpPort);
	}
	else {
		StartWithoutTLS(httpPort);
	}
	

	if (context == NULL) {
		UE_LOG(UT4WebAdmin, Warning, TEXT(" * * * * * * * * * * * * * * * * * * * * * * *"));
		UE_LOG(UT4WebAdmin, Warning, TEXT(" UT4WebAdmin failed to start http(s) server at port: %i "), httpPort);
		UE_LOG(UT4WebAdmin, Warning, TEXT(" * * * * * * * * * * * * * * * * * * * * * * *"));
	}
	else {
		UE_LOG(UT4WebAdmin, Log, TEXT(" * * * * * * * * * * * * * * * * * * * * * * *"));
		UE_LOG(UT4WebAdmin, Log, TEXT(" UT4WebAdmin started at port: %i "), httpPort);
		UE_LOG(UT4WebAdmin, Log, TEXT(" * * * * * * * * * * * * * * * * * * * * * * *"));
	}
}

void UUT4WebAdminHttpServer::Stop()
{
	if (NULL != context) {
		lws_context_destroy(context);
	}
}
bool started = false;

// Start HTTPs server when GWorld available
void UUT4WebAdminHttpServer::Tick(float DeltaTime)
{
	if (!started && GWorld != NULL && NULL == context) {
		started = true;
		Start();
	}
	else {
		// TODO poll
	}
}

TStatId UUT4WebAdminHttpServer::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UUT4WebAdminHttpServer, STATGROUP_Tickables);
}