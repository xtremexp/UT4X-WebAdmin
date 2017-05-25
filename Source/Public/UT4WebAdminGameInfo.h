#pragma once

#include "UT4WebAdmin.h"
#include "UT4WebAdminBaseInfo.h"


TSharedPtr<FJsonObject> GetPlayerInfoJSON(TWeakObjectPtr<AUTLobbyPlayerState> PlayerState);
TSharedPtr<FJsonObject> GetGameInfoJSON();