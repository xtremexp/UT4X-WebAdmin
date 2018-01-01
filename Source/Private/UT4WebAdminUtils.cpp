#pragma once

#include "UT4WebAdmin.h"
#include "UT4WebAdminUtils.h"
#include "UTPlayerController.h"

APlayerController* getPlayerControllerByUID(const char* PlayerNetIdC) {

	if (PlayerNetIdC != NULL && GWorld != NULL) {

		FString PlayerNetId = FString(PlayerNetIdC);

		for (FConstPlayerControllerIterator Iterator = GWorld->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* PlayerController = Iterator->Get();
			if (PlayerController->PlayerState != NULL && PlayerController->PlayerState->UniqueId.IsValid())
			{
				if (*PlayerController->PlayerState->UniqueId.ToString() == PlayerNetId)
				{
					return PlayerController;
				}
			}
		}
	}

	return NULL;
}

// TODO add kickedBy/bannedBy info and return player not found if error
bool KickPlayerByNetId(const char* PlayerNetIdC, const char* ReasonC, bool isBan)
{
	if (GWorld != NULL) {

		AUTBaseGameMode* BaseGameMode = Cast<AUTBaseGameMode>(GWorld->GetAuthGameMode());

		if (BaseGameMode) {
			const FString& playerUidStr = FString(PlayerNetIdC);
			const FString& reasonStr = FString(ReasonC);
			BaseGameMode->RconKick(playerUidStr, isBan, reasonStr);
			/*
			APlayerController* OtherPlayer;

			OtherPlayer = getPlayerControllerByUID(PlayerNetIdC);

			if (OtherPlayer != NULL) {
			// from gamesession.h
			const FText& BanReason = FText::FromString(ReasonC);
			return BaseGameMode->GameSession->BanPlayer(OtherPlayer, BanReason);
			}*/
		}
		
	}

	return false;
}


// TODO add info bannedBy and return player not found if error (player may have left game)
bool MutePlayerByNetId(const char* PlayerNetIdC, const char* ReasonC)
{
	if (GWorld != NULL) {

		APlayerController* OtherPlayer;
		OtherPlayer = getPlayerControllerByUID(PlayerNetIdC);

		// TEXT MUTE
		if (OtherPlayer != NULL && OtherPlayer->PlayerState != NULL) {
			OtherPlayer->GameplayMutePlayer(OtherPlayer->PlayerState->UniqueId);

			FString displayedMessage;
			
			if (ReasonC) {
				FString reason = FString(ReasonC);
				displayedMessage = FString("You have been muted. Reason: " + reason);
			}
			else {
				displayedMessage = FString("You have been muted.");
			}

			OtherPlayer->ClientMessage(displayedMessage, NAME_None, 5.f);
			return true; // no return state from gameplaymute assuming it succeeded
		}

		// TODO VOICE MUTE
	}

	return false;
}

bool IsGameInstanceServer() {
	if (GWorld != NULL) {
		AUTBaseGameMode *BaseGameMode = Cast<AUTBaseGameMode>(GWorld->GetAuthGameMode());
		return BaseGameMode && BaseGameMode->IsGameInstanceServer();
	}

	return false;
}
