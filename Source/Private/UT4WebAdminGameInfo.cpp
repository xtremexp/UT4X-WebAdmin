#include "UT4WebAdmin.h"
#include "UT4WebAdminGameInfo.h"


TSharedPtr<FJsonObject> GetRemotePlayerInfoJSON(FRemotePlayerInfo* RemotePlayerInfo)
{
	TSharedPtr<FJsonObject> RemotePlayerInfoJson = MakeShareable(new FJsonObject);

	RemotePlayerInfoJson->SetStringField(TEXT("PlayerID"), RemotePlayerInfo->PlayerID.ToString());
	RemotePlayerInfoJson->SetBoolField(TEXT("bIsSpectator"), RemotePlayerInfo->bIsSpectator);
	RemotePlayerInfoJson->SetStringField(TEXT("PlayerName"), RemotePlayerInfo->PlayerName);
	RemotePlayerInfoJson->SetNumberField(TEXT("PlayerScore"), RemotePlayerInfo->PlayerScore);
	RemotePlayerInfoJson->SetNumberField(TEXT("RankCheck"), RemotePlayerInfo->RankCheck);
	RemotePlayerInfoJson->SetNumberField(TEXT("XPLevel"), RemotePlayerInfo->XPLevel);
	RemotePlayerInfoJson->SetNumberField(TEXT("TeamNum"), RemotePlayerInfo->TeamNum); // 0 = Red, 1 = Blue
	RemotePlayerInfoJson->SetStringField(TEXT("Avatar"), RemotePlayerInfo->Avatar.ToString());

	return RemotePlayerInfoJson;
}

// FRemotePlayerInfo

TSharedPtr<FJsonObject> GetMapInfoJSON(TWeakObjectPtr<AUTReplicatedMapInfo> MatchInfo)
{
	TSharedPtr<FJsonObject> MapInfoJson = MakeShareable(new FJsonObject);

	if (MatchInfo != NULL) {
		MapInfoJson->SetStringField(TEXT("Title"), MatchInfo->Title);
		MapInfoJson->SetStringField(TEXT("Author"), MatchInfo->Author);
		MapInfoJson->SetStringField(TEXT("Title"), MatchInfo->Title);
		MapInfoJson->SetStringField(TEXT("Description"), MatchInfo->Description);
		MapInfoJson->SetStringField(TEXT("MapScreenshotReference"), MatchInfo->MapScreenshotReference);
	}

	return MapInfoJson;
}

TSharedPtr<FJsonObject> GetLobbyMatchInfoJSON(AUTLobbyMatchInfo* AvailableMatch, uint32 InstancePort)
{
	TSharedPtr<FJsonObject> MatchJson = MakeShareable(new FJsonObject);

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

	return MatchJson;
}

TSharedPtr<FJsonObject> GetPlayerInfoJSON(TWeakObjectPtr<AUTLobbyPlayerState> PlayerState)
{
	TSharedPtr<FJsonObject> PlayerJson = MakeShareable(new FJsonObject);

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
			JsonObject->SetStringField(TEXT("ServerType"), "Lobby");

			if (LobbyGameMode)
			{
				// LobbyPassword
				TSharedPtr<FJsonObject> LobbyInfoJson = MakeShareable(new FJsonObject);
				LobbyInfoJson->SetStringField(TEXT("LobbyPassword"), LobbyGameMode->LobbyPassword);
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


				JsonObject->SetObjectField(TEXT("LobbyInfo"), LobbyInfoJson);
			}
		}
		// Either dedicated server or lobby instanced servers
		else {
			JsonObject->SetStringField(TEXT("ServerType"), "Dedi");
		}
	}


	return JsonObject;
}
