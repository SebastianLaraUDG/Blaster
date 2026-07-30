// Sebastian Lara. All rights reserved.


#include "LobbyPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Blaster/GameMode/HostLobbyGameMode.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/BlasterTypes/CharacterCustomization.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/GameInstance/BlasterGameInstance.h"

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

void ALobbyPlayerController::ServerSetCustomization_Implementation(const FCharacterCustomization& NewCustomization)
{
	if (ABlasterPlayerState* PS = GetPlayerState<ABlasterPlayerState>())
	{
		PS->CustomizationData = NewCustomization;
		if (const auto BlasterCharacter = Cast<ABlasterCharacter>(PS->GetPawn()))
		{
			BlasterCharacter->ApplyCustomization();
		}
	}
}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (IsLocalController())
	{
		if (const UBlasterGameInstance* GameInstance = GetGameInstance<UBlasterGameInstance>())
		{
			ServerSetCustomization(GameInstance->PendingCustomization);
		}
	}
}
