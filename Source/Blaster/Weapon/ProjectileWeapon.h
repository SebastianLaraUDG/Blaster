// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * A Weapon that actually spawns a projectile, which while move throughout the world.
 */
UCLASS(Abstract, Blueprintable)
class BLASTER_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()
	
};
