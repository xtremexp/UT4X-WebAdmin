#include "UT4WebAdmin.h"
#include "UT4WebAdminServerInfo.h"


TSharedPtr<FJsonObject> GetServerInfoJSON(AUTBaseGameMode* BaseGameMode)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	AUTGameState* BaseGameState = Cast<AUTGameState>(GWorld->GetGameState());

	if (BaseGameState) {

		// GameMode
		JsonObject->SetStringField(TEXT("ServerName"), BaseGameState->ServerName);
		JsonObject->SetStringField(TEXT("ServerMOTD"), BaseGameState->ServerMOTD);
		JsonObject->SetStringField(TEXT("ServerDescription"), BaseGameState->ServerDescription);
	}

	return JsonObject;
}