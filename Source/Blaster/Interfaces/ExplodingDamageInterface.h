// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ExplodingDamageInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UExplodingDamageInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for objects that deal exploding damage like grenades and rocket projectiles.
 */
class BLASTER_API IExplodingDamageInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Apply radial damage with falloff. Useful for grenades and rockets.
	void ExplodeDamage(float Damage, float MinimumDamage, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff);
};
