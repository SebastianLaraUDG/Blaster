// Sebastian Lara. All rights reserved.


#include "OverheadWidget.h"

#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::SetDisplayText(const FString& TextToDisplay) const
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole LocalRole = InPawn->GetLocalRole();	// GetRemoteRole()
	FString Role;
	
	switch (LocalRole)
	{
	case ROLE_Authority:			Role = FString(("Authority"));				break;
	case ROLE_AutonomousProxy:		Role = FString(("AutonomousProxy"));		break;
	case ROLE_SimulatedProxy:		Role = FString(("SimulatedProxy"));			break;
	case ROLE_None:					Role = FString(("None"));					break;
	}

	// Display local role and Player State Player Name.
	if (UWorld* const World = GetWorld())
	{
		FString StatePlayerName = FString("");
		
		if (APlayerState* const PlayerState = InPawn->GetPlayerState())
		{
			StatePlayerName = PlayerState->GetPlayerName();
		}

		const FString LocalRoleString = FString::Printf(TEXT("Local Role: %s.Player State Name: %s"), *Role, *StatePlayerName);
		SetDisplayText(LocalRoleString);
	}
}

void UOverheadWidget::ShowPlayerName(APawn* InPawn) const
{
	FString StatePlayerName = FString("");
		
	if (const APlayerState* const PlayerState = InPawn->GetPlayerState())
	{
		StatePlayerName = PlayerState->GetPlayerName();
	}
	SetDisplayText(StatePlayerName);
}

void UOverheadWidget::NativeDestruct()
{	
	RemoveFromParent();
	Super::NativeConstruct();
}
