#pragma once

#include "UT4WebAdmin.h"
#include "Core.h"
#include "UnrealTournament.h"

struct FChatRow
{
	FString SessionName;
	FString SenderName;
	FString SenderUidStr;
	int32 SenderTeamNum;
	FString Message;
	FString Time;
};