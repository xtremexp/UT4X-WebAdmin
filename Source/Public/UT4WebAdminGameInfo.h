#pragma once

#include "UT4WebAdmin.h"
#include "UT4WebAdminBaseInfo.h"

TSharedPtr<FJsonObject> GetRemotePlayerInfoJSON(FRemotePlayerInfo* RemotePlayerInfo);
TSharedPtr<FJsonObject> GetMapInfoJSON(TWeakObjectPtr<AUTReplicatedMapInfo> MatchInfo);
TSharedPtr<FJsonObject> GetLobbyMatchInfoJSON(AUTLobbyMatchInfo* AvailableMatch, uint32 InstancePort);
TSharedPtr<FJsonObject> GetPlayerInfoJSON(TWeakObjectPtr<AUTLobbyPlayerState> PlayerState);
TSharedPtr<FJsonObject> GetGameInfoJSON();