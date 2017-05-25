#pragma once

#include <iostream>
#include "Core.h"
#include "UnrealTournament.h"
#include "UTGameMode.h"
#include "UTLobbyGameState.h"
#include "UTLobbyGameMode.h"
#include "UTLobbyMatchInfo.h"
#include "UTLobbyPlayerState.h"

#include "ThirdParty/Libmicrohttpd/include/microhttpd.h"


#include "UT4WebAdmin.generated.h"

// Log messages
DEFINE_LOG_CATEGORY_STATIC(UT4WebAdmin, Log, All);


UCLASS(Config=UT4WebAdmin)
class UUT4WebAdmin : public UObject, public FTickableGameObject
{

	GENERATED_UCLASS_BODY()

	void Init();
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

	void StartMicroHttp();

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

private:
	struct MHD_Daemon *daemon;
	AUTGameMode* GameMode;
};