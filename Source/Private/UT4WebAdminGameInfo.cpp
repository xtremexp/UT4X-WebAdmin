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

TSharedPtr<FJsonObject> GetMatchUpdateJSON(FMatchUpdate* FMatchUpdate)
{
	TSharedPtr<FJsonObject> MatchUpdateJson = MakeShareable(new FJsonObject);

	if (FMatchUpdate) {
		MatchUpdateJson->SetNumberField(TEXT("TimeLimit"), FMatchUpdate->TimeLimit);
		MatchUpdateJson->SetNumberField(TEXT("GoalScore"), FMatchUpdate->GoalScore);
		MatchUpdateJson->SetNumberField(TEXT("GameTime"), FMatchUpdate->GameTime);
		MatchUpdateJson->SetNumberField(TEXT("TimeLimit"), FMatchUpdate->TimeLimit);
		MatchUpdateJson->SetBoolField(TEXT("bMatchHasBegun"), FMatchUpdate->bMatchHasBegun);
		MatchUpdateJson->SetBoolField(TEXT("bMatchHasEnded"), FMatchUpdate->bMatchHasEnded);
		MatchUpdateJson->SetStringField(TEXT("MatchState"), FMatchUpdate->MatchState.ToString());
		

		TArray<TSharedPtr<FJsonValue>> TeamScoresJson;

		for (int32 Idx = 0; Idx < FMatchUpdate->TeamScores.Num(); Idx++)
		{
			TSharedPtr<FJsonObject> TeamScoreJson = MakeShareable(new FJsonObject);
			TeamScoreJson->SetNumberField(FString::FromInt(Idx), FMatchUpdate->TeamScores[Idx]);

			TeamScoresJson.Add(MakeShareable(new FJsonValueObject(TeamScoreJson)));
		}

		MatchUpdateJson->SetArrayField(TEXT("TeamScores"), TeamScoresJson);
	}

	return MatchUpdateJson;
}

