// Sebastian Lara. All rights reserved.


#include "LobbyPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Blaster/GameMode/HostLobbyGameMode.h"

void ALobbyPlayerController::ClientShowHostWidget_Implementation()
{
	if (IsLocalController() && HostWidgetClass)
	{
		HostWidget = CreateWidget<UUserWidget>(this, HostWidgetClass);
		if (HostWidget)
		{
			HostWidget->AddToViewport();
			bShowMouseCursor = true;
		}
	}
}

void ALobbyPlayerController::ServerStartMatch_Implementation()
{
	if (auto HostLobbyGameMode = GetWorld()->GetAuthGameMode<AHostLobbyGameMode>())
	{
		HostLobbyGameMode->TryStartMatch();
	}
}
