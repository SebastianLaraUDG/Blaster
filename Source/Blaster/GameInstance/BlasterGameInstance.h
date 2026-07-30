// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Blaster/BlasterTypes/CharacterCustomization.h"
#include "BlasterGameInstance.generated.h"


/**
 * 
 */
UCLASS()
class BLASTER_API UBlasterGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	// Saved locally before log in to session.
	// It is transferred to the PlayerState when spawning.
	UPROPERTY(BlueprintReadWrite)
	FCharacterCustomization PendingCustomization;
	
};
