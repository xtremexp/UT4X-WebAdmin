#pragma once

#include "UT4WebAdmin.h"
#include "UT4WebAdminBaseInfo.h"


TSharedPtr<FJsonObject> GetMatchUpdateJSON(FMatchUpdate* FMatchUpdate);
TSharedPtr<FJsonObject> GetRemotePlayerInfoJSON(FRemotePlayerInfo* RemotePlayerInfo);
TSharedPtr<FJsonObject> GetMapInfoJSON(TWeakObjectPtr<AUTReplicatedMapInfo> MatchInfo);
TSharedPtr<FJsonObject> GetLobbyMatchInfoJSON(AUTLobbyMatchInfo* AvailableMatch, uint32 InstancePort);
TSharedPtr<FJsonObject> GetUTPlayerStateJSON(AUTPlayerState* PlayerState);
TSharedPtr<FJsonObject> GetGameInfoJSON();