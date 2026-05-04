// Sebastian Lara

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

class UTextBlock;
class UButton;
class UMultiplayerSessionsSubsystem;
/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;
	
	// Set up a UI widget which shows the cursor. Perfect for main menus.
	UFUNCTION(BlueprintCallable)
	void MenuSetupByPath(FString LobbyPath = FString(TEXT("/Game/Project/Maps/Lobby")), int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")));
	// Set up a UI widget which shows the cursor. Perfect for main menus.
	UFUNCTION(BlueprintCallable)
	void MenuSetupByLevel(TSoftObjectPtr<UWorld> Level, int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")));
	
protected:
	virtual void NativeDestruct() override;

	//
	// Callbacks for the custom delegates on the MultiplayerSessionsSubsystem
	//
	
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);
	
private:
	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;
	
	// Text for displaying if the online subsystem was not found.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> OnlineSubsystemWarningText;

	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();
	
	UFUNCTION()
	void OnOnlineSubsystemNotAvailable();

	// Remove widget and enable gameplay input mode
	void MenuTearDown();
	
	void BindMultiplayerSessionsSubsystemCallbacks();

	// The subsystem designed to handle all online session functionality
	UPROPERTY()
	UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	int32 NumPublicConnections{4};
	FString MatchType{TEXT("FreeForAll")};

	FString PathToLobby{TEXT("")};
};
