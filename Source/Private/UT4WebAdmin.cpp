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

	// Init SQLite DB
	// TODO SQLite in separate thread
	if (_SQLiteServer == NULL) {
		_SQLiteServer = NewObject<UUT4WebAdminSQLite>();
		_SQLiteServer->Start();
	}

	// Init Http Server
	if (_HttpServer == NULL) {
		_HttpServer = new UUT4WebAdminHttpServer(8080, _SQLiteServer);// NewObject<UUT4WebAdminHttpServer>(8080, _SQLiteServer);

		/*
		if (_SQLiteServer != NULL) {
			//TSharedPtr<UUT4WebAdminSQLite> fuck(_SQLiteServer);
			_HttpServer->SetSQLiteServer(_SQLiteServer);
		}*/

		// http server started in UUT4WebAdminHttpServer.tick() until GWorld is available
		// to get GWorld.GetAuthMode
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

			FChatRow ChatRow;
			ChatRow.Time = FDateTime::Now().ToIso8601();
			ChatRow.SenderName = UTPlayerState->PlayerName;
			ChatRow.SenderUidStr = UTPlayerState->UniqueId.ToString();
			ChatRow.SenderTeamNum = UTPlayerState->GetTeamNum();
			ChatRow.Message = Msg;

			// save chat for current session game
			ChatRows.Add(ChatRow);

			// save chat in SQL Lite
			_SQLiteServer->SaveChatMessage(ChatRow);
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
		//_HttpServer->Tick(DeltaTime);
	}
}
