#pragma once

#include "Core.h"
#include "UnrealTournament.h"

#include "UT4WebAdmin.generated.h"

// Log messages
DEFINE_LOG_CATEGORY_STATIC(UT4WebAdmin, Log, All)

class AUT4WebAdmin : public UObject, public FTickableGameObject
{
	GENERATED_UCLASS_BODY()

	void Init();
};