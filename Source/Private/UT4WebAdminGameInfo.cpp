#include "UT4WebAdmin.h"
#include "UT4WebAdminGameInfo.h"



// WeakObjectPtr<AUTReplicatedGameRuleset>

TSharedPtr<FJsonObject> GetGameRulesetJSON(TWeakObjectPtr<AUTReplicatedGameRuleset> GameRuleset) 
{
	TSharedPtr<FJsonObject> RulesetJson = MakeShareable(new FJsonObject);

	if (GameRuleset != NULL) {
		RulesetJson->SetStringField(TEXT("Title"), GameRuleset->Title);
		RulesetJson->SetStringField(TEXT("Description"), GameRuleset->Description);
		RulesetJson->SetStringField(TEXT("Title"), GameRuleset->Title);
		RulesetJson->SetNumberField(TEXT("MaxPlayers"), GameRuleset->MaxPlayers);

		TArray<TSharedPtr<FJsonValue>> MapListJson;
		// maplist useful to know which map admin can switch to
		for (int32 MapIdx = 0; MapIdx < GameRuleset->MapList.Num(); MapIdx++)
		{
			TSharedPtr<FJsonObject> MapJson = MakeShareable(new FJsonObject);
			MapJson->SetStringField(TEXT("MapPackageName"), GameRuleset->MapList[MapIdx]->MapPackageName);
			MapJson->SetStringField(TEXT("MapAssetName"), GameRuleset->MapList[MapIdx]->MapAssetName);
			MapJson->SetStringField(TEXT("Title"), GameRuleset->MapList[MapIdx]->Title);
			// useful as to check if map is redirected or not
			MapJson->SetStringField(TEXT("PackageURL"), GameRuleset->MapList[MapIdx]->Redirect.PackageURL);

			MapListJson.Add(MakeShareable(new FJsonValueObject(MapJson)));
		}

		RulesetJson->SetArrayField(TEXT("MapList"), MapListJson);
	}

	return RulesetJson;
}



// FRemotePlayerInfo
TSharedPtr<FJsonObject> GetMapInfoJSON(TWeakObjectPtr<AUTReplicatedMapInfo> MapInfo)
{
	TSharedPtr<FJsonObject> MapInfoJson = MakeShareable(new FJsonObject);

	if (MapInfo != NULL) {
		MapInfoJson->SetStringField(TEXT("Title"), MapInfo->Title);
		MapInfoJson->SetStringField(TEXT("Author"), MapInfo->Author);
		MapInfoJson->SetStringField(TEXT("MapPackageName"), MapInfo->MapPackageName);
		MapInfoJson->SetStringField(TEXT("MapAssetName"), MapInfo->MapAssetName);
		MapInfoJson->SetStringField(TEXT("Description"), MapInfo->Description);
		MapInfoJson->SetStringField(TEXT("MapScreenshotReference"), MapInfo->MapScreenshotReference);
		MapInfoJson->SetStringField(TEXT("RedirectPackageName"), MapInfo->Redirect.PackageName);
		MapInfoJson->SetStringField(TEXT("RedirectPackageURL"), MapInfo->Redirect.PackageURL);
		
	}

	return MapInfoJson;
}


