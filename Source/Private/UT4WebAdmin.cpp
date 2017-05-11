#include "UT4WebAdmin.h"
#include "Base64.h"

UUT4WebAdmin::UUT4WebAdmin(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{

}

void UUT4WebAdmin::Init()
{
	UE_LOG(UT4WebAdmin, Log, TEXT("=================="));
	UE_LOG(UT4WebAdmin, Log, TEXT("UT4WebAdmin Started"));
	UE_LOG(UT4WebAdmin, Log, TEXT("=================="));
}

void UUT4WebAdmin::Tick(float DeltaTime)
{

}

TStatId UUT4WebAdmin::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UUT4WebAdmin, STATGROUP_Tickables);
}
