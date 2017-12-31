#pragma once

#include "UT4WebAdmin.h"
#include "UT4WebAdminBaseInfo.h"
#include "UT4WebAdminTypes.h"
#include "UT4WebAdminSQLite.h"

TArray<FAssetData> AllMaps;
TSharedPtr<FJsonObject> GetMapInfoJSON(TWeakObjectPtr<AUTReplicatedMapInfo> MatchInfo);
TSharedPtr<FJsonObject> GetChatMessagesJSON(TArray<FChatRow>& ChatRows);
TSharedPtr<FJsonObject> GetGameInfoJSON(UUT4WebAdminSQLite* _SQLiteServer);