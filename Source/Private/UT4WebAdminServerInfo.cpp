#include "UT4WebAdmin.h"
#include "UT4WebAdminServerInfo.h"


TSharedPtr<FJsonObject> GetServerInfoJSON()
{

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	if (GWorld) {
		AUTGameState* BaseGameState = Cast<AUTGameState>(GWorld->GetGameState());

		if (BaseGameState) {

			// GameMode
			JsonObject->SetStringField(TEXT("ServerName"), BaseGameState->ServerName);
			JsonObject->SetStringField(TEXT("ServerMOTD"), BaseGameState->ServerMOTD);
			JsonObject->SetStringField(TEXT("ServerDescription"), BaseGameState->ServerDescription);
		}
	}
	else {
		JsonObject->SetStringField(TEXT("ServerName"), "ServerName");
		JsonObject->SetStringField(TEXT("ServerMOTD"), "ServerMOTD");
		JsonObject->SetStringField(TEXT("ServerDescription"), "ServerDescription");
	}

	return JsonObject;
}