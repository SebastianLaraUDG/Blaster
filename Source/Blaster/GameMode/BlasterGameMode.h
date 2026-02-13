// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

class ABlasterCharacter;
class ABlasterPlayerController;
/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ABlasterGameMode();
	virtual void PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController) const;
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);
	
	// How long the player will control an invisible flying pawn before
	// match begins.
	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

protected:
	virtual void HandleMatchIsWaitingToStart() override;
	virtual void OnMatchStateSet() override;
private:
	FTimerHandle WarmupStateTimer;
};
