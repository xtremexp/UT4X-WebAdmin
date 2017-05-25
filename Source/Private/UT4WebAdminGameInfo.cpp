#include "UT4WebAdmin.h"
#include "UT4WebAdminGameInfo.h"


TSharedPtr<FJsonObject> GetMatchInfoJSON(AUTLobbyMatchInfo* AvailableMatch)
{
	TSharedPtr<FJsonObject> MatchJson = MakeShareable(new FJsonObject);

	MatchJson->SetStringField(TEXT("CurrentState"), AvailableMatch->CurrentState.ToString());
	MatchJson->SetNumberField(TEXT("bPrivateMatch"), AvailableMatch->bPrivateMatch);
	MatchJson->SetNumberField(TEXT("bSpectatable"), AvailableMatch->bSpectatable);
	MatchJson->SetNumberField(TEXT("bJoinAnytime"), AvailableMatch->bJoinAnytime);
	MatchJson->SetNumberField(TEXT("bRankLocked"), AvailableMatch->bRankLocked);

	// Players Info
	TArray<TSharedPtr<FJsonValue>> AvailablePlayersJson;
	for (int32 PlayerIdx = 0; PlayerIdx < AvailableMatch->Players.Num(); PlayerIdx++)
	{
		TSharedPtr<FJsonObject> PlayerJson = GetPlayerInfoJSON(AvailableMatch->Players[PlayerIdx]);
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

		// GameMode
		/*
		JsonObject->SetBoolField(TEXT("IsMatchInProgress"), BaseGameMode->IsMatchInProgress());
		JsonObject->SetBoolField(TEXT("HasMatchEnded"), BaseGameMode->HasMatchEnded());
		JsonObject->SetNumberField(TEXT("NumTravellingPlayers"), BaseGameMode->NumTravellingPlayers);
		JsonObject->SetStringField(TEXT("NetworkNumber"), BaseGameMode->GetNetworkNumber());
		JsonObject->SetStringField(TEXT("MatchState"), BaseGameMode->GetMatchState().ToString());
		
		// UTBaseGameMode
		JsonObject->SetStringField(TEXT("ServerInstanceID"), BaseGameMode->ServerInstanceID);
		JsonObject->SetStringField(TEXT("ServerInstanceGUID"), BaseGameMode->ServerInstanceGUID.ToString()); // The Unique ID for this game instance.
		JsonObject->SetStringField(TEXT("ContextGUID"), BaseGameMode->ContextGUID.ToString()); // ?
		JsonObject->SetStringField(TEXT("ServerPassword"), BaseGameMode->ServerPassword);
		JsonObject->SetStringField(TEXT("SpectatePassword"), BaseGameMode->SpectatePassword);
		JsonObject->SetBoolField(TEXT("bRequirePassword"), BaseGameMode->bRequirePassword);
		JsonObject->SetStringField(TEXT("DisplayName"), BaseGameMode->DisplayName.ToString());
		JsonObject->SetNumberField(TEXT("MinAllowedRank"), BaseGameMode->MinAllowedRank);
		JsonObject->SetNumberField(TEXT("MaxAllowedRank"), BaseGameMode->MaxAllowedRank);
		JsonObject->SetBoolField(TEXT("bTrainingGround"), BaseGameMode->bTrainingGround);
		JsonObject->SetNumberField(TEXT("NumPlayers"), BaseGameMode->GetNumPlayers());
		JsonObject->SetNumberField(TEXT("NumMatches"), BaseGameMode->GetNumMatches()); // 1 if dedi server else [0, .., X] range for hubs/lobbies
		JsonObject->SetNumberField(TEXT("CurrentPlaylistId"), BaseGameMode->CurrentPlaylistId); // no idea what this is about
		JsonObject->SetBoolField(TEXT("bPrivateMatch"), BaseGameMode->bPrivateMatch);
		JsonObject->SetStringField(TEXT("RankedLeagueName"), BaseGameMode->GetRankedLeagueName()); // always empty for the moment
		JsonObject->SetBoolField(TEXT("SupportsInstantReplay"), BaseGameMode->SupportsInstantReplay());
		JsonObject->SetBoolField(TEXT("bIsLANGame"), BaseGameMode->bIsLANGame);
		*/
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

				TArray<TSharedPtr<FJsonValue>> AvailableMatchJson;
				for (int32 i = 0; i < UTLobbyGameState->AvailableMatches.Num(); i++)
				{
					TSharedPtr<FJsonObject> MatchJson = GetMatchInfoJSON(UTLobbyGameState->AvailableMatches[i]);
					AvailableMatchJson.Add(MakeShareable(new FJsonValueObject(MatchJson)));
				}

				LobbyInfoJson->SetArrayField(TEXT("AvailableMatches"), AvailableMatchJson);


				// JsonObject->SetArrayField(TEXT("Rulesets"),  RulesetJson);
				JsonObject->SetObjectField(TEXT("LobbyInfo"), LobbyInfoJson);
			}
		}
		// TODO
		else {
			JsonObject->SetStringField(TEXT("ServerType"), "Dedi");
		}
	}


	return JsonObject;
}
