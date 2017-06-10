#pragma once

#include "UT4WebAdmin.h"
#include "UT4WebAdminBaseInfo.h"


TSharedPtr<FJsonObject> GetMapInfoJSON(TWeakObjectPtr<AUTReplicatedMapInfo> MatchInfo);
TSharedPtr<FJsonObject> GetGameInfoJSON();