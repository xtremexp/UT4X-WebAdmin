#include "UT4WebAdmin.h"


AUT4WebAdmin::AUT4WebAdmin(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UUT4WebAdmin::Init()
{
	UE_LOG(UT4WebAdmin, Log, TEXT("=================="));
	UE_LOG(UT4WebAdmin, Log, TEXT("UT4WebAdmin Started"));
	UE_LOG(UT4WebAdmin, Log, TEXT("=================="));
}
