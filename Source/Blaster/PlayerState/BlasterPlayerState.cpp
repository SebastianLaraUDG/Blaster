// Sebastian Lara. All rights reserved.


#include "BlasterPlayerState.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"

/**
 * TODO: this script makes many double checks. Maybe this can be improved?
 * Also Score functions contain duplicate code, as well as Defeats functions. Maybe group them inside small functions to improve readability.
 */

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ABlasterPlayerState, Defeats);
}

void ABlasterPlayerState::AddToScore(const float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
	
	CheckController();
	if (Controller)
	{
		Controller->SetHUDScore(GetScore());	
	}
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();
	
	CheckController();
	if (Controller)
	{
		Controller->SetHUDScore(GetScore());	
	}
}

void ABlasterPlayerState::AddToDefeats(const int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;
	
	CheckController();
	if (Controller)
	{
		Controller->SetHUDDefeats(Defeats);
	}
}

void ABlasterPlayerState::OnRep_Defeats()
{	
	CheckController();
	if (Controller)
	{
		Controller->SetHUDDefeats(Defeats);
	}
}

void ABlasterPlayerState::CheckCharacter()
{
	Character = Character ? Character.Get() : Cast<ABlasterCharacter>(GetPawn());
}

void ABlasterPlayerState::CheckController()
{
	CheckCharacter();
	Controller = Controller ? Controller.Get() : Cast<ABlasterPlayerController>(Character->Controller);
}
