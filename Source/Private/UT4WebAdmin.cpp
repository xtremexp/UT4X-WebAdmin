#include "UT4WebAdmin.h"
#include "UT4WebAdminHttpServer.h"

AUT4WebAdmin::AUT4WebAdmin(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	_HttpServer = nullptr;
	GameMode = nullptr;

	DisplayName = FText::FromString("UT4 WebAdmin v0.0.1");
	Author = FText::FromString("Thomas 'XtremeXp/Winter' P.");
	Description = FText::FromString("Web administration mutator for Unreal Tournament 4 (2017) ");

	Start();
}


void AUT4WebAdmin::Start()
{
	// Don't garbage collect me
	SetFlags(RF_MarkAsRootSet);

	// Init Http Server
	if (_HttpServer == NULL) {
		_HttpServer = NewObject<UUT4WebAdminHttpServer>();
		// http server started in UUT4WebAdminHttpServer.tick() until GWorld is available
		// to get GWorld.GetAuthMode
	}
	
	
	// Init SQLite DB
	if (_SQLiteServer == NULL) {
		_SQLiteServer = NewObject<UUT4WebAdminSQLite>();
		_SQLiteServer->Start();
	}
}



void AUT4WebAdmin::Stop()
{
	if (_HttpServer) {
		_HttpServer->Stop();
	}

	if (_SQLiteServer) {
		_SQLiteServer->Stop();
	}
}

void AUT4WebAdmin::PostPlayerInit_Implementation(AController* C) {

	if (C && C->PlayerState && !C->PlayerState->bIsABot) {
		UE_LOG(UT4WebAdmin, Log, TEXT(" %s joined the unreal world."), *C->PlayerState->PlayerName);
	}

	Super::PostPlayerInit_Implementation(C);
}

/* Used to generate chat log history */
bool AUT4WebAdmin::AllowTextMessage_Implementation(FString& Msg, bool bIsTeamMessage, AUTBasePlayerController* Sender) {

	if (Sender && Sender->PlayerState && !Sender->PlayerState->bIsABot) {
		UE_LOG(UT4WebAdmin, Log, TEXT(" Message from %s : %s"), *Sender->PlayerState->PlayerName, *Msg);

		if (_SQLiteServer) {
			AUTPlayerState* UTPlayerState = Cast<AUTPlayerState>(Sender->PlayerState);
			_SQLiteServer->SaveChatMessage(UTPlayerState->PlayerName, UTPlayerState->UniqueId, UTPlayerState->GetTeamNum(), *Msg);
		}
	}

	return Super::AllowTextMessage_Implementation(Msg, bIsTeamMessage, Sender);
}


void AUT4WebAdmin::NotifyLogout_Implementation(AController* C) {

	if (C && C->PlayerState && !C->PlayerState->bIsABot) {
		UE_LOG(UT4WebAdmin, Log, TEXT(" %s left the unreal world."), *C->PlayerState->PlayerName);
	}

	Super::NotifyLogout_Implementation(C);
}

void AUT4WebAdmin::ModifyLogin_Implementation(UPARAM(ref) FString& Portal, UPARAM(ref) FString& Options) {

	// TODO somebody is connecting to the server
	UE_LOG(UT4WebAdmin, Log, TEXT(" ModifyLogin: %s %s"), *Portal, *Options);

	Super::ModifyLogin_Implementation(Portal, Options);
}

void AUT4WebAdmin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_HttpServer) {
		_HttpServer->Tick(DeltaTime);
	}
}
