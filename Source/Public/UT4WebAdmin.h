#pragma once

#include "Core.h"
#include "UnrealTournament.h"

#include "civetweb.h"
#include "CivetServer.h"

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

	/* Listening port of http/webadmin server */
	UPROPERTY(Config)
		uint32 Port;

private:
	/* Http server reference */
	struct mg_context *ctx;
	AUTGameMode* GameMode;
};