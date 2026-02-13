// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "BlasterPlayerController.generated.h"

class ABlasterHUD;
/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	/**
	 * HUD
	 */
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(const float& Score);
	void SetHUDDefeats(const int32& Defeats);
	void SetHUDWeaponAmmo(const int32& Ammo);
	void SetHUDWeaponCarriedAmmo(const int32& CarriedAmmo);
	void SetHUDEquippedWeaponName(EWeaponType WeaponType);
	void SetHUDMatchCountdown(const float& CountdownTime);
	
	void OnMatchStateSet(const FName& State);

protected:
	virtual void BeginPlay() override;
	// Set HUD time and check time sync.
	void SetHUDTime();
	
	virtual float GetServerTime() const; // Synced with server world clock.
	virtual void ReceivedPlayer() override; // Sync with server clock as soon as possible.
	
	/**
	 * Sync time between client and server.
	 */
	
	// Requests the current server time, passing in the client's time when the request was sent.
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(const float& TimeOfClientRequest);
	
	// Reports the current server time to the client in response to ServerRequestServerTime
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(const float& TimeOfClientRequest, const float& TimeServerReceivedClientRequest);
	
	float ClientServerDelta = 0.f; // Difference between client and server time.
	
	// Time sync and time HUD text update frequency. It is recommended to set this at a low value. Greater values will increase delay between updates.
	// TODO: I don't like putting this here. It should be in the game mode or game state.
	UPROPERTY(EditAnywhere, Category = Time, meta = (ClampMin = 0.f))
	float TimeSyncFrequency = 0.2f;
	
	
	void CheckTimeSync();
	
private:
	
	TObjectPtr<ABlasterHUD> BlasterHUD;
	// TODO: I don't like putting the match time here. It should be in the game mode or game state.
	float MatchTime = 125.f;
	
	uint32 CountdownInt = 0;
	FTimerHandle CountdownTimer;
	
	/** Match state. */
	
	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;
	
	UFUNCTION()
	void OnRep_MatchState();
	
	// Add character overlay to player HUD when match starts.
	void HandleMatchHasStarted();
};
