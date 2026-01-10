// Sebastian Lara. All rights reserved.


#include "BlasterPlayerController.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	
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
