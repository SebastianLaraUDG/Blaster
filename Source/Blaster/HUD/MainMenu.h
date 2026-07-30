// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

class UMultiplayerSessionsSubsystem;
class UTextBlock;
class UButton;
/**
 * 
 */
UCLASS()
class BLASTER_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual bool Initialize() override;
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> OnlineSubsystemWarningText;
	
	UPROPERTY()
	UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;
	
	// The level to load on click StartButton.
	UPROPERTY(EditDefaultsOnly, Category = Levels)
	TSoftObjectPtr<UWorld> Level;
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UFUNCTION()
	void OnStartClicked();
	
	UFUNCTION()
	void OnQuitClicked();
	
	UFUNCTION()
	void OnOnlineSubsystemNotAvailable();
};
