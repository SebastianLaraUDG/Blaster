// Sebastian Lara. All rights reserved.


#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true;
}

void ABlasterGameMode::BeginPlay()
{
	Super::BeginPlay();
	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ABlasterGameMode::HandleMatchIsWaitingToStart()
{
	/** Setup and start a timer when level starts (no gameplay).*/
	
	
	Super::HandleMatchIsWaitingToStart();

	/** Lambda */
	FTimerDelegate TimerCallback = FTimerDelegate::CreateLambda([this]()
	{
		// Run only the first time the match starts.
		if (!HasMatchStarted())
		{
			// Start match and clear timer.
			StartMatch();
			GetWorldTimerManager().ClearTimer(WarmupStateTimer);
		}
	});

	// Run a timer to start match.
	GetWorldTimerManager().SetTimer(WarmupStateTimer, TimerCallback, WarmupTime, false);
}

void ABlasterGameMode::HandleMatchHasStarted()
{
	/** Setup and start a timer when match begins to transition to State Cooldown.*/
	
	
	Super::HandleMatchHasStarted();

	FTimerHandle TimerHandle;

	/** Lambda */
	FTimerDelegate TimerCallback = FTimerDelegate::CreateLambda([this]()
	{
		// Clear all the timers of this object to avoid issues.
		GetWorldTimerManager().ClearAllTimersForObject(this);
		// Set state to Cooldown to display winner.
		SetMatchState(MatchState::Cooldown);
	});
	
	GetWorldTimerManager().SetTimer(TimerHandle, TimerCallback, MatchTime, false);
}

void ABlasterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();
	
	// Add overlay to all player controllers.
	for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (auto BlasterPlayerController = Cast<ABlasterPlayerController>(*It))
		{
			BlasterPlayerController->OnMatchStateSet(MatchState);
		}
	}
}

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController,
                                        ABlasterPlayerController* AttackerController) const
{
	if (!AttackerController || !AttackerController->PlayerState)
	{
		return;
	}
	if (!VictimController || !VictimController->PlayerState)
	{
		return;
	}
	
	ABlasterPlayerState* AttackerPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
	ABlasterPlayerState* VictimPlayerState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;
	// Make sure Attacker is not the victim. Add score then.
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);
	}
	// Add victim defeat.
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}
	
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}
}

void ABlasterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	// Destroy pawn.
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	// Choose a random player start in the level and respawn.
	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(),PlayerStarts);
		const int32 SelectionIndex = FMath::RandRange(0, PlayerStarts.Num() - 1); // A random index to choose a player start. TODO: implement
																						// an algorithm to spawn at a location far away from all players.
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[SelectionIndex]);
	}
}