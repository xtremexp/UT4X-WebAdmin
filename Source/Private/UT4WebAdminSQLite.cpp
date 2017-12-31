#include "UT4WebAdmin.h"

UUT4WebAdminSQLite::UUT4WebAdminSQLite(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// don't garbace collect me
	SetFlags(RF_MarkAsRootSet);
}

/* Get human comprehensive sqlite result code meaning.*/
FString ResultCodeToMessage(int resultCode) {
	FString message = "";

	switch (resultCode) {
		case SQLITE_ERROR: message = "SQL error or missing database"; break;
		case SQLITE_INTERNAL: message = "Internal logic error in SQLite"; break;
		case SQLITE_PERM: message = "Access permission denied"; break;
		case SQLITE_ABORT: message = "Callback routine requested an abort"; break;
		case SQLITE_BUSY: message = "The database file is locked"; break;
		case SQLITE_LOCKED: message = "A table in the database is locked"; break;
		case SQLITE_NOMEM: message = "A malloc() failed"; break;
		case SQLITE_READONLY: message = "Attempt to write a readonly database"; break;
		case SQLITE_INTERRUPT: message = " Operation terminated by sqlite3_interrupt()"; break;
		case SQLITE_NOTFOUND: message = "Unknown opcode in sqlite3_file_control()"; break;
		case SQLITE_FULL: message = "Insertion failed because database is full"; break;
		case SQLITE_CANTOPEN: message = "Unable to open the database file"; break;
		case SQLITE_PROTOCOL: message = "Database lock protocol error"; break;
		case SQLITE_EMPTY: message = "Database is empty"; break;
		case SQLITE_SCHEMA: message = "The database schema changed"; break;
		case SQLITE_TOOBIG: message = " String or BLOB exceeds size limit"; break;
		case SQLITE_CONSTRAINT: message = "Abort due to constraint violation"; break;
		case SQLITE_MISMATCH: message = "Data type mismatch"; break;
		case SQLITE_MISUSE: message = "Library used incorrectly"; break;
		case SQLITE_NOLFS: message = "Uses OS features not supported on host"; break;
		case SQLITE_AUTH: message = "Authorization denied"; break;
		case SQLITE_FORMAT: message = "Auxiliary database format error"; break;
		case SQLITE_RANGE: message = " 2nd parameter to sqlite3_bind out of range"; break;
		case SQLITE_NOTADB: message = "File opened that is not a database file"; break;
		case SQLITE_NOTICE: message = "Notifications from sqlite3_log()"; break;
		case SQLITE_WARNING: message = "Warnings from sqlite3_log()"; break;
		case SQLITE_ROW: message = "sqlite3_step() has another row ready"; break;
		case SQLITE_DONE: message = "sqlite3_step() has finished executing "; break;
		default:
		break;
	}

	return message;
};

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
			UE_LOG(UT4WebAdmin, Warning, TEXT("Failed at opening SQLite DB file %s with SQLite. Error code: %i %s"), *DatabasePath, resultCode, *ResultCodeToMessage(resultCode));
			UE_LOG(UT4WebAdmin, Warning, TEXT("Some features such as chat logs will not be available."));
		}
	}
}

void UUT4WebAdminSQLite::Stop() {
	if (Database)
	{
		int resultCode = sqlite3_close(Database);

		// result code might be SQLITE_BUSY
		if (resultCode != SQLITE_OK) {
			UE_LOG(UT4WebAdmin, Warning, TEXT("Could not close SQLite DB connection. Error code: %i %s"), resultCode, *ResultCodeToMessage(resultCode));
		}
	}
}



