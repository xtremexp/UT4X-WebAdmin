#pragma once

#include "UT4WebAdmin.h"
#include "UT4WebAdminBaseInfo.h"


TSharedPtr<FJsonObject> GetMatchInfoJSON(AUTLobbyMatchInfo* AvailableMatch);
TSharedPtr<FJsonObject> GetPlayerInfoJSON(TWeakObjectPtr<AUTLobbyPlayerState> PlayerState);
TSharedPtr<FJsonObject> GetGameInfoJSON();