TSharedPtr<FJsonObject> GetRemotePlayerInfoJSON(FRemotePlayerInfo* RemotePlayerInfo)
{
	TSharedPtr<FJsonObject> RemotePlayerInfoJson = MakeShareable(new FJsonObject);

	if (RemotePlayerInfo) {
		RemotePlayerInfoJson->SetStringField(TEXT("PlayerID"), RemotePlayerInfo->PlayerID.ToString());
		RemotePlayerInfoJson->SetBoolField(TEXT("bIsSpectator"), RemotePlayerInfo->bIsSpectator);
		RemotePlayerInfoJson->SetStringField(TEXT("PlayerName"), RemotePlayerInfo->PlayerName);
		RemotePlayerInfoJson->SetNumberField(TEXT("PlayerScore"), RemotePlayerInfo->PlayerScore);
		RemotePlayerInfoJson->SetNumberField(TEXT("RankCheck"), RemotePlayerInfo->RankCheck);
		RemotePlayerInfoJson->SetNumberField(TEXT("XPLevel"), RemotePlayerInfo->XPLevel);
		RemotePlayerInfoJson->SetNumberField(TEXT("TeamNum"), RemotePlayerInfo->TeamNum); // 0 = Red, 1 = Blue
		RemotePlayerInfoJson->SetStringField(TEXT("Avatar"), RemotePlayerInfo->Avatar.ToString());
	}

	return RemotePlayerInfoJson;
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

// Fields must be the same as getLobbyMatchInfo
TSharedPtr<FJsonObject> GetDediMatchInfoJSON(AUTGameMode* UTGameMode)
{
	TSharedPtr<FJsonObject> MatchJson = MakeShareable(new FJsonObject);

	if (UTGameMode != NULL) {

		AUTGameState* UTGameState = UTGameMode->UTGameState;

		/*
		MatchJson->SetNumberField(TEXT("InstancePort"), 0);
		MatchJson->SetStringField(TEXT("CurrentState"), UTGameState->CurrentState.ToString());
		MatchJson->SetNumberField(TEXT("bPrivateMatch"), UTGameMode->bPrivateMatch);
		MatchJson->SetNumberField(TEXT("bSpectatable"), UTGameState->bSpectatable);
		MatchJson->SetNumberField(TEXT("bJoinAnytime"), UTGameState->bJoinAnytime);
		MatchJson->SetNumberField(TEXT("bRankLocked"), UTGameMode->bRankLocked);
		MatchJson->SetNumberField(TEXT("GameInstanceID"), UTGameState->GameInstanceID);
		MatchJson->SetStringField(TEXT("GameInstanceGUID"), UTGameState->GameInstanceGUID);
		MatchJson->SetNumberField(TEXT("NumPlayersInMatch"), UTGameState->NumPlayersInMatch());
		MatchJson->SetNumberField(TEXT("NumSpectatorsInMatch"), UTGameState->NumSpectatorsInMatch());
		MatchJson->SetNumberField(TEXT("RankCheck"), UTGameMode->RankCheck); // average rank
		MatchJson->SetNumberField(TEXT("InstanceLaunchTime"), UTGameState->LaunchTime);
		MatchJson->SetStringField(TEXT("CustomGameName"), UTGameState->ServerName);

		TSharedPtr<FJsonObject> CurrentRulesetJson = GetGameRulesetJSON(UTGameState->CurrentRuleset);
		MatchJson->SetObjectField(TEXT("Ruleset"), CurrentRulesetJson);
		*/
	}

	return MatchJson;
}

TSharedPtr<FJsonObject> GetLobbyMatchInfoJSON(AUTLobbyMatchInfo* AvailableMatch, uint32 InstancePort)
{
	TSharedPtr<FJsonObject> MatchJson = MakeShareable(new FJsonObject);

	if (AvailableMatch != NULL) {
		// will be used to get full game info from http://<serverhost>:<InstancePort+100>/gameinfo url
		MatchJson->SetNumberField(TEXT("InstancePort"), InstancePort);
		MatchJson->SetStringField(TEXT("CurrentState"), AvailableMatch->CurrentState.ToString());
		MatchJson->SetNumberField(TEXT("bPrivateMatch"), AvailableMatch->bPrivateMatch);
		MatchJson->SetNumberField(TEXT("bSpectatable"), AvailableMatch->bSpectatable);
		MatchJson->SetNumberField(TEXT("bJoinAnytime"), AvailableMatch->bJoinAnytime);
		MatchJson->SetNumberField(TEXT("bRankLocked"), AvailableMatch->bRankLocked);
		MatchJson->SetNumberField(TEXT("GameInstanceID"), AvailableMatch->GameInstanceID);
		MatchJson->SetStringField(TEXT("GameInstanceGUID"), AvailableMatch->GameInstanceGUID);
		MatchJson->SetNumberField(TEXT("NumPlayersInMatch"), AvailableMatch->NumPlayersInMatch());
		MatchJson->SetNumberField(TEXT("NumSpectatorsInMatch"), AvailableMatch->NumSpectatorsInMatch());
		MatchJson->SetNumberField(TEXT("RankCheck"), AvailableMatch->RankCheck); // average rank
		MatchJson->SetNumberField(TEXT("InstanceLaunchTime"), AvailableMatch->InstanceLaunchTime);
		MatchJson->SetStringField(TEXT("CustomGameName"), AvailableMatch->CustomGameName);

		TSharedPtr<FJsonObject> CurrentRulesetJson = GetGameRulesetJSON(AvailableMatch->CurrentRuleset);
		MatchJson->SetObjectField(TEXT("Ruleset"), CurrentRulesetJson);

		TSharedPtr<FJsonObject> MatchUpdateJson = GetMatchUpdateJSON(&AvailableMatch->MatchUpdate);
		MatchJson->SetObjectField(TEXT("MatchUpdate"), MatchUpdateJson);

		TSharedPtr<FJsonObject> MapInfoJson = GetMapInfoJSON(AvailableMatch->InitialMapInfo);
		MatchJson->SetObjectField(TEXT("MapInfo"), MapInfoJson);

		// Players Info
		TArray<TSharedPtr<FJsonValue>> AvailablePlayersJson;
		for (int32 PlayerIdx = 0; PlayerIdx < AvailableMatch->PlayersInMatchInstance.Num(); PlayerIdx++)
		{
			TSharedPtr<FJsonObject> PlayerJson = GetRemotePlayerInfoJSON(&AvailableMatch->PlayersInMatchInstance[PlayerIdx]);
			AvailablePlayersJson.Add(MakeShareable(new FJsonValueObject(PlayerJson)));
		}

		MatchJson->SetArrayField(TEXT("AvailablePlayers"), AvailablePlayersJson);
	}

	return MatchJson;
}

TSharedPtr<FJsonObject> GetPlayerInfoJSON(AUTPlayerState* PlayerState)
{
	TSharedPtr<FJsonObject> PlayerJson = MakeShareable(new FJsonObject);

	if (PlayerState != NULL) {
		PlayerJson->SetStringField(TEXT("PlayerName"), PlayerState->PlayerName);
		PlayerJson->SetNumberField(TEXT("Kills"), PlayerState->Kills);
		PlayerJson->SetNumberField(TEXT("KillAssists"), PlayerState->KillAssists);
		PlayerJson->SetNumberField(TEXT("DamageDone"), PlayerState->DamageDone);
		PlayerJson->SetNumberField(TEXT("Deaths"), PlayerState->Deaths);
		PlayerJson->SetNumberField(TEXT("FlagCaptures"), PlayerState->FlagCaptures);
		PlayerJson->SetNumberField(TEXT("FlagReturns"), PlayerState->FlagReturns);
		PlayerJson->SetNumberField(TEXT("Assists"), PlayerState->Assists);
		PlayerJson->SetBoolField(TEXT("bIsRconAdmin"), PlayerState->bIsRconAdmin);
		PlayerJson->SetNumberField(TEXT("PartySize"), PlayerState->PartySize);
		PlayerJson->SetStringField(TEXT("PartyLeader"), PlayerState->PartyLeader);
		PlayerJson->SetStringField(TEXT("ClanName"), PlayerState->ClanName);
		PlayerJson->SetNumberField(TEXT("ElapsedTime"), PlayerState->ElapsedTime);
		PlayerJson->SetNumberField(TEXT("XP"), PlayerState->GetPrevXP());
		PlayerJson->SetStringField(TEXT("CountryFlag"), PlayerState->CountryFlag.ToString());
		PlayerJson->SetStringField(TEXT("EpicAccountName"), PlayerState->EpicAccountName);
		PlayerJson->SetStringField(TEXT("ClampedName"), PlayerState->ClampedName);

		// RANKS
		PlayerJson->SetNumberField(TEXT("DuelRank"), PlayerState->DuelRank);
		PlayerJson->SetNumberField(TEXT("CTFRank"), PlayerState->CTFRank);
		PlayerJson->SetNumberField(TEXT("TDMRank"), PlayerState->TDMRank);
		PlayerJson->SetNumberField(TEXT("DMRank"), PlayerState->DMRank);
		PlayerJson->SetNumberField(TEXT("ShowdownRank"), PlayerState->ShowdownRank);
		PlayerJson->SetNumberField(TEXT("FlagRunRank"), PlayerState->FlagRunRank);
		PlayerJson->SetNumberField(TEXT("RankedDuelRank"), PlayerState->RankedDuelRank);
		PlayerJson->SetNumberField(TEXT("RankedCTFRank"), PlayerState->RankedCTFRank);
		PlayerJson->SetNumberField(TEXT("RankedShowdownRank"), PlayerState->RankedShowdownRank);
		PlayerJson->SetNumberField(TEXT("RankedFlagRunRank"), PlayerState->RankedFlagRunRank);

		PlayerJson->SetNumberField(TEXT("KickCount"), PlayerState->KickCount);
		PlayerJson->SetNumberField(TEXT("LastActiveTime"), PlayerState->LastActiveTime);

		PlayerJson->SetNumberField(TEXT("Ping"), PlayerState->Ping);
		PlayerJson->SetStringField(TEXT("SavedNetworkAddress"), PlayerState->SavedNetworkAddress);
	}

	return PlayerJson;
}

TSharedPtr<FJsonObject> GetGameInfoJSON()
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	// Get a reference of any object using the UTBaseGameMode
	AUTBaseGameMode* BaseGameMode;
	BaseGameMode = Cast<AUTBaseGameMode>(GWorld->GetAuthGameMode());

	if (BaseGameMode) {

		if (BaseGameMode->IsLobbyServer())
		{
			AUTLobbyGameMode* LobbyGameMode;
			LobbyGameMode = GWorld->GetAuthGameMode<AUTLobbyGameMode>();
			JsonObject->SetBoolField(TEXT("IsLobbyServer"), true);

			if (LobbyGameMode)
			{
				TSharedPtr<FJsonObject> LobbyInfoJson = MakeShareable(new FJsonObject);
				LobbyInfoJson->SetBoolField(TEXT("IsPassworded"), LobbyGameMode->LobbyPassword.IsEmpty());
				LobbyInfoJson->SetNumberField(TEXT("NumPlayers"), LobbyGameMode->GetNumPlayers());
				LobbyInfoJson->SetNumberField(TEXT("NumMatches"), LobbyGameMode->GetNumMatches());

				AUTLobbyGameState* UTLobbyGameState = LobbyGameMode->UTLobbyGameState;

				TArray<TSharedPtr<FJsonValue>> GameInstancesJson;
				for (int32 i = 0; i < UTLobbyGameState->GameInstances.Num(); i++)
				{
					TSharedPtr<FJsonObject> MatchJson = GetLobbyMatchInfoJSON(UTLobbyGameState->GameInstances[i].MatchInfo, UTLobbyGameState->GameInstances[i].InstancePort);
					GameInstancesJson.Add(MakeShareable(new FJsonValueObject(MatchJson)));
				}

				LobbyInfoJson->SetArrayField(TEXT("GameInstances"), GameInstancesJson);


				JsonObject->SetObjectField(TEXT("ServerInfo"), LobbyInfoJson);
			}
		}
		// Either dedicated server or lobby instanced servers
		else {
			JsonObject->SetBoolField(TEXT("IsLobbyServer"), false);

			AUTGameMode* UTGameMode = Cast<AUTGameMode>(GWorld->GetAuthGameMode());

			if (UTGameMode) {
				TSharedPtr<FJsonObject> DediInfoJson = MakeShareable(new FJsonObject);
				DediInfoJson->SetBoolField(TEXT("IsPassworded"), UTGameMode->ServerPassword.IsEmpty());
				DediInfoJson->SetNumberField(TEXT("NumPlayers"), UTGameMode->GetNumPlayers());
				DediInfoJson->SetNumberField(TEXT("NumMatches"), 1);

				TArray<TSharedPtr<FJsonValue>> GameInstancesJson;
				TSharedPtr<FJsonObject> MatchJson = GetDediMatchInfoJSON(UTGameMode);
				GameInstancesJson.Add(MakeShareable(new FJsonValueObject(MatchJson)));

				DediInfoJson->SetArrayField(TEXT("GameInstances"), GameInstancesJson);

				JsonObject->SetObjectField(TEXT("ServerInfo"), DediInfoJson);
			}
		}
	}


	return JsonObject;
}
