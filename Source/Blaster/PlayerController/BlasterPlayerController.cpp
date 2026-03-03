// Sebastian Lara. All rights reserved.


// ReSharper disable CppTooWideScope
#include "BlasterPlayerController.h"

#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/HUD/Announcement.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Blaster/HUD/SniperScope.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

/** 
 * TODO: This script uses the same code many times with minor changes, so
 * maybe I could refactor it to use functions and that way make it more readable.
 */



void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());

	ServerCheckMatchState();

	// Start timer to check server-client time sync.
	// Update time HUD and check time sync. 
	GetWorldTimerManager().SetTimer(CountdownTimer, this, &ThisClass::SetHUDTime, TimeSyncFrequency, true);
	// TODO: stop timer when game ends.
}


void ABlasterPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	if (!BlasterHUD)
	{
		BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	}


	const bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HealthBar &&
		BlasterHUD->CharacterOverlay->HealthText;

	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		// Progress bar Percent.
		BlasterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		// Text.
		const FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt32(Health),
		                                           FMath::CeilToInt32(MaxHealth));
		BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}

void ABlasterPlayerController::SetHUDScore(const float Score)
{
	if (!BlasterHUD)
	{
		BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	}
	const bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->ScoreAmount;
	if (bHUDValid)
	{
		const FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		BlasterHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
}

void ABlasterPlayerController::SetHUDDefeats(const int32 Defeats)
{
	if (!BlasterHUD)
	{
		BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	}
	const bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->ScoreAmount;
	if (bHUDValid)
	{
		const FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		BlasterHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
}

void ABlasterPlayerController::SetHUDWeaponAmmo(const int32 Ammo)
{
	if (!BlasterHUD)
	{
		BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	}
	const bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->WeaponAmmoAmount;
	if (bHUDValid)
	{
		const FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		BlasterHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void ABlasterPlayerController::SetHUDWeaponCarriedAmmo(const int32 CarriedAmmo)
{
	if (!BlasterHUD)
	{
		BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	}
	const bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->CarriedAmmoAmount;
	if (bHUDValid)
	{
		const FString CarriedAmmoText = FString::Printf(TEXT("%d"), CarriedAmmo);
		BlasterHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(CarriedAmmoText));
	}
}

void ABlasterPlayerController::SetHUDEquippedWeaponName(const EWeaponType WeaponType)
{
	if (!BlasterHUD)
	{
		BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	}
	const bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->EquippedWeaponName;

	if (bHUDValid)
	{
		FText EquippedWeaponName;
		switch (WeaponType)
		{
		case EWeaponType::EWT_AssaultRifle: EquippedWeaponName = FText::FromString(TEXT("Assault Rifle"));
			break;
		case EWeaponType::EWT_RocketLauncher: EquippedWeaponName = FText::FromString(TEXT("Rocket Launcher"));
			break;
		case EWeaponType::EWT_Pistol: EquippedWeaponName = FText::FromString(TEXT("Pistol"));
			break;
		case EWeaponType::EWT_Shotgun: EquippedWeaponName = FText::FromString(TEXT("Shotgun"));
			break;
		case EWeaponType::EWT_SniperRifle: EquippedWeaponName = FText::FromString(TEXT("Sniper Rifle"));
			break;
		case EWeaponType::EWT_SubmachineGun: EquippedWeaponName = FText::FromString(TEXT("SMG"));
			break;
		// Empty text.
		case EWeaponType::EWT_MAX: EquippedWeaponName = FText::FromString(TEXT(""));
			break;
		}
		BlasterHUD->CharacterOverlay->EquippedWeaponName->SetText(EquippedWeaponName);
	}
}

void ABlasterPlayerController::SetHUDMatchCountdown(const float CountdownTime)
{
	BlasterHUD = BlasterHUD ? BlasterHUD.Get() : Cast<ABlasterHUD>(GetHUD());

	const bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->MatchCountDownText;
	if (bHUDValid)
	{
		if (CountdownTime < 0.f) // Avoid displaying negative time.
		{
			BlasterHUD->CharacterOverlay->MatchCountDownText->SetText(FText());
			return;
		}
		const int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		const int32 Seconds = CountdownTime - Minutes * 60.f;

		/* Play countdown animation in text if specified and the time remaining is 30 seconds or less. */
		if (Seconds <= 30 && Minutes == 0 && BlasterHUD->CharacterOverlay->CountdownAnimation)
		{
			BlasterHUD->CharacterOverlay->MatchCountDownText->SetColorAndOpacity(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
			BlasterHUD->CharacterOverlay->PlayAnimation(BlasterHUD->CharacterOverlay->CountdownAnimation,
			                                            0.f, 30);
			// 30 loops is hardcoded for 30 seconds, it could be changed to a UPROPERTY variable if you want to.
		}

		const FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		BlasterHUD->CharacterOverlay->MatchCountDownText->SetText(FText::FromString(CountdownText));
	}
}

void ABlasterPlayerController::SetHUDAnnouncementCountdown(const float CountdownTime)
{
	BlasterHUD = BlasterHUD ? BlasterHUD.Get() : Cast<ABlasterHUD>(GetHUD());
	const bool bHUDValid = BlasterHUD && BlasterHUD->Announcement && BlasterHUD->Announcement->WarmupTime;
	if (bHUDValid)
	{
		if (CountdownTime < 0.f) // Avoid displaying negative time.
		{
			BlasterHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}
		const int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		const int32 Seconds = CountdownTime - Minutes * 60;

		const FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		BlasterHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void ABlasterPlayerController::SetHUDSniperScope(const bool bIsAiming)
{
	BlasterHUD = BlasterHUD ? BlasterHUD.Get() : Cast<ABlasterHUD>(GetHUD());
	if (!BlasterHUD->SniperScope)
	{
		BlasterHUD->AddSniperScope();
	}
	
	const bool bHUDValid = BlasterHUD && BlasterHUD->SniperScope && BlasterHUD->SniperScope->ScopeZoomIn;
	if (!bHUDValid)
	{
		return;
	}
	
	// Play aim animation (zoom in).
	if (bIsAiming)
	{
		BlasterHUD->SniperScope->SetVisibility(ESlateVisibility::Visible);
		BlasterHUD->SniperScope->PlayAnimation(BlasterHUD->SniperScope->ScopeZoomIn);
	}
	// Play reverse aim animation (zoom out).
	else
	{
		BlasterHUD->SniperScope->PlayAnimation(BlasterHUD->SniperScope->ScopeZoomIn, 0.f, 1,
		                                       EUMGSequencePlayMode::Reverse);
	}
}

void ABlasterPlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;

	// 1. Calculate the remaining real time based on current match state.
	if (MatchState == MatchState::WaitingToStart)
	{
		TimeLeft = WarmupTime - GetServerTime();
	}
	else if (MatchState == MatchState::InProgress)
	{
		TimeLeft = WarmupTime + MatchTime - GetServerTime();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		// The total amount of time until the end of the Cooldown.
		TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime();
	}

	const uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

	// 2. Only update the HUD if the seconds changed to save a bit of performance.
	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			// In Cooldown and Warmup we use to use the Announcement text.
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		else if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}

	CountdownInt = SecondsLeft;
	CheckTimeSync();
}

// ~Begin Time Sync interface.

void ABlasterPlayerController::ServerRequestServerTime_Implementation(const float TimeOfClientRequest)
{
	const float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void ABlasterPlayerController::ClientReportServerTime_Implementation(const float TimeOfClientRequest,
                                                                     const float TimeServerReceivedClientRequest)
{
	// The time it took for the client request to get to the server.
	const float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	const float CurrentServerTime = TimeServerReceivedClientRequest + 0.5f * RoundTripTime;
	// Dividing by two is just an approximation.
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

void ABlasterPlayerController::CheckTimeSync()
{
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

float ABlasterPlayerController::GetServerTime() const
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

// ~End Time Sync interface.

void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	/*
	Update HUD on possess. This is because when the character respawned HUD (health bar) was not
	being initialized correctly.
	*/
	if (const auto BlasterCharacter = Cast<ABlasterCharacter>(InPawn))
	{
		BlasterCharacter->UpdateHUD();
	}
}

void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MatchState)
}

void ABlasterPlayerController::ServerCheckMatchState_Implementation()
{
	// Get all time values from the server and get ready for announcement and match widgets.
	if (const auto BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		WarmupTime = BlasterGameMode->WarmupTime;
		MatchTime = BlasterGameMode->MatchTime;
		CooldownTime = BlasterGameMode->CooldownTime;
		MatchState = BlasterGameMode->GetMatchState();
		LevelStartingTime = BlasterGameMode->LevelStartingTime;
		ClientJoinMidGame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime);
	}
}

void ABlasterPlayerController::ClientJoinMidGame_Implementation(const FName& StateOfMatch, const float Warmup,
                                                                const float Match, const float Cooldown,
                                                                const float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch; // Update the variable locally so that the client gets the info NOW. 
	OnMatchStateSet(MatchState);
	if (BlasterHUD && MatchState == MatchState::WaitingToStart)
	{
		BlasterHUD->AddAnnouncement();
	}
}

void ABlasterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		/*
		 // Add sniper in next tick due to its dependency with the player controller (it cannot be created yet in begin play).
		const auto AddSniperScopeCallback = FTimerDelegate::CreateLambda([this]
			{
				// Add sniper scope and hide it.
				BlasterHUD = BlasterHUD ? BlasterHUD.Get() : Cast<ABlasterHUD>(GetHUD());
				BlasterHUD->AddSniperScope();
			if (BlasterHUD->SniperScope)
				BlasterHUD->SniperScope->SetVisibility(ESlateVisibility::Hidden);
			}
		);
		//GetWorldTimerManager().SetTimerForNextTick(AddSniperScopeCallback);
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, AddSniperScopeCallback, 5.f, false);
		*/
	}
}

void ABlasterPlayerController::OnMatchStateSet(const FName& State)
{
	MatchState = State;

	// Just transitioned to gameplay.
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	// Match ended.
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ABlasterPlayerController::OnRep_MatchState()
{
	// Just transitioned to gameplay.
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	// Match ended.
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

/** When match starts add character overlay and hide announcement text.*/
void ABlasterPlayerController::HandleMatchHasStarted()
{
	if (BlasterHUD = BlasterHUD ? BlasterHUD.Get() : Cast<ABlasterHUD>(BlasterHUD); BlasterHUD)
	{
		BlasterHUD->AddCharacterOverlay();
		// Hide announcement text. 
		if (BlasterHUD->Announcement)
		{
			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ABlasterPlayerController::HandleCooldown()
{
	if (BlasterHUD = BlasterHUD ? BlasterHUD.Get() : Cast<ABlasterHUD>(BlasterHUD); BlasterHUD)
	{
		BlasterHUD->CharacterOverlay->RemoveFromParent(); // Remove overlay responsible for health, ammo, etc.

		const bool bHUDValid = BlasterHUD->Announcement &&
			BlasterHUD->Announcement->AnnouncementText &&
			BlasterHUD->Announcement->InfoText;

		if (bHUDValid)
		{
			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Visible); // Show announcement text.
			const FString AnnouncementText("New match starts in: ");
			// New text telling match will restart in a few seconds.
			BlasterHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));

			DisplayWinner();
		}
	}
	// In cooldown state, disable gameplay movement and
	// in case the weapon is firing, stop firing. 
	auto BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
	if (BlasterCharacter && BlasterCharacter->GetCombatComponent())
	{
		BlasterCharacter->bDisableGameplay = true;
		BlasterCharacter->GetCombatComponent()->FireButtonPressed(false);
	}
}

void ABlasterPlayerController::DisplayWinner() const
{
	const auto BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
	const auto BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
	if (BlasterGameState && BlasterPlayerState)
	{
		const TArray<ABlasterPlayerState*> TopPlayers = BlasterGameState->TopScoringPlayers;
		FString InfoTextString;

		// Case NO WINNER.
		if (TopPlayers.Num() == 0)
		{
			InfoTextString = FString("There is no winner.");
		}
		// Case CURRENT PLAYER IS WINNER.
		else if (TopPlayers.Num() == 1 && TopPlayers[0] == BlasterPlayerState)
		{
			InfoTextString = FString("You are the winner!");
		}
		// Case ONE WINNER, BUT IT IS NOT THIS PLAYER.
		else if (TopPlayers.Num() == 1)
		{
			InfoTextString = FString::Printf(TEXT("Winner: \n %s"), *TopPlayers[0]->GetPlayerName());
		}
		// Case MORE THAN ONE WINNER.
		else if (TopPlayers.Num() > 1)
		{
			InfoTextString = FString("Players tied for the win:\n");
			for (const auto TiedPlayer : TopPlayers)
			{
				InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
			}
		}

		BlasterHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
	}
}
