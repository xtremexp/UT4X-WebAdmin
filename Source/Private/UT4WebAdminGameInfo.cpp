#include "UT4WebAdmin.h"
#include "UT4WebAdminGameInfo.h"


TSharedPtr<FJsonObject> GetChatInfoJSON(UUT4WebAdminSQLite* _SQLiteServer)
{

	if (_SQLiteServer != NULL) {
		TArray<FChatRow> ChatRows;
		_SQLiteServer->GetChatMessages(ChatRows);
		return GetChatMessagesJSON(ChatRows);
	}
	else {
		return MakeShareable(new FJsonObject);
	}
}


TSharedPtr<FJsonObject> GetChatMessagesJSON(TArray<FChatRow>& ChatRows)
{
	TSharedPtr<FJsonObject> ChatMessages = MakeShareable(new FJsonObject);

	TArray<TSharedPtr<FJsonValue>> ChatRowsJson;

	for (int32 ChatIdx = 0; ChatIdx < ChatRows.Num(); ChatIdx++)
	{
		FChatRow ChatRow = ChatRows[ChatIdx];
		TSharedPtr<FJsonObject> ChatRowJson = MakeShareable(new FJsonObject);
		ChatRowJson->SetStringField(TEXT("Message"), ChatRow.Message);
		ChatRowJson->SetStringField(TEXT("SenderName"), ChatRow.SenderName);
		ChatRowJson->SetNumberField(TEXT("SenderTeamNum"), ChatRow.SenderTeamNum);
		ChatRowJson->SetStringField(TEXT("SenderUidStr"), ChatRow.SenderUidStr);
		ChatRowJson->SetStringField(TEXT("Time"), ChatRow.Time); // Time as string in ISO8601 format
		ChatRowJson->SetStringField(TEXT("SessionName"), ChatRow.SessionName); // TODO might be instance UID of lobby instance name

		ChatRowsJson.Add(MakeShareable(new FJsonValueObject(ChatRowJson)));
	}

	ChatMessages->SetArrayField(TEXT("ChatRows"), ChatRowsJson);

	return ChatMessages;
}

// WeakObjectPtr<AUTReplicatedGameRuleset>

