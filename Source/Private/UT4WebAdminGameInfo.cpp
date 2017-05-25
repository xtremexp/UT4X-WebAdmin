#include "UT4WebAdmin.h"
#include "UT4WebAdminGameInfo.h"


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
					AUTLobbyMatchInfo* AvailableMatch = UTLobbyGameState->AvailableMatches[i];
					TSharedPtr<FJsonObject> MatchJson = MakeShareable(new FJsonObject);

					MatchJson->SetStringField(TEXT("CurrentState"), AvailableMatch->CurrentState.ToString());
					MatchJson->SetNumberField(TEXT("bPrivateMatch"), AvailableMatch->bPrivateMatch);
					MatchJson->SetNumberField(TEXT("bSpectatable"), AvailableMatch->bSpectatable);
					MatchJson->SetNumberField(TEXT("bJoinAnytime"), AvailableMatch->bJoinAnytime);
					MatchJson->SetNumberField(TEXT("bRankLocked"), AvailableMatch->bRankLocked);
					
					TArray<TSharedPtr<FJsonValue>> AvailablePlayersJson;
					for (int32 PlayerIdx = 0; PlayerIdx < AvailableMatch->Players.Num(); PlayerIdx++)
					{
						//TWeakObkectPtr<AUTLobbyPlayerState> UTLobbyPlayerState = AvailableMatch->Players[PlayerIdx];

						TSharedPtr<FJsonObject> PlayerJson = MakeShareable(new FJsonObject);
						PlayerJson->SetStringField(TEXT("PlayerName"), AvailableMatch->Players[PlayerIdx]->PlayerName);
						PlayerJson->SetNumberField(TEXT("Kills"), AvailableMatch->Players[PlayerIdx]->Kills);
						PlayerJson->SetNumberField(TEXT("KillAssists"), AvailableMatch->Players[PlayerIdx]->KillAssists);
						PlayerJson->SetNumberField(TEXT("DamageDone"), AvailableMatch->Players[PlayerIdx]->DamageDone);
						PlayerJson->SetNumberField(TEXT("Deaths"), AvailableMatch->Players[PlayerIdx]->Deaths);
						PlayerJson->SetNumberField(TEXT("FlagCaptures"), AvailableMatch->Players[PlayerIdx]->FlagCaptures);
						PlayerJson->SetNumberField(TEXT("FlagReturns"), AvailableMatch->Players[PlayerIdx]->FlagReturns);
						PlayerJson->SetNumberField(TEXT("Assists"), AvailableMatch->Players[PlayerIdx]->Assists);
						PlayerJson->SetBoolField(TEXT("bIsRconAdmin"), AvailableMatch->Players[PlayerIdx]->bIsRconAdmin);
						PlayerJson->SetNumberField(TEXT("PartySize"), AvailableMatch->Players[PlayerIdx]->PartySize);
						PlayerJson->SetStringField(TEXT("PartyLeader"), AvailableMatch->Players[PlayerIdx]->PartyLeader);
						PlayerJson->SetStringField(TEXT("ClanName"), AvailableMatch->Players[PlayerIdx]->ClanName);
						PlayerJson->SetNumberField(TEXT("ElapsedTime"), AvailableMatch->Players[PlayerIdx]->ElapsedTime);
						PlayerJson->SetNumberField(TEXT("XP"), AvailableMatch->Players[PlayerIdx]->GetPrevXP());
						PlayerJson->SetStringField(TEXT("CountryFlag"), AvailableMatch->Players[PlayerIdx]->CountryFlag.ToString());
						PlayerJson->SetStringField(TEXT("EpicAccountName"), AvailableMatch->Players[PlayerIdx]->EpicAccountName);
						PlayerJson->SetStringField(TEXT("ClampedName"), AvailableMatch->Players[PlayerIdx]->ClampedName);
						// RANKS
						PlayerJson->SetNumberField(TEXT("DuelRank"), AvailableMatch->Players[PlayerIdx]->DuelRank);
						PlayerJson->SetNumberField(TEXT("CTFRank"), AvailableMatch->Players[PlayerIdx]->CTFRank);
						PlayerJson->SetNumberField(TEXT("TDMRank"), AvailableMatch->Players[PlayerIdx]->TDMRank);
						PlayerJson->SetNumberField(TEXT("DMRank"), AvailableMatch->Players[PlayerIdx]->DMRank);
						PlayerJson->SetNumberField(TEXT("ShowdownRank"), AvailableMatch->Players[PlayerIdx]->ShowdownRank);
						PlayerJson->SetNumberField(TEXT("FlagRunRank"), AvailableMatch->Players[PlayerIdx]->FlagRunRank);
						PlayerJson->SetNumberField(TEXT("RankedDuelRank"), AvailableMatch->Players[PlayerIdx]->RankedDuelRank);
						PlayerJson->SetNumberField(TEXT("RankedCTFRank"), AvailableMatch->Players[PlayerIdx]->RankedCTFRank);
						PlayerJson->SetNumberField(TEXT("RankedShowdownRank"), AvailableMatch->Players[PlayerIdx]->RankedShowdownRank);
						PlayerJson->SetNumberField(TEXT("RankedFlagRunRank"), AvailableMatch->Players[PlayerIdx]->RankedFlagRunRank);

						PlayerJson->SetNumberField(TEXT("KickCount"), AvailableMatch->Players[PlayerIdx]->KickCount);
						PlayerJson->SetNumberField(TEXT("LastActiveTime"), AvailableMatch->Players[PlayerIdx]->LastActiveTime);
						
						PlayerJson->SetNumberField(TEXT("Ping"), AvailableMatch->Players[PlayerIdx]->Ping);
						PlayerJson->SetStringField(TEXT("SavedNetworkAddress"), AvailableMatch->Players[PlayerIdx]->SavedNetworkAddress);
						

						
						AvailablePlayersJson.Add(MakeShareable(new FJsonValueObject(PlayerJson)));
					}
					
					// Players

					MatchJson->SetArrayField(TEXT("AvailablePlayers"), AvailablePlayersJson);
					
					AvailableMatchJson.Add(MakeShareable(new FJsonValueObject(MatchJson)));
				}

				LobbyInfoJson->SetArrayField(TEXT("AvailableMatches"), AvailableMatchJson);


				// JsonObject->SetArrayField(TEXT("Rulesets"),  RulesetJson);
				JsonObject->SetObjectField(TEXT("LobbyInfo"), LobbyInfoJson);
				
				

				// UTLobbyGameState

				//FString LobbyPassword;
				// TODO get instance data
			}
		}
		// TODO
		else {
			JsonObject->SetStringField(TEXT("ServerType"), "Dedi");
		}
	}


	return JsonObject;
}
