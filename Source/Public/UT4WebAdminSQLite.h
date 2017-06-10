#pragma once

#include "Core.h"
#include "UnrealTournament.h"
#include "UTGameInstance.h"

#include "UT4WebAdminSQLite.generated.h"

UCLASS(Config=UT4WebAdmin)
class UUT4WebAdminSQLite : public UObject
{
	GENERATED_UCLASS_BODY()

	/* Save chat messages for current session - Messages might be cleaned up after each new session */
	void SaveChatMessageForSession(FString sessionName, FString senderName, FString senderUniqueId, FString message);

	/* Global chat save - Never deleted */
	void SaveChatMessage(FString senderName, const FUniqueNetIdRepl& senderUniqueId, int32 senderTeamNum, FString message);
};