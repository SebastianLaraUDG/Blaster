// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "CharacterCustomizationWidget.generated.h"

struct FCharacterCustomization;
class ACharacterPreviewActor;
class UBlasterGameInstance;
class UMultiplayerSessionsSubsystem;
class UButton;


UENUM(BlueprintType)
enum class ECharacterMaterialIndex : uint8
{
	Material0 UMETA(DisplayName = "Material 0"),
	Material1 UMETA(DisplayName = "Material 1")
};

/**
 * 
 */
UCLASS()
class BLASTER_API UCharacterCustomizationWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual bool Initialize() override;
	
	UPROPERTY(BlueprintReadWrite, Category = Customization)
	TObjectPtr<ACharacterPreviewActor> PreviewActor;
	//
	// Color customization functions.
	//
	
	UFUNCTION(BlueprintCallable, Category = Customization)
	void SetMatColorLight(FLinearColor NewColor, ECharacterMaterialIndex MaterialIndex);

	UFUNCTION(BlueprintCallable, Category = Customization)
	void SetMatColorMajor(FLinearColor NewColor, ECharacterMaterialIndex MaterialIndex);
	
	UFUNCTION(BlueprintCallable, Category = Customization)
	void SetMatColorMinor(FLinearColor NewColor, ECharacterMaterialIndex MaterialIndex);
	
	UFUNCTION(BlueprintCallable, Category = Customization)
	void SetMatEmissivePower(float NewPower, ECharacterMaterialIndex MaterialIndex);
	
	UFUNCTION(BlueprintCallable, Category = Customization)
	void SetMatEmissiveMultiplier(float NewMultiplier, ECharacterMaterialIndex MaterialIndex);
	
	//
	// UFUNCTION(BlueprintCallable, Category = Customization)
	// void UpdateCustomizationAndPreview(const FCharacterCustomization& NewCustomization);
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	// Callbacks del subsystem
	UFUNCTION()
	void OnCreateSession(const bool bWasSuccessful);
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, const bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnOnlineSubsystemNotAvailable();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> HostButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JoinButton;

	UPROPERTY(EditDefaultsOnly, Category = "Session")
	int32 NumPublicConnections = 4;

	UPROPERTY(EditDefaultsOnly, Category = "Session")
	FString MatchType = TEXT("FreeForAll");

	UPROPERTY(EditDefaultsOnly, Category = "Levels")
	TSoftObjectPtr<UWorld> LobbyLevel;

	UPROPERTY()
	UMultiplayerSessionsSubsystem* SessionsSubsystem;

	UPROPERTY()
	UBlasterGameInstance* BlasterGameInstance;

	FString PathToLobby;

	UFUNCTION()
	void OnHostClicked();

	UFUNCTION()
	void OnJoinClicked();

	void BindSubsystemCallbacks();
	void UnbindSubsystemCallbacks();
	
	void RefreshPreview() const;
};
