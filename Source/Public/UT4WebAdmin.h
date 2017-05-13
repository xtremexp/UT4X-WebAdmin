#pragma once

#include "Core.h"
#include "UnrealTournament.h"

#define USE_CIVETWEB 1

#if defined(USE_CIVETWEB)
#include "civetweb.h"
#include "CivetServer.h"
#endif

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

	#if defined(USE_CIVETWEB)
	/* Starts civertweb http server */
	void StartCivetWeb(FString &DocumentRoot, FString &PortStr);

	/* Stops civertweb http server */
	void StopCivetWeb();
	#endif

	/* Listening port of http/webadmin server */
	UPROPERTY(Config)
		uint32 Port;

private:
	#if defined(USE_CIVETWEB)
		struct mg_context *ctx;
	#endif
	AUTGameMode* GameMode;
};