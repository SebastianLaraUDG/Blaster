// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

class UUserWidget;

/**
 * Adds a widget which can try to start a match if the player is the host.
 */
UCLASS()
class BLASTER_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> HostWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> HostWidget;
	
	// Create the HostWidget for the listen-server host of the match.
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientShowHostWidget();
	
	// Call TryStartMatch from HostLobbyGameMode
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerStartMatch();
};
