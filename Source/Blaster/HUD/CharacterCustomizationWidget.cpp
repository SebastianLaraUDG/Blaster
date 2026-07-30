// Sebastian Lara. All rights reserved.


#include "CharacterCustomizationWidget.h"

#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Blaster/Actors/CharacterPreviewActor.h"
#include "Components/Button.h"
#include "Blaster/GameInstance/BlasterGameInstance.h"

bool UCharacterCustomizationWidget::Initialize()
{
	if (!Super::Initialize()) return false;
	
	if (HostButton)
		HostButton->OnClicked.AddDynamic(this, &ThisClass::OnHostClicked);

	if (JoinButton)
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::OnJoinClicked);
	
	return true;
}

/*
void UCharacterCustomizationWidget::UpdateCustomizationAndPreview(const FCharacterCustomization& NewCustomization)
{
	if (BlasterGameInstance)
	{
		BlasterGameInstance->PendingCustomization = NewCustomization;
	}
	if (PreviewActor)
	{
		PreviewActor->ApplyCustomization(NewCustomization);
	}
}
*/
	
void UCharacterCustomizationWidget::SetMatColorLight(FLinearColor NewColor, ECharacterMaterialIndex MaterialIndex)
{
	if (!BlasterGameInstance)
	{
		return;
	}
	
	if (MaterialIndex == ECharacterMaterialIndex::Material0)
	{
		BlasterGameInstance->PendingCustomization.Mat0_Light = NewColor;
	}
	else if (MaterialIndex == ECharacterMaterialIndex::Material1)
	{
		BlasterGameInstance->PendingCustomization.Mat1_Light = NewColor;
	}
	RefreshPreview();
}

void UCharacterCustomizationWidget::SetMatColorMajor(FLinearColor NewColor, ECharacterMaterialIndex MaterialIndex)
{
	if (!BlasterGameInstance)
	{
		return;
	}
	if (MaterialIndex == ECharacterMaterialIndex::Material0)
	{
		BlasterGameInstance->PendingCustomization.Mat0_Major = NewColor;
	}
	else if (MaterialIndex == ECharacterMaterialIndex::Material1)
	{
		BlasterGameInstance->PendingCustomization.Mat1_Major = NewColor;
	}
	RefreshPreview();
}

void UCharacterCustomizationWidget::SetMatColorMinor(FLinearColor NewColor, ECharacterMaterialIndex MaterialIndex)
{
	if (!BlasterGameInstance)
	{
		return;
	}
	if (MaterialIndex == ECharacterMaterialIndex::Material0)
	{
		BlasterGameInstance->PendingCustomization.Mat0_Minor = NewColor;
	}
	else if (MaterialIndex == ECharacterMaterialIndex::Material1)
	{
		BlasterGameInstance->PendingCustomization.Mat1_Minor = NewColor;
	}
	RefreshPreview();
}

void UCharacterCustomizationWidget::SetMatEmissivePower(const float NewPower, ECharacterMaterialIndex MaterialIndex)
{
	if (!BlasterGameInstance)
	{
		return;
	}
	if (MaterialIndex == ECharacterMaterialIndex::Material0)
	{
		BlasterGameInstance->PendingCustomization.Mat0_EmissivePower = NewPower;
	}
	else if (MaterialIndex == ECharacterMaterialIndex::Material1)
	{
		BlasterGameInstance->PendingCustomization.Mat1_EmissivePower = NewPower;
	}
	RefreshPreview();
}

void UCharacterCustomizationWidget::SetMatEmissiveMultiplier(const float NewMultiplier, ECharacterMaterialIndex MaterialIndex)
{
	if (!BlasterGameInstance)
	{
		return;
	}
	if (MaterialIndex == ECharacterMaterialIndex::Material0)
	{
		BlasterGameInstance->PendingCustomization.Mat0_EmissiveMultiplier = NewMultiplier;
	}
	else if (MaterialIndex == ECharacterMaterialIndex::Material1)
	{
		BlasterGameInstance->PendingCustomization.Mat1_EmissiveMultiplier = NewMultiplier;
	}
	RefreshPreview();
}


void UCharacterCustomizationWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	BlasterGameInstance = GetGameInstance<UBlasterGameInstance>();
	if (const UGameInstance* GI = GetGameInstance())
	{
		SessionsSubsystem = GI->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}
	
	if (!LobbyLevel.IsNull())
	{
		PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyLevel.ToSoftObjectPath().GetLongPackageName());
	}
	BindSubsystemCallbacks();
}

void UCharacterCustomizationWidget::NativeDestruct()
{
	UnbindSubsystemCallbacks();
	Super::NativeDestruct();
}



void UCharacterCustomizationWidget::OnHostClicked()
{
	if (HostButton) HostButton->SetIsEnabled(false);
	if (SessionsSubsystem)
		SessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
}

void UCharacterCustomizationWidget::OnJoinClicked()
{
	if (JoinButton) JoinButton->SetIsEnabled(false);
	if (SessionsSubsystem)
		SessionsSubsystem->FindSessions(10000);
}

void UCharacterCustomizationWidget::BindSubsystemCallbacks()
{
	if (!SessionsSubsystem) return;

	SessionsSubsystem->MultiplayerOnOnlineSubsystemNotAvailable.AddDynamic(
		this, &ThisClass::OnOnlineSubsystemNotAvailable);
	
	SessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(
		this, &ThisClass::OnCreateSession);
	
	SessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(
		this, &ThisClass::OnFindSessions);
	
	SessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(
		this, &ThisClass::OnJoinSession);
}

void UCharacterCustomizationWidget::UnbindSubsystemCallbacks()
{
	if (!SessionsSubsystem) return;

	SessionsSubsystem->MultiplayerOnOnlineSubsystemNotAvailable.RemoveDynamic(
		this, &ThisClass::OnOnlineSubsystemNotAvailable);
	
	SessionsSubsystem->MultiplayerOnCreateSessionComplete.RemoveDynamic(
		this, &ThisClass::OnCreateSession);
	
	SessionsSubsystem->MultiplayerOnFindSessionsComplete.RemoveAll(this);
	SessionsSubsystem->MultiplayerOnJoinSessionComplete.RemoveAll(this);
}

void UCharacterCustomizationWidget::RefreshPreview() const
{
	if (PreviewActor && BlasterGameInstance)
	{
		PreviewActor->ApplyCustomization(BlasterGameInstance->PendingCustomization);
	}
}

// ~Begin Online interface.
void UCharacterCustomizationWidget::OnOnlineSubsystemNotAvailable()
{
	if (HostButton) HostButton->SetIsEnabled(true);
	if (JoinButton) JoinButton->SetIsEnabled(true);
}

void UCharacterCustomizationWidget::OnCreateSession(const bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (UWorld* World = GetWorld())
			World->ServerTravel(PathToLobby);
	}
	else
	{
		if (HostButton) HostButton->SetIsEnabled(true);
	}
}

void UCharacterCustomizationWidget::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, const bool bWasSuccessful)
{
	if (!SessionsSubsystem) return;

	for (const auto& Result : SessionResults)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		if (SettingsValue == MatchType)
		{
			SessionsSubsystem->JoinSession(Result);
			return;
		}
	}

	if (!bWasSuccessful || SessionResults.Num() == 0)
	{
		if (JoinButton) JoinButton->SetIsEnabled(true);
	}
}

void UCharacterCustomizationWidget::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	if (const IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			if (APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController())
				PC->ClientTravel(Address, TRAVEL_Absolute);
		}
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		if (JoinButton) JoinButton->SetIsEnabled(true);
	}
}

// ~End Online interface.
