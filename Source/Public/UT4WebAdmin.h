#pragma once

#include <iostream>
#include "Core.h"
#include "UnrealTournament.h"
#include "UTGameMode.h"
#include "UT4WebAdminHttpServer.h"

#include "UT4WebAdmin.generated.h"

// Log messages
DEFINE_LOG_CATEGORY_STATIC(UT4WebAdmin, Log, All);


UCLASS(Config=UT4WebAdmin)
class UUT4WebAdmin : public UObject
{

	GENERATED_UCLASS_BODY()

	void Start();
	void Stop();

	// Main administrator username
	UPROPERTY(Config)
		FString HeadAdminUsername;

	// Main administrator password
	UPROPERTY(Config)
		FString HeadAdminPassword;


	UUT4WebAdminHttpServer* _HttpServer;
private:
	AUTGameMode* GameMode;
};