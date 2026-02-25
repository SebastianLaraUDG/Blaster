// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

class UNiagaraSystem;

/*
 * A type of weapon that casts line traces to emulate projectile hits.
 * NOTE: this type of weapon applies damage to the actor it hits, no matter
 * the subclass.
 */
UCLASS()
class BLASTER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& HitTarget) override;
private:
	
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.000001f))
	float Damage = 10.f;
	
	/* In the future, to implement different impact particles and sounds I could use the same approach
	* I used in the Projectile class.
	* For now, I will use only one impact particle.
	 */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ImpactEffect;
};