// slightly modified original UTGameInstance.ExecDatabaseCommand
bool UUT4WebAdminSQLite::ExecDatabaseCommandNew(const FString& DatabaseCommand, TArray<FDbRow>& DatabaseRows)
{
	int DBreturn = SQLITE_ERROR;

	if (!Database) {
		FString DatabasePath = FPaths::GamePluginsDir() / UT4WA_PLUGIN_FOLDER / UT4WA_SQL_FOLDER / "UT4WebAdmin.db";
		sqlite3_open_v2(TCHAR_TO_ANSI(*DatabasePath), &Database, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX, nullptr);
	}

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
			UE_LOG(UT4WebAdmin, Warning, TEXT("Could not close execute query. Error code: %i %s"), DBreturn, *ResultCodeToMessage(DBreturn));
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
					const char* DBText = (const char*) sqlite3_column_text(DatabaseStatement, i);

					const int32 ConvertedTextLength = FUTF8ToTCHAR_Convert::ConvertedLength(DBText, strlen(DBText));
					TCHAR* ConvertedText = new TCHAR[ConvertedTextLength];
					// static FORCEINLINE void Convert(TCHAR* Dest, int32 DestLen, const ANSICHAR* Source, int32 SourceLen)
					FUTF8ToTCHAR_Convert::Convert(ConvertedText, ConvertedTextLength, DBText, strlen(DBText));

					NewRow.Text.Add(ConvertedText);// ANSI_TO_TCHAR((char*)ConvertedText));
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

// TODO add parameters for selective search
bool UUT4WebAdminSQLite::GetChatMessages(TArray<FChatRow>& ChatRows) {

	TArray<FDbRow> DatabaseRows;
	FString Sql = "SELECT time, sender_name, sender_uid, sender_team_num, message FROM `ut4webadmin_chat`;";

	ExecDatabaseCommandNew(*Sql, DatabaseRows);

	for (int32 RowNum = 0; RowNum < DatabaseRows.Num(); RowNum ++)
	{
		FChatRow ChatRow;

		for (int32 ColNum = 0; ColNum < DatabaseRows[RowNum].Text.Num(); ColNum++) {
			
			switch (ColNum) {
				// Time in '2017-06-11T14:52:36.628Z' in ISO8601 format
				case 0: ChatRow.Time = DatabaseRows[RowNum].Text[ColNum]; break;
				case 1: ChatRow.SenderName = DatabaseRows[RowNum].Text[ColNum]; break;
				case 2: ChatRow.SenderUidStr = DatabaseRows[RowNum].Text[ColNum]; break;
				case 3: ChatRow.SenderTeamNum = FCString::Atoi(*DatabaseRows[RowNum].Text[ColNum]); break;
				case 4: ChatRow.Message = DatabaseRows[RowNum].Text[ColNum]; break;
				default: break;
			}
		}

		ChatRows.Add(ChatRow);
	}

	return false;
}

void UUT4WebAdminSQLite::SaveChatMessage(const FChatRow& ChatRow) {

	if ( Database ) {
		FString sql = "INSERT INTO `ut4webadmin_chat` (`time`, `sender_name`, `sender_uid`, `sender_team_num`, `message`) VALUES (?, ?, ?, ?, ?);";

		sqlite3_stmt* stmt = nullptr;
		int resultCode = sqlite3_prepare_v2(Database, TCHAR_TO_ANSI(*sql), -1, &stmt, nullptr);

		if (resultCode == SQLITE_OK) {

			bind_text(stmt, 1, ChatRow.Time);
			bind_text(stmt, 2, ChatRow.SenderName);
			bind_text(stmt, 3, ChatRow.SenderUidStr);
			sqlite3_bind_int(stmt, 4, ChatRow.SenderTeamNum);
			bind_text(stmt, 5, ChatRow.Message);

			resultCode = sqlite3_step(stmt);
			if (resultCode != SQLITE_DONE) {
				UE_LOG(UT4WebAdmin, Warning, TEXT("Error inserting data. SQL: %s. Result code: %i %s"), *sql, resultCode, *ResultCodeToMessage(resultCode));
			}

			resultCode = sqlite3_finalize(stmt);
		}
		else {
			UE_LOG(UT4WebAdmin, Warning, TEXT("SQLite prepare failed. SQL: %s. Result code: %i %s"), *sql, resultCode, *ResultCodeToMessage(resultCode));
		}
	}
}

// bind FString parameter for sqlite statement
// note: by default db encoding is UTF-8 in SQLite
int UUT4WebAdminSQLite::bind_text(sqlite3_stmt* stmt, int idx, const FString& text) {

	int resultCode = SQLITE_ERROR;

	if (*text != NULL) {
		// it seems that by default FString got UTF-16  encoded chars internally
		// FString are UTF-16 encoded
		// SQLite need UTF-8 so need to convert the string from UTF-16 to UTF-8
		const int32 ConvertedTextLength = FTCHARToUTF8_Convert::ConvertedLength(*text, text.Len());
		char* ConvertedText = new char[ConvertedTextLength];

		FTCHARToUTF8_Convert::Convert(ConvertedText, ConvertedTextLength, *text, text.Len());

		resultCode = sqlite3_bind_text(stmt, idx, ConvertedText, ConvertedTextLength, SQLITE_STATIC);

		if (resultCode != SQLITE_OK) {
			UE_LOG(UT4WebAdmin, Warning, TEXT("Could not bind text '%s' parameter for sql query at index %i. Result code: %i %s"), *text, idx, resultCode, *ResultCodeToMessage(resultCode));
		}
	}
	else {
		resultCode = sqlite3_bind_null(stmt, idx);

		if (resultCode != SQLITE_OK) {
			UE_LOG(UT4WebAdmin, Warning, TEXT("Could not bind text null parameter for sql query at index %i. Result code: %i %s"), idx, resultCode, *ResultCodeToMessage(resultCode));
		}
	}

	return resultCode;
}

void UUT4WebAdminSQLite::SaveChatMessageForSession(const FString& sessionName, const FString& senderName, const FString& senderUniqueId, const FString& message) {

	// TODO
}
