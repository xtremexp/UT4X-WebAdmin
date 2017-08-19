#pragma once

#include "UT4WebAdmin.h"
#include "UT4WebAdminBaseInfo.h"
#include "UT4WebAdminTypes.h"

TArray<FAssetData> AllMaps;
TSharedPtr<FJsonObject> GetMapInfoJSON(TWeakObjectPtr<AUTReplicatedMapInfo> MatchInfo);
TSharedPtr<FJsonObject> GetGameInfoJSON();