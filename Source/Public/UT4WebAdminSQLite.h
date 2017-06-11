#pragma once

#include "Core.h"
#include "UnrealTournament.h"
#include "UTGameInstance.h"
#include "UT4WebAdminTypes.h"
#include "ThirdParty/sqlite/sqlite3.h"

#include "UT4WebAdminSQLite.generated.h"

#define UT4WA_SQL_FOLDER "sql"

USTRUCT(BlueprintType)
struct FDbRow
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(BlueprintReadOnly)
		TArray<FString> Text;
};

UCLASS(Config=UT4WebAdmin)
class UUT4WebAdminSQLite : public UObject
{
	GENERATED_UCLASS_BODY()


	/* Starts SQLite  */
	void Start();

	/* Stops SQLLite - Closes database */
	void Stop();

	/* Adapted function from original UTGameInstance.cpp adding parameters binding for safer sql (injection) */
	bool ExecDatabaseCommand(const FString& DatabaseCommand, TArray<FDbRow>& DatabaseRows);

	/* Save chat messages for current session - Messages might be cleaned up after each new session */
	void SaveChatMessageForSession(const FString& sessionName, const FString& senderName, const FString& senderUniqueId, const FString& message);

	/* Chat history saved to SQLite DB */
	void SaveChatMessage(const FString& senderName, const FUniqueNetIdRepl& senderUniqueId, int32 senderTeamNum, const FString& message);

	/* Get chat history from SQLite DB */
	bool GetChatMessages(TArray<FChatRow>& ChatRows);

	/* Utility function to bind fstring parameter to sql statement */
	int bind_text(sqlite3_stmt* stmt, int idx, const FString& text);

protected:
	struct sqlite3* Database;
};