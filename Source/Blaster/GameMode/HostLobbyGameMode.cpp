// Sebastian Lara. All rights reserved.


#include "HostLobbyGameMode.h"

#include "Blaster/PlayerController/LobbyPlayerController.h"
#include "GameFramework/GameStateBase.h"

void AHostLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (bHasShownHostWidget || !NewPlayer->IsLocalController()) return;
	
	if (auto LobbyPlayerController = Cast<ALobbyPlayerController>(NewPlayer))
	{
		LobbyPlayerController->ClientShowHostWidget();
	}
	
	bHasShownHostWidget = true;
}

void AHostLobbyGameMode::TryStartMatch()
{
	if (Level.IsNull())
	{
		UE_LOG(LogGameMode, Error, TEXT("ERROR: Level is null. %s"), *GetNameSafe(this))
		return;
	}
	
	const int32 Players = GetWorld()->GetGameState()->PlayerArray.Num();
	if (Players < 2)
	{
		UE_LOG(LogGameMode, Display, TEXT("ERROR: Match should contain at least 2 players. NOT ENOUGH PLAYERS. %s"), *GetNameSafe(this))
		return;
	}
	
	bUseSeamlessTravel = true;
	if (auto World = GetWorld())
	{
		World->ServerTravel(
			FString::Printf(TEXT("%s?listen"),
			                *Level.ToSoftObjectPath().GetLongPackageName()));
	}
}