TSharedPtr<FJsonObject> GetGameRulesetJSON(TWeakObjectPtr<AUTReplicatedGameRuleset> GameRuleset) 
{
	TSharedPtr<FJsonObject> RulesetJson = MakeShareable(new FJsonObject);

	if (GameRuleset != NULL) {
		RulesetJson->SetStringField(TEXT("Title"), GameRuleset->Data.Title);
		RulesetJson->SetStringField(TEXT("Description"), GameRuleset->Data.Description);
		RulesetJson->SetNumberField(TEXT("MaxPlayers"), GameRuleset->Data.MaxPlayers);
		//RulesetJson->SetStringField(TEXT("Title"), GameRuleset->Data.Title);
		//RulesetJson->SetStringField(TEXT("Description"), GameRuleset->Data.Description);
		//RulesetJson->SetNumberField(TEXT("MaxPlayers"), GameRuleset->Data.MaxPlayers);

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

TSharedPtr<FJsonObject> GetMapAssetJSON(const FAssetData& MapAsset)
{
	TSharedPtr<FJsonObject> MapInfoJson = MakeShareable(new FJsonObject);

	if (MapAsset != NULL) {
		FText LocDesc = FText::GetEmpty();
		
		static FName NAME_MapInfo_MapFileSize(TEXT("MapFileSize"));

		const FString* Title = MapAsset.TagsAndValues.Find(NAME_MapInfo_Title);
		const FString* Author = MapAsset.TagsAndValues.Find(NAME_MapInfo_Author);
		const FString* Screenshot = MapAsset.TagsAndValues.Find(NAME_MapInfo_ScreenshotReference);
		const FString* Description = MapAsset.TagsAndValues.Find(NAME_MapInfo_Description);
		const FString* MapScreenShotReference = MapAsset.TagsAndValues.Find(NAME_MapInfo_ScreenshotReference);
		const FString* OptimalPlayerCountStr = MapAsset.TagsAndValues.Find(NAME_MapInfo_OptimalPlayerCount);
		const FString* OptimalTeamPlayerCountStr = MapAsset.TagsAndValues.Find(NAME_MapInfo_OptimalTeamPlayerCount);
		const FString* MapFileSize = MapAsset.TagsAndValues.Find(NAME_MapInfo_MapFileSize);

		int32 OptimalPlayerCount = 6;
		if (OptimalPlayerCountStr != NULL)
		{
			OptimalPlayerCount = FCString::Atoi(**OptimalPlayerCountStr);
		}

		int32 OptimalTeamPlayerCount = 10;
		if (OptimalTeamPlayerCountStr != NULL)
		{
			OptimalTeamPlayerCount = FCString::Atoi(**OptimalTeamPlayerCountStr);
		}

		if (Description != nullptr)
		{
			FTextStringHelper::ReadFromString(**Description, LocDesc);
		}

		MapInfoJson->SetStringField(TEXT("Title"), (Title != NULL && !Title->IsEmpty()) ? *Title : *MapAsset.AssetName.ToString());
		MapInfoJson->SetStringField(TEXT("Author"), (Author != NULL) ? *Author : FString());
		MapInfoJson->SetStringField(TEXT("MapPackageName"), MapAsset.PackageName.ToString());
		MapInfoJson->SetStringField(TEXT("MapAssetName"), MapAsset.AssetName.ToString());
		MapInfoJson->SetStringField(TEXT("Description"), (!LocDesc.IsEmpty()) ? LocDesc.ToString() : FString());
		MapInfoJson->SetStringField(TEXT("MapScreenshotReference"), (Screenshot != NULL) ? *Screenshot : FString());
		MapInfoJson->SetNumberField(TEXT("OptimalPlayerCount"), OptimalPlayerCount);
		MapInfoJson->SetNumberField(TEXT("OptimalTeamPlayerCount"), OptimalTeamPlayerCount);
		MapInfoJson->SetStringField(TEXT("MapFileSize"), *MapFileSize); // might be useful for user to choose a small map for faster download ...
		// FIX ME get redirect info
		/*
		MapInfoJson->SetStringField(TEXT("RedirectPackageName"), MapInfo->Redirect.PackageName);
		MapInfoJson->SetStringField(TEXT("RedirectPackageFullURL"), MapInfo->Redirect.ToString());
		MapInfoJson->SetStringField(TEXT("RedirectPackageChecksum"), MapInfo->Redirect.PackageChecksum);
		*/

	}

	return MapInfoJson;
}


// FRemotePlayerInfo
TSharedPtr<FJsonObject> GetMapInfoJSON(TWeakObjectPtr<AUTReplicatedMapInfo> MapInfo)
{
	TSharedPtr<FJsonObject> MapInfoJson = MakeShareable(new FJsonObject);

	// Info from map list from lobby
	if (MapInfo != NULL) {
		MapInfoJson->SetStringField(TEXT("Title"), MapInfo->Title);
		MapInfoJson->SetStringField(TEXT("Author"), MapInfo->Author);
		MapInfoJson->SetStringField(TEXT("MapPackageName"), MapInfo->MapPackageName);
		MapInfoJson->SetStringField(TEXT("MapAssetName"), MapInfo->MapAssetName);
		MapInfoJson->SetStringField(TEXT("Description"), MapInfo->Description);
		MapInfoJson->SetStringField(TEXT("MapScreenshotReference"), MapInfo->MapScreenshotReference);
		MapInfoJson->SetNumberField(TEXT("OptimalPlayerCount"), MapInfo->OptimalPlayerCount);
		MapInfoJson->SetNumberField(TEXT("OptimalTeamPlayerCount"), MapInfo->OptimalTeamPlayerCount);

		MapInfoJson->SetStringField(TEXT("RedirectPackageName"), MapInfo->Redirect.PackageName);
		MapInfoJson->SetStringField(TEXT("RedirectPackageFullURL"), MapInfo->Redirect.ToString());
		MapInfoJson->SetStringField(TEXT("RedirectPackageChecksum"), MapInfo->Redirect.PackageChecksum);
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
		InstanceInfoJson->SetNumberField(TEXT("MaxPlayers"), LobbyMatchInfo->DedicatedServerMaxPlayers);

		InstanceInfoJson->SetStringField(TEXT("ServerInstanceGUID"), LobbyMatchInfo->GameInstanceGUID);
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

		// TODO add team scores if team game
	}
	// Data from dedi or instanced lobby server
	else if(UTGameMode) {
		InstanceInfoJson->SetNumberField(TEXT("bPrivateMatch"), UTGameMode->bPrivateMatch);
		InstanceInfoJson->SetNumberField(TEXT("bRankLocked"), UTGameMode->bRankLocked);
		InstanceInfoJson->SetNumberField(TEXT("NumPlayers"), UTGameMode->GetNumPlayers());
		InstanceInfoJson->SetNumberField(TEXT("NumSpectators"), UTGameMode->GetNumSpectators());
		InstanceInfoJson->SetNumberField(TEXT("MaxPlayers"), UTGameMode->DefaultMaxPlayers);

		if (UTGameState) {
			InstanceInfoJson->SetStringField(TEXT("ServerInstanceGUID"), UTGameState->ServerInstanceGUID.ToString());
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
		// TODO add team scores if team game
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
				// Exact ping as float (rounded and compressed in replicated UTPlayerState->Ping) as seen in PlayerState.h
				PlayerJson->SetNumberField(TEXT("Ping"), (int32) UTPlayerState->ExactPing);
				PlayerJson->SetStringField(TEXT("SavedNetworkAddress"), UTPlayerState->SavedNetworkAddress);
				PlayerJson->SetBoolField(TEXT("bIsMuted"), UTPlayerState->bIsMuted);
				PlayerJson->SetBoolField(TEXT("bIsInactive"), UTPlayerState->bIsInactive);
				PlayerJson->SetBoolField(TEXT("bIsMatchHost"), UTPlayerState->bIsMatchHost);
				PlayerJson->SetNumberField(TEXT("RemainingLives"), UTPlayerState->RemainingLives);
				PlayerJson->SetStringField(TEXT("PartyLeader"), UTPlayerState->PartyLeader);
				PlayerJson->SetBoolField(TEXT("bIsMatchHost"), UTPlayerState->bIsMatchHost);
				PlayerJson->SetBoolField(TEXT("bIsWarmingUp"), UTPlayerState->bIsWarmingUp);

				int32 BadgeLevel = 0;
				int32 BadgeSubLevel = 0;
				UTPlayerState->GetBadgeFromELO(UTGameMode, UTGameMode->bRankedSession, BadgeLevel, BadgeSubLevel);
				PlayerJson->SetNumberField(TEXT("BadgeLevel"), BadgeLevel);
				PlayerJson->SetNumberField(TEXT("BadgeSubLevel"), BadgeSubLevel);

				PlayersJson.Add(MakeShareable(new FJsonValueObject(PlayerJson)));
			}
		}
	}

	InstanceInfoJson->SetArrayField(TEXT("Players"), PlayersJson);

	// Map Played
	TSharedPtr<FJsonObject> MapInfoJson = MakeShareable(new FJsonObject);

	// Current map being played
	if (LobbyMatchInfo) {
		MapInfoJson = GetMapInfoJSON(LobbyMatchInfo->InitialMapInfo);
	}
	else if (UTGameMode && UTGameState) {

		AUTReplicatedMapInfo* CurrentMapInfoReplicated = NULL;
		FString currentMapName = GWorld->GetMapName();

		// Get current map info from map vote list
		for (int32 Idx = 0; Idx < UTGameState->MapVoteList.Num(); Idx++)
		{
			if (UTGameState->MapVoteList[Idx] && UTGameState->MapVoteList[Idx]->MapAssetName == currentMapName) {
				CurrentMapInfoReplicated = UTGameState->MapVoteList[Idx];
				break;
			}
		}

		if (CurrentMapInfoReplicated) {
			MapInfoJson = GetMapInfoJSON(CurrentMapInfoReplicated);
		}
		// map not in map list have to load level to get data
		else {
			// TODO cache ? perf might be bad
			UUTGameEngine* UTGameEngine = Cast<UUTGameEngine>(GEngine);

			if (UTGameEngine && !GWorld->GetMapName().IsEmpty()) {
				UUTLevelSummary* LevelSummary = UTGameEngine->LoadLevelSummary(GWorld->GetMapName());
				if (LevelSummary) {
					MapInfoJson->SetStringField(TEXT("Author"), LevelSummary->Author);
					MapInfoJson->SetStringField(TEXT("MapAssetName"), GWorld->GetMapName());
					MapInfoJson->SetStringField(TEXT("Title"), LevelSummary->Title);
					MapInfoJson->SetStringField(TEXT("Description"), LevelSummary->Description.ToString());
					MapInfoJson->SetNumberField(TEXT("OptimalPlayerCount"), LevelSummary->OptimalPlayerCount);
					MapInfoJson->SetNumberField(TEXT("OptimalTeamPlayerCount"), LevelSummary->OptimalTeamPlayerCount);
					// TODO missing redirect info
				}
			}
		}
	}
	/*
	
	*/

	InstanceInfoJson->SetObjectField(TEXT("Map"), MapInfoJson);


	// Map List
	TArray<TSharedPtr<FJsonValue>> MapInfosJson;

	if (LobbyMatchInfo) {
		TWeakObjectPtr<AUTReplicatedGameRuleset> GameRuleset = LobbyMatchInfo->CurrentRuleset;
		// This is map list for current ruleset - It might not contains all maps for current gametype
		// allow switch to any map?
		if (GameRuleset != NULL) {
			for (int32 MapIdx = 0; MapIdx < GameRuleset->MapList.Num(); MapIdx++)
			{
				MapInfoJson = GetMapInfoJSON(GameRuleset->MapList[MapIdx]);
				MapInfosJson.Add(MakeShareable(new FJsonValueObject(MapInfoJson)));
			}
		}
		else {
			// TODO if player has created a custom game then map list is empty !
		}
	}
	else if (UTGameMode && UTGameState) {
		// no mapvote list for dedi?
		// Adapted from UTGameMode.PrepareMapVote()
		if (UTGameState->MapVoteList.Num() == 0) {
			TArray<FString> MapPrefixList;
			MapPrefixList.Add(UTGameMode->GetMapPrefix());
			TArray<FString> MapList;

			// avoid always scanning
			if (AllMaps.Num() == 0) {
				UTGameState->ScanForMaps(MapPrefixList, AllMaps);
			}

			// Now, go through all of the maps 
			// TODO cache
			for (const FAssetData& MapAsset : AllMaps)
			{
				FString PackageName = MapAsset.PackageName.ToString();
				const FString* Title = MapAsset.TagsAndValues.Find(NAME_MapInfo_Title);
				const FString* Screenshot = MapAsset.TagsAndValues.Find(NAME_MapInfo_ScreenshotReference);

				MapInfoJson = GetMapAssetJSON(MapAsset);
				MapInfosJson.Add(MakeShareable(new FJsonValueObject(MapInfoJson)));
			}
		}
		else {
			for (int32 MapIdx = 0; MapIdx < UTGameState->MapVoteList.Num(); MapIdx++)
			{
				AUTReplicatedMapInfo* UTReplicatedMapInfo = UTGameState->MapVoteList[MapIdx];
				MapInfoJson = GetMapInfoJSON(UTReplicatedMapInfo);
				MapInfosJson.Add(MakeShareable(new FJsonValueObject(MapInfoJson)));
			}
		}
	}

	InstanceInfoJson->SetArrayField(TEXT("MapList"), MapInfosJson);

	// Game Info being played - Rulet for lobby game mode info for dedi
	TSharedPtr<FJsonObject> GameInfoJson = MakeShareable(new FJsonObject);

	if (LobbyMatchInfo) {

		TWeakObjectPtr<AUTReplicatedGameRuleset> GameRuleset = LobbyMatchInfo->CurrentRuleset;

		if (GameRuleset != NULL) {
			GameInfoJson->SetStringField(TEXT("Title"), GameRuleset->Data.Title);
			GameInfoJson->SetNumberField(TEXT("MaxPlayers"), GameRuleset->Data.MaxPlayers);
			//GameInfoJson->SetStringField(TEXT("Title"), GameRuleset->Data.Title);
			//GameInfoJson->SetNumberField(TEXT("MaxPlayers"), GameRuleset->Data.MaxPlayers);
		}
	}
	else if (UTGameMode && UTGameState) {
		AGameSession * GameSession = UTGameMode->GameSession;
		GameInfoJson->SetStringField(TEXT("Title"), UTGameMode->DisplayName.ToString());

		if (UTGameMode->GameSession) {
			GameInfoJson->SetNumberField(TEXT("MaxPlayers"), UTGameMode->GameSession->MaxPlayers);
		}
		
	}

	// Mutators being used in current session
	if (UTGameMode && UTGameState) {

		TArray<TSharedPtr<FJsonValue>> MutatorsInfoJson;
		AUTMutator* NextMutator = UTGameMode->BaseMutator;


		while (NextMutator != NULL)
		{
			TSharedPtr<FJsonObject> MutatorInfoJson = MakeShareable(new FJsonObject);
			MutatorInfoJson->SetStringField(TEXT("DisplayName"), NextMutator->DisplayName.ToString());
			MutatorInfoJson->SetStringField(TEXT("Author"), NextMutator->Author.ToString());
			MutatorInfoJson->SetStringField(TEXT("Description"), NextMutator->Description.ToString());
			MutatorsInfoJson.Add(MakeShareable(new FJsonValueObject(MutatorInfoJson)));

			NextMutator = NextMutator->NextMutator;
		}

		InstanceInfoJson->SetArrayField(TEXT("Mutators"), MutatorsInfoJson);
	}


	// Available mutators and gametypes
	// FIXME crashing for unknown reason (?)
	/*
	if (UTGameMode && UTGameState) {

		TArray<TSharedPtr<FJsonValue>> MutatorsAvailableJson;
		TArray<UClass*> MutatorListAvailable;
		TArray<UClass*> AllGametypes;
		UTGameState->GetAvailableGameData(AllGametypes, MutatorListAvailable);

		for (UClass* MutClass : MutatorListAvailable)
		{
			const AUTMutator* Mutator = MutClass->GetDefaultObject<AUTMutator>();
			TSharedPtr<FJsonObject> MutatorAvailableJson = MakeShareable(new FJsonObject);
			MutatorAvailableJson->SetStringField(TEXT("DisplayName"), Mutator->DisplayName.ToString());
			MutatorAvailableJson->SetStringField(TEXT("Author"), Mutator->Author.ToString());
			MutatorAvailableJson->SetStringField(TEXT("Description"), Mutator->Description.ToString());

			MutatorsAvailableJson.Add(MakeShareable(new FJsonValueObject(MutatorAvailableJson)));
		}

		InstanceInfoJson->SetArrayField(TEXT("MutatorsAvailable"), MutatorsAvailableJson);
		TArray<TSharedPtr<FJsonValue>> GametypesAvailableJson;

		for (UClass* GametypeClass : AllGametypes)
		{
			TSharedPtr<FJsonObject> GametypeAvailable = MakeShareable(new FJsonObject);
			GametypeAvailable->SetStringField(TEXT("DisplayName"), GametypeClass->GetName());

			GametypesAvailableJson.Add(MakeShareable(new FJsonValueObject(GametypeAvailable)));
		}

		InstanceInfoJson->SetArrayField(TEXT("GametypesAvailable"), GametypesAvailableJson);
	}*/
	

	

	InstanceInfoJson->SetObjectField(TEXT("GameInfo"), GameInfoJson);

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

		// Players waiting in hub but not in any server instance
		if (isLobbyData) {
			AUTLobbyGameState* UTLobbyGameState = LobbyGameMode->UTLobbyGameState;
			TArray<TSharedPtr<FJsonValue>> PlayersJson;

			for (int32 Idx = 0; Idx < UTLobbyGameState->PlayerArray.Num(); Idx++)
			{
				AUTPlayerState* UTPlayerState = Cast<AUTPlayerState>(UTLobbyGameState->PlayerArray[Idx]);
				TSharedPtr<FJsonObject> PlayerJson = MakeShareable(new FJsonObject);

				// only get data useful for admins to sort/make matchs
				PlayerJson->SetStringField(TEXT("UniqueId"), UTPlayerState->UniqueId.ToString());
				PlayerJson->SetStringField(TEXT("PlayerName"), UTPlayerState->PlayerName);
				PlayerJson->SetBoolField(TEXT("bIsInactive"), UTPlayerState->bIsInactive);
				PlayerJson->SetNumberField(TEXT("XPLevel"), UTPlayerState->GetPrevXP());
				PlayerJson->SetNumberField(TEXT("Ping"), (int32)UTPlayerState->ExactPing);
				PlayerJson->SetStringField(TEXT("SavedNetworkAddress"), UTPlayerState->SavedNetworkAddress);
				PlayersJson.Add(MakeShareable(new FJsonValueObject(PlayerJson)));
			}

			ServerInfoJson->SetArrayField(TEXT("WaitingPlayersInHub"), PlayersJson);
		}

		ServerInfoJson->SetArrayField(TEXT("GameInstances"), GameInstancesJson);
	}


	TSharedPtr<FJsonObject> ServerGlobalInfoJson = MakeShareable(new FJsonObject);
	ServerGlobalInfoJson->SetObjectField(TEXT("data"), ServerInfoJson);

	return ServerGlobalInfoJson;
}
