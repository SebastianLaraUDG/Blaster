// Sebastian Lara. All rights reserved.


#include "HostWidget.h"
#include "Components/Button.h"
#include "Blaster/PlayerController/LobbyPlayerController.h"

void UHostWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	StartMatchButton->OnClicked.AddDynamic(this, &UHostWidget::OnStartMatchClick);
}

void UHostWidget::OnStartMatchClick()
{
	if (auto LobbyPlayerController = Cast<ALobbyPlayerController>(GetOwningPlayer()))
	{
		LobbyPlayerController->ServerStartMatch();
	}
}
