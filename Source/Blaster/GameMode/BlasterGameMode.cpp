// Sebastian Lara. All rights reserved.


#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController,
                                        ABlasterPlayerController* AttackerController) const
{
	if (!AttackerController || !AttackerController->PlayerState)
	{
		UE_LOG(LogTemp, Error, TEXT("Attacker controller or their player state is not valid in class: %s"),
		       *ThisClass::StaticClass()->GetName());
		return;
	}
	if (!VictimController || !VictimController->PlayerState)
	{
		UE_LOG(LogTemp, Error, TEXT("Victim controller or their player state is not valid in class: %s"),
			   *ThisClass::StaticClass()->GetName());
		if (!IsValid(VictimController))
			UE_LOG(LogTemp, Error, TEXT("Victim controller is not valid in class: %s"),
			   *ThisClass::StaticClass()->GetName());
		//if (VictimController->PlayerState == nullptr)
	//		UE_LOG(LogTemp, Error, TEXT("Victim controller player state is not valid in class: %s"),
//			   *ThisClass::StaticClass()->GetName());
		return;
	}
	ABlasterPlayerState* AttackerPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
	ABlasterPlayerState* VictimPlayerState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;
	
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);
	}
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
