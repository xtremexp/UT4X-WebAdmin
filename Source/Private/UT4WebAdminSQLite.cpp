#include "UT4WebAdmin.h"

UUT4WebAdminSQLite::UUT4WebAdminSQLite(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// don't garbace collect me
	SetFlags(RF_MarkAsRootSet);
}

void UUT4WebAdminSQLite::Start() {
	// TODO one db file per instance server ?
	// TODO create file if not exists
	FString DatabasePath = FPaths::GamePluginsDir() / UT4WA_PLUGIN_FOLDER / UT4WA_SQL_FOLDER / "UT4WebAdmin.db";

	if (Database == NULL) {
		int resultCode = sqlite3_open_v2(TCHAR_TO_ANSI(*DatabasePath), &Database, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX, nullptr);

		if (resultCode == SQLITE_OK)
		{
			UE_LOG(UT4WebAdmin, Log, TEXT("SQLite DB file successfully opened at: %s"), *DatabasePath);
		}
		else
		{
			UE_LOG(UT4WebAdmin, Warning, TEXT("Failed at opening SQLite DB file %s with SQLite. Error code: %i"), *DatabasePath, resultCode);
			UE_LOG(UT4WebAdmin, Warning, TEXT("Some features such as chat logs might not be available."));
		}
	}
}

void UUT4WebAdminSQLite::Stop() {
	if (Database)
	{
		int resultCode = sqlite3_close(Database);

		// result code might be SQLITE_BUSY
		if (resultCode != SQLITE_OK) {
			UE_LOG(UT4WebAdmin, Warning, TEXT("Could not close SQLite DB connection. Error code: %i"), resultCode);
		}
	}
}

// slightly modified original UTGameInstance.ExecDatabaseCommand
bool UUT4WebAdminSQLite::ExecDatabaseCommand(const FString& DatabaseCommand, TArray<FDbRow>& DatabaseRows)
{
	int DBreturn = SQLITE_ERROR;

	if (Database)
	{
		// Might want to do small amount of input scrubbing to avoid injection
		// Removing anything after the first semicolon might be enough
		// Could go to a system where parameters get bound, but seems like a pain for mods to use
		sqlite3_stmt* DatabaseStatement = nullptr;

		// TODO make generic bind auto function
		// sqlite3_prepare_v2(db, "insert into demo (name, age) values (?1, ?2);", -1, &stmt, NULL);       /* 1 */
		// sqlite3_bind_text(stmt, 1, "Susan", -1, SQLITE_STATIC); 

		DBreturn = sqlite3_prepare_v2(Database, TCHAR_TO_ANSI(*DatabaseCommand), -1, &DatabaseStatement, nullptr);
		if (DBreturn != SQLITE_OK)
		{
			return false;
		}

		DBreturn = sqlite3_step(DatabaseStatement);
		while (DBreturn == SQLITE_ROW)
		{
			FDbRow NewRow;
			int DBColumnCount = sqlite3_column_count(DatabaseStatement);
			for (int i = 0; i < DBColumnCount; i++)
			{
				int DBColumnType = sqlite3_column_type(DatabaseStatement, i);
				if (DBColumnType == SQLITE_TEXT || DBColumnType == SQLITE3_TEXT)
				{
					const unsigned char* DBText = sqlite3_column_text(DatabaseStatement, i);
					NewRow.Text.Add(ANSI_TO_TCHAR((char*)DBText));
				}
				else if (DBColumnType == SQLITE_INTEGER)
				{
					int DBInteger = sqlite3_column_int(DatabaseStatement, i);
					NewRow.Text.Add(FString::Printf(TEXT("%d"), DBInteger));
				}
				else if (DBColumnType == SQLITE_FLOAT)
				{
					float DBFloat = sqlite3_column_double(DatabaseStatement, i);
					NewRow.Text.Add(FString::Printf(TEXT("%f"), DBFloat));
				}
				else if (DBColumnType == SQLITE_BLOB)
				{
					// nothing to handle yet
				}
			}
			DatabaseRows.Add(NewRow);
			DBreturn = sqlite3_step(DatabaseStatement);
		}
	}

	return (DBreturn == SQLITE_DONE) || (DBreturn == SQLITE_OK);
}

void UUT4WebAdminSQLite::SaveChatMessage(const FString& senderName, const FUniqueNetIdRepl& senderUniqueId, int32 senderTeamNum, const FString& message) {

	if ( Database ) {

		FString sql = "INSERT INTO `ut4webadmin_chat` (`time`, `sender_name`, `sender_uid`, `sender_team_num`, `message`) VALUES (?, ?, ?, ?, ?);";

		sqlite3_stmt* stmt = nullptr;
		int resultCode = sqlite3_prepare_v2(Database, TCHAR_TO_ANSI(*sql), -1, &stmt, nullptr);

		if (resultCode == SQLITE_OK) {
			FString now = FDateTime::Now().ToIso8601();
			FString uidStr = senderUniqueId.ToString();

			//bind_text(stmt, 1, now);
			//const char* nowChar = (const char *)TCHAR_TO_UTF8(*now);

			bind_text(stmt, 1, now);
			bind_text(stmt, 2, uidStr);
			bind_text(stmt, 3, senderName);
			//sqlite3_bind_text(stmt, 1, nowChar, strlen(nowChar), SQLITE_STATIC);
			//sqlite3_bind_text(stmt, 2, (const char *)TCHAR_TO_UTF8(*uidStr), uidStr.Len(), SQLITE_STATIC);
			//sqlite3_bind_text(stmt, 3, (const char *)TCHAR_TO_UTF8(*senderName), senderName.Len(), SQLITE_STATIC);
			sqlite3_bind_int(stmt, 4, senderTeamNum);
			bind_text(stmt, 5, message);
			//sqlite3_bind_text(stmt, 5, (const char *)TCHAR_TO_UTF8(*message), message.Len(), SQLITE_STATIC);

			resultCode = sqlite3_step(stmt);
			if (resultCode != SQLITE_DONE) {
				UE_LOG(UT4WebAdmin, Warning, TEXT("Error inserting data. SQL: %s. Result code: %i"), *sql, resultCode);
			}

			resultCode = sqlite3_finalize(stmt);
		}
		// at this stage table might be missing
		else {
			UE_LOG(UT4WebAdmin, Warning, TEXT("SQLite prepare failed. SQL: %s. Result code: %i"), *sql, resultCode);
		}
	}
}

// FIXME seems to always bind pointer adress
int UUT4WebAdminSQLite::bind_text(sqlite3_stmt* stmt, int idx, const FString& text) {

	int resultCode = SQLITE_ERROR;

	if (*text != NULL) {
		UE_LOG(UT4WebAdmin, Warning, TEXT("Text: %s"), *text);

		resultCode = sqlite3_bind_text(stmt, idx, (const char *) text.GetCharArray().GetData(), text.Len(), SQLITE_STATIC);

		if (resultCode != SQLITE_OK) {
			UE_LOG(UT4WebAdmin, Warning, TEXT("Could not bind text '%s' parameter for sql query at index %i. Result code: %i"), *text, idx, resultCode);
		}
	}
	else {
		resultCode = sqlite3_bind_null(stmt, idx);

		if (resultCode != SQLITE_OK) {
			UE_LOG(UT4WebAdmin, Warning, TEXT("Could not bind text null parameter for sql query at index %i. Result code: %i"), idx, resultCode);
		}
	}

	return resultCode;
}

void UUT4WebAdminSQLite::SaveChatMessageForSession(const FString& sessionName, const FString& senderName, const FString& senderUniqueId, const FString& message) {

	// TODO
}