TSharedPtr<FJsonObject> GetInstanceInfoJSON(AUTLobbyMatchInfo* LobbyMatchInfo, AUTGameMode* UTGameMode)
{
	TSharedPtr<FJsonObject> InstanceInfoJson = MakeShareable(new FJsonObject);
	AUTGameState* UTGameState = NULL;
	
	if (UTGameMode) {
		UTGameState = UTGameMode->UTGameState;
	}

	// Global Match info
	// Data from lobby parent server
	if (LobbyMatchInfo) {
		InstanceInfoJson->SetNumberField(TEXT("bPrivateMatch"), LobbyMatchInfo->bPrivateMatch);
		InstanceInfoJson->SetNumberField(TEXT("bRankLocked"), LobbyMatchInfo->bRankLocked);
		InstanceInfoJson->SetNumberField(TEXT("NumPlayers"), LobbyMatchInfo->NumPlayersInMatch());
		InstanceInfoJson->SetNumberField(TEXT("NumSpectators"), LobbyMatchInfo->NumSpectatorsInMatch());
		InstanceInfoJson->SetNumberField(TEXT("MaxPlayers"), 0);

		InstanceInfoJson->SetStringField(TEXT("ServerName"), LobbyMatchInfo->CustomGameName);
		InstanceInfoJson->SetStringField(TEXT("ServerMOTD"), "");
		InstanceInfoJson->SetStringField(TEXT("CurrentState"), LobbyMatchInfo->CurrentState.ToString());

		FMatchUpdate* MatchUpdate = &LobbyMatchInfo->MatchUpdate;

		InstanceInfoJson->SetNumberField(TEXT("TimeLimit"), MatchUpdate->TimeLimit);
		InstanceInfoJson->SetNumberField(TEXT("GoalScore"), MatchUpdate->GoalScore);
		InstanceInfoJson->SetNumberField(TEXT("ElapsedTime"), MatchUpdate->GameTime);

		InstanceInfoJson->SetBoolField(TEXT("HasMatchStarted"), MatchUpdate->bMatchHasBegun);
		InstanceInfoJson->SetBoolField(TEXT("HasMatchEnded"), MatchUpdate->bMatchHasEnded);
		InstanceInfoJson->SetBoolField(TEXT("bTeamGame"), MatchUpdate->TeamScores.Num() > 0);

		InstanceInfoJson->SetStringField(TEXT("MatchState"), MatchUpdate->MatchState.ToString());
	}
	// Data from dedi or instanced lobby server
	else if(UTGameMode) {
		InstanceInfoJson->SetNumberField(TEXT("bPrivateMatch"), UTGameMode->bPrivateMatch);
		InstanceInfoJson->SetNumberField(TEXT("bRankLocked"), UTGameMode->bRankLocked);
		InstanceInfoJson->SetNumberField(TEXT("NumPlayers"), UTGameMode->GetNumPlayers());
		InstanceInfoJson->SetNumberField(TEXT("NumSpectators"), 0);
		InstanceInfoJson->SetNumberField(TEXT("MaxPlayers"), UTGameMode->DefaultMaxPlayers);

		if (UTGameState) {
			InstanceInfoJson->SetStringField(TEXT("ServerName"), UTGameState->ServerName);
			InstanceInfoJson->SetStringField(TEXT("ServerMOTD"), UTGameState->ServerMOTD);
			InstanceInfoJson->SetStringField(TEXT("CurrentState"), UTGameState->GetMatchState().ToString());

			InstanceInfoJson->SetNumberField(TEXT("TimeLimit"), UTGameState->TimeLimit);
			InstanceInfoJson->SetNumberField(TEXT("GoalScore"), UTGameState->GoalScore);
			InstanceInfoJson->SetNumberField(TEXT("ElapsedTime"), UTGameState->ElapsedTime);

			InstanceInfoJson->SetBoolField(TEXT("HasMatchStarted"), UTGameState->HasMatchStarted());
			InstanceInfoJson->SetBoolField(TEXT("HasMatchEnded"), UTGameState->HasMatchEnded());
			InstanceInfoJson->SetBoolField(TEXT("bTeamGame"), UTGameState->bTeamGame);

			InstanceInfoJson->SetStringField(TEXT("MatchState"), UTGameMode->GetMatchState().ToString());
		}
	}

	TArray<TSharedPtr<FJsonValue>> PlayersJson;

	// Players
	if (LobbyMatchInfo) {
		
		for (int32 PlayerIdx = 0; PlayerIdx < LobbyMatchInfo->PlayersInMatchInstance.Num(); PlayerIdx++)
		{
			FRemotePlayerInfo* RemotePlayerInfo = &LobbyMatchInfo->PlayersInMatchInstance[PlayerIdx];
			if (RemotePlayerInfo) {
				TSharedPtr<FJsonObject> PlayerJson = MakeShareable(new FJsonObject);
				PlayerJson->SetStringField(TEXT("UniqueId"), RemotePlayerInfo->PlayerID.ToString());
				PlayerJson->SetBoolField(TEXT("bIsSpectator"), RemotePlayerInfo->bIsSpectator);
				PlayerJson->SetStringField(TEXT("PlayerName"), RemotePlayerInfo->PlayerName);
				PlayerJson->SetNumberField(TEXT("PlayerScore"), RemotePlayerInfo->PlayerScore);
				PlayerJson->SetNumberField(TEXT("XPLevel"), RemotePlayerInfo->XPLevel);
				PlayerJson->SetNumberField(TEXT("TeamNum"), RemotePlayerInfo->TeamNum); // 0 = Red, 1 = Blue
				PlayerJson->SetStringField(TEXT("Avatar"), RemotePlayerInfo->Avatar.ToString());
				PlayerJson->SetBoolField(TEXT("bIsABot"), false); // lobby always send real players data
				PlayersJson.Add(MakeShareable(new FJsonValueObject(PlayerJson)));
			}
		}
	}
	else if (UTGameMode && UTGameState) {
		for (int32 Idx = 0; Idx < UTGameState->PlayerArray.Num(); Idx++)
		{
			AUTPlayerState* UTPlayerState = Cast<AUTPlayerState>(UTGameState->PlayerArray[Idx]);
			if (UTPlayerState != NULL) {
				TSharedPtr<FJsonObject> PlayerJson = MakeShareable(new FJsonObject);
				PlayerJson->SetStringField(TEXT("UniqueId"), UTPlayerState->UniqueId.ToString());
				PlayerJson->SetBoolField(TEXT("bIsSpectator"), UTPlayerState->bIsSpectator);
				PlayerJson->SetStringField(TEXT("PlayerName"), UTPlayerState->PlayerName);
				PlayerJson->SetNumberField(TEXT("PlayerScore"), UTPlayerState->Score);
				PlayerJson->SetNumberField(TEXT("XPLevel"), UTPlayerState->GetPrevXP());
				PlayerJson->SetNumberField(TEXT("TeamNum"), UTPlayerState->GetTeamNum()); // 0 = Red, 1 = Blue
				PlayerJson->SetStringField(TEXT("Avatar"), UTPlayerState->Avatar.ToString());
				PlayerJson->SetBoolField(TEXT("bIsABot"), UTPlayerState->bIsABot);

				// Extra info only from dedi server can't get this from lobby parent server
				PlayerJson->SetNumberField(TEXT("Kills"), UTPlayerState->Kills);
				PlayerJson->SetNumberField(TEXT("DamageDone"), UTPlayerState->DamageDone);
				PlayerJson->SetNumberField(TEXT("Deaths"), UTPlayerState->Deaths);
				PlayerJson->SetBoolField(TEXT("bIsRconAdmin"), UTPlayerState->bIsRconAdmin);
				PlayerJson->SetStringField(TEXT("ClanName"), UTPlayerState->ClanName);
				PlayerJson->SetNumberField(TEXT("ElapsedTime"), UTPlayerState->ElapsedTime);
				PlayerJson->SetStringField(TEXT("CountryFlag"), UTPlayerState->CountryFlag.ToString());
				PlayerJson->SetStringField(TEXT("EpicAccountName"), UTPlayerState->EpicAccountName);
				PlayerJson->SetNumberField(TEXT("LastActiveTime"), UTPlayerState->LastActiveTime);
				PlayerJson->SetNumberField(TEXT("Ping"), UTPlayerState->Ping);
				PlayerJson->SetStringField(TEXT("SavedNetworkAddress"), UTPlayerState->SavedNetworkAddress);

				PlayersJson.Add(MakeShareable(new FJsonValueObject(PlayerJson)));
			}
		}
	}

	InstanceInfoJson->SetArrayField(TEXT("Players"), PlayersJson);

	// Map Played
	TSharedPtr<FJsonObject> MapInfoJson = NULL;

	// Current map being played
	if (LobbyMatchInfo) {
		MapInfoJson = GetMapInfoJSON(LobbyMatchInfo->InitialMapInfo);
	}
	else if (UTGameMode && UTGameState) {
		// TODO get map info for dedi
		MapInfoJson = MakeShareable(new FJsonObject); //GetMapInfoJSON(LobbyMatchInfo->InitialMapInfo);
	}

	InstanceInfoJson->SetObjectField(TEXT("Map"), MapInfoJson);


	// Map List
	TArray<TSharedPtr<FJsonValue>> MapInfosJson;

	if (LobbyMatchInfo) {
		TWeakObjectPtr<AUTReplicatedGameRuleset> GameRuleset = LobbyMatchInfo->CurrentRuleset;
		if (GameRuleset != NULL) {
			for (int32 MapIdx = 0; MapIdx < GameRuleset->MapList.Num(); MapIdx++)
			{
				TSharedPtr<FJsonObject> MapInfoJson = GetMapInfoJSON(GameRuleset->MapList[MapIdx]);
				MapInfosJson.Add(MakeShareable(new FJsonValueObject(MapInfoJson)));
			}
		}
	}
	else if (UTGameMode && UTGameState) {
		for (int32 Idx = 0; Idx < UTGameState->MapVoteList.Num(); Idx++)
		{
			AUTReplicatedMapInfo* UTReplicatedMapInfo = UTGameState->MapVoteList[Idx];
			TSharedPtr<FJsonObject> MapInfoJson = GetMapInfoJSON(UTReplicatedMapInfo);
			MapInfosJson.Add(MakeShareable(new FJsonValueObject(MapInfoJson)));
		}
	}

	InstanceInfoJson->SetArrayField(TEXT("MapList"), MapInfosJson);

	// TODO ruleset for lobby
	//TSharedPtr<FJsonObject> CurrentRulesetJson = GetGameRulesetJSON(AvailableMatch->CurrentRuleset);
	//MatchJson->SetObjectField(TEXT("Ruleset"), CurrentRulesetJson);


	return InstanceInfoJson;
}


