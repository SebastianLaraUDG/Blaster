// Sebastian Lara. All rights reserved.


#include "MainMenu.h"

#include "MultiplayerSessionsSubsystem.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"

bool UMainMenu::Initialize()
{
	if (!Super::Initialize()) return false;
	
	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &ThisClass::OnStartClicked);
	}
	
	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &ThisClass::OnQuitClicked);
	}
	return true;
}

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnOnlineSubsystemNotAvailable.AddDynamic(this, &ThisClass::OnOnlineSubsystemNotAvailable);
	}
	if (OnlineSubsystemWarningText)
	{
		OnlineSubsystemWarningText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainMenu::NativeDestruct()
{
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnOnlineSubsystemNotAvailable.RemoveDynamic(this, &ThisClass::OnOnlineSubsystemNotAvailable);
	}
	Super::NativeDestruct();
}

void UMainMenu::OnStartClicked()
{
	// Check for online subsystem.
	if (MultiplayerSessionsSubsystem)
	{
		if (!MultiplayerSessionsSubsystem->IsOnlineSubsystemAvailable())
		{
#if WITH_EDITOR
			if (GEngine)
			{
				const FString DebugString = FString::Printf(TEXT("ERROR: Online subsystem is not available on this platform. Message from %s. This message is EDITOR ONLY."), *GetClass()->GetName());
				GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Red, DebugString);
			}
#endif
			return;
		}
	}
	
	// Check for level asset.
	if (Level.IsNull())
	{
#if WITH_EDITOR
		const FString DebugString = FString::Printf(TEXT("ERROR: Level asset is not set in %s. This message is EDITOR ONLY."), *GetNameSafe(this));	
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Red, DebugString);
#endif
		return;
	}
	// Attempt to load level if it is set.
	if (const auto World = GetWorld())
	{
		World->ServerTravel(Level.ToSoftObjectPath().GetLongPackageName());
	}
}

void UMainMenu::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}

void UMainMenu::OnOnlineSubsystemNotAvailable()
{
	if (OnlineSubsystemWarningText)
	{
		OnlineSubsystemWarningText->SetText(
		FText::FromString(TEXT("No online subsystem available. Make sure Steam is running."))
		);
		OnlineSubsystemWarningText->SetVisibility(ESlateVisibility::Visible);
	}
}
