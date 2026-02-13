// Sebastian Lara. All rights reserved.


#include "BlasterPlayerController.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/HUD/Announcement.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"
#include "Net/UnrealNetwork.h"

/** 
 * TODO: This script uses the same code many times with minor changes, so
 * maybe I could refactor it to use functions and that way make it more readable.
 */
void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	
	if (BlasterHUD)
	{
		BlasterHUD->AddAnnouncement();
	}
	
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
		const FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt32(Health), FMath::CeilToInt32(MaxHealth));
		BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}

void ABlasterPlayerController::SetHUDScore(const float& Score)
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

void ABlasterPlayerController::SetHUDDefeats(const int32& Defeats)
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

void ABlasterPlayerController::SetHUDWeaponAmmo(const int32& Ammo)
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

void ABlasterPlayerController::SetHUDWeaponCarriedAmmo(const int32& CarriedAmmo)
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

void ABlasterPlayerController::SetHUDEquippedWeaponName(EWeaponType WeaponType)
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
		
		/**
		 * With a packaged game, GetDisplayValueAsText() will not return the display name,
		 * but the enum name as in, EWT_AssaultRifle, without the enum class name. 
		*/
#if WITH_EDITOR
		switch (WeaponType)
		{
		case EWeaponType::EWT_AssaultRifle: EquippedWeaponName = FText::FromString(TEXT("Assault Rifle")); break;
			// Empty text.
		case EWeaponType::EWT_MAX:	EquippedWeaponName = FText::FromString(TEXT("")); break;
		}
		
#else
		// Empty text.
		if (WeaponType == EWeaponType::EWT_MAX)
		{
			EquippedWeaponName = FText::FromString(TEXT(""));			
		}
		else
		{
			EquippedWeaponName = StaticEnum<EWeaponType>()->GetDisplayNameTextByValue(static_cast<int64>(WeaponType));/*Deprecated UEnum::GetDisplayValueAsText<EWeaponType>(WeaponType);*/
		}
		
#endif
		BlasterHUD->CharacterOverlay->EquippedWeaponName->SetText(EquippedWeaponName);
	}
}

void ABlasterPlayerController::SetHUDMatchCountdown(const float& CountdownTime)
{	
	BlasterHUD = BlasterHUD ? BlasterHUD.Get() : Cast<ABlasterHUD>(GetHUD());
	
	const bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->MatchCountDownText;
	if (bHUDValid)
	{
		const int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		const int32 Seconds = CountdownTime - Minutes * 60.f;
		
		const FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		BlasterHUD->CharacterOverlay->MatchCountDownText->SetText(FText::FromString(CountdownText));
	}
}

void ABlasterPlayerController::SetHUDTime()
{
	// CheckTimeSync();
	const uint32 SecondsLeft = FMath::CeilToInt(MatchTime - GetServerTime());
	if (CountdownInt != SecondsLeft)
	{
		SetHUDMatchCountdown(MatchTime - GetServerTime());
	}
	CountdownInt = SecondsLeft;
	
	CheckTimeSync();
}

// ~Begin Time Sync interface.

void ABlasterPlayerController::ServerRequestServerTime_Implementation(const float& TimeOfClientRequest)
{
	const float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void ABlasterPlayerController::ClientReportServerTime_Implementation(const float& TimeOfClientRequest,
	const float& TimeServerReceivedClientRequest)
{
	// The time it took for the client request to get to the server.
	const float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	const float CurrentServerTime = TimeServerReceivedClientRequest + 0.5f * RoundTripTime; // Dividing by two is just an approximation.
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
	if (auto BlasterCharacter = Cast<ABlasterCharacter>(InPawn))
	{
		BlasterCharacter->UpdateHUD();
	}
}

void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MatchState)
}

void ABlasterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ABlasterPlayerController::OnMatchStateSet(const FName& State)
{
	MatchState = State;
	
	HandleMatchHasStarted();
}

void ABlasterPlayerController::OnRep_MatchState()
{
	HandleMatchHasStarted();
}

/** When match starts add character overlay and hide announcement text. */
void ABlasterPlayerController::HandleMatchHasStarted()
{
	if (MatchState != MatchState::InProgress) return;


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