TSharedPtr<FJsonObject> GetGameInfoJSON()
{
	TSharedPtr<FJsonObject> ServerInfoJson = MakeShareable(new FJsonObject);

	AUTBaseGameMode* BaseGameMode;
	BaseGameMode = Cast<AUTBaseGameMode>(GWorld->GetAuthGameMode());

	AUTLobbyGameMode* LobbyGameMode;
	LobbyGameMode = GWorld->GetAuthGameMode<AUTLobbyGameMode>();

	AUTGameMode* UTGameMode;
	UTGameMode = Cast<AUTGameMode>(GWorld->GetAuthGameMode());

	bool isLobbyData = BaseGameMode && BaseGameMode->IsLobbyServer() && LobbyGameMode;
	bool isDediData = BaseGameMode && !BaseGameMode->IsLobbyServer() && UTGameMode;

	if (BaseGameMode) {

		ServerInfoJson->SetBoolField(TEXT("IsLobbyServer"), isLobbyData);

		if (isLobbyData) {
			ServerInfoJson->SetBoolField(TEXT("IsPassworded"), LobbyGameMode->LobbyPassword.IsEmpty());
			ServerInfoJson->SetNumberField(TEXT("NumPlayers"), LobbyGameMode->GetNumPlayers());
			ServerInfoJson->SetNumberField(TEXT("NumMatches"), LobbyGameMode->GetNumMatches());
		}
		else if (isDediData) {
			ServerInfoJson->SetBoolField(TEXT("IsPassworded"), UTGameMode->ServerPassword.IsEmpty());
			ServerInfoJson->SetNumberField(TEXT("NumPlayers"), UTGameMode->GetNumPlayers());
			ServerInfoJson->SetNumberField(TEXT("NumMatches"), 1);
		}


		TArray<TSharedPtr<FJsonValue>> GameInstancesJson;

		// one or multiple instances for lobby instanced servers
		if (isLobbyData) {
			AUTLobbyGameState* UTLobbyGameState = LobbyGameMode->UTLobbyGameState;

			if (UTLobbyGameState) {
				for (int32 i = 0; i < UTLobbyGameState->GameInstances.Num(); i++)
				{
					TSharedPtr<FJsonObject> MatchJson = GetInstanceInfoJSON(UTLobbyGameState->GameInstances[i].MatchInfo, NULL);
					MatchJson->SetNumberField("InstancePort", UTLobbyGameState->GameInstances[i].InstancePort);
					MatchJson->SetBoolField("IsDataFromDedi", false);
					GameInstancesJson.Add(MakeShareable(new FJsonValueObject(MatchJson)));
				}
			}
		}
		// always one instance for dedi server
		else if (isDediData) {
			TSharedPtr<FJsonObject> MatchJson = GetInstanceInfoJSON(NULL, UTGameMode);
			MatchJson->SetNumberField("InstancePort", 0); // FIXME
			MatchJson->SetBoolField("IsDataFromDedi", true);
			GameInstancesJson.Add(MakeShareable(new FJsonValueObject(MatchJson)));
		}

		ServerInfoJson->SetArrayField(TEXT("GameInstances"), GameInstancesJson);
	}


	return ServerInfoJson;
}
