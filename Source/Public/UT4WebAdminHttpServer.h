#pragma once

#if PLATFORM_WINDOWS
#include "WindowsHWrapper.h"
#include "AllowWindowsPlatformTypes.h"
#endif

#include "Core.h"
#include "HAL/ThreadSafeCounter.h"
#include "HAL/Runnable.h"
#include "UnrealTournament.h"
#include "UTBaseGameMode.h"
#include "UTLobbyGameMode.h"
#include "UT4WebAdminUtils.h"
#include "UT4WebAdminSQLite.h"


#include "ThirdParty/libWebSockets/include/libwebsockets.h"
#include "ThirdParty/libWebSockets/include/lws_config.h"


#if PLATFORM_WINDOWS
#include <io.h>
#endif

#if PLATFORM_LINUX
#include <sys/io.h>
#endif
#include <fcntl.h>

#include "UT4WebAdminHttpServer.generated.h"

UCLASS(Config=UT4WebAdmin)
class UUT4WebAdminHttpServer : public UObject, public FRunnable//, public FTickableGameObject
{
	GENERATED_UCLASS_BODY()

	public:

	// FRunnable Interface
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

	// FTickable Interface
	/*
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override
	{
		return true;
	}
	virtual bool IsTickableWhenPaused() const override
	{
		return true;
	}
	*/

	static int CallBack_HTTP(struct lws *wsi,
		enum lws_callback_reasons reason, void *user,
		void *in, size_t len);

	// Main administrator username
	UPROPERTY(Config)
	FString HeadAdminUsername;

	// Main administrator password
	UPROPERTY(Config)
	FString HeadAdminPassword;

	// Note: disabled some properties until civetweb can use them properly!
	/* Http port of webserver */
	UPROPERTY(Config)
	uint32 WebHttpPort;

	/* If 'true' then http server is enabled else disabled */
	UPROPERTY(Config)
	bool WebHttpsEnabled;

	/* Path to .pem server certificate file */
	UPROPERTY(Config)
	FString WebServerCertificateFile;

	/* Path to .key server key file */
	UPROPERTY(Config)
	FString WebServerKeyFile;

	/* Reference to SQLite DB */
	UUT4WebAdminSQLite* _SQLite;

private:

	// Holds the server thread object.
	FRunnableThread* Thread;

	// Holds a flag indicating whether the thread should stop executing
	FThreadSafeCounter StopRequested;

	// Is the Listner thread up and running. 
	FThreadSafeCounter Running;

	// Service Http connections on this thread.
	FRunnableThread* WorkerThread;


private:
	struct lws_context *Context;
	AUTBaseGameMode* BaseGameMode;
};