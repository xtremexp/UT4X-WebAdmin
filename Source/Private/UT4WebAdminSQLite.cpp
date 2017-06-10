#include "UT4WebAdmin.h"

UUT4WebAdminSQLite::UUT4WebAdminSQLite(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// don't garbace collect me
	SetFlags(RF_MarkAsRootSet);
}

void UUT4WebAdminSQLite::SaveChatMessage(FString senderName, const FUniqueNetIdRepl& senderUniqueId, int32 senderTeamNum, FString message) {
	if (GWorld != NULL) {
		UUTGameInstance* UTGameInstance = Cast<UUTGameInstance>(GWorld->GetGameInstance());

		if (UTGameInstance) {
			TArray<FDatabaseRow> DBRows;
			FString sql = "INSERT INTO `ut4webadmin_chat` (`time`, `sender_name`, `sender_uid`, `sender_team_num`, `message`)";
			// TODO SEC DO NOT INSERT IF message contains sensitive key words such as INSERT/DROP/...
			// TODO SEC FILTER BAD CHARS IN MESSAGE
			sql += " VALUES ";
			// ISO 8601 needed for SQLite
			sql += "('" + FDateTime::Now().ToIso8601() + "', '" + senderName +"', '" + senderUniqueId.ToString() + "', '"+ FString::FromInt(senderTeamNum) + "', '" + message + "');";

			UE_LOG(UT4WebAdmin, Log, TEXT(" SQL: %s"), *sql);
			UTGameInstance->ExecDatabaseCommand(sql, DBRows);
		}
	}
}

void UUT4WebAdminSQLite::SaveChatMessageForSession(FString sessionName, FString senderName, FString senderUniqueId, FString message) {

	// TODO
}
