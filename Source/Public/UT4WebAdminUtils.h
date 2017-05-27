#pragma once

#include "UT4WebAdmin.h"
#include "UT4WebAdminBaseInfo.h"

APlayerController* getPlayerControllerByUID(const char* PlayerNetIdC);
bool KickPlayerByNetId(const char* PlayerNetIdC, const char* ReasonC, bool isBan);
bool MutePlayerByNetId(const char* PlayerNetIdC, const char* ReasonC);