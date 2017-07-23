#pragma once

#if PLATFORM_WINDOWS
#include "WindowsHWrapper.h"
#include "AllowWindowsPlatformTypes.h"
#endif

#include "Core.h"
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
class UUT4WebAdminHttpServer : public UObject, public FTickableGameObject
{
	GENERATED_UCLASS_BODY()

	void Start();
	void StartWithTLS(uint32 httpPort);
	void StartWithoutTLS(uint32 httpPort);
	void Stop();

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
	struct lws_context *context;
	AUTBaseGameMode* BaseGameMode;
};