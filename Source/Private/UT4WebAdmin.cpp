#include "UT4WebAdmin.h"

UT4WebAdmin::UT4WebAdmin(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	MGServer = nullptr;
	GameMode = nullptr;
}

void UHTTPAdmin::Init()
{
	UE_LOG(HTTPAdmin, Log, TEXT("=================="));
	UE_LOG(HTTPAdmin, Log, TEXT("UT4WebAdmin Started));
	UE_LOG(HTTPAdmin, Log, TEXT("=================="));
}
