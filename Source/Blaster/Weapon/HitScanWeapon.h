// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

class UNiagaraSystem;
class UParticleSystem;
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
	
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.000001f))
	float MaxTraceDistance = 100.f; 
	
	/* In the future, to implement different impact particles and sounds I could use the same approach
	* I used in the Projectile class.
	* For now, I will use only one impact particle.
	 */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ImpactEffect;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UParticleSystem> BeamEffectLegacy; // A legacy particle because I need to set a vector parameter and converting to niagara does not support making that parameter (I would have to make the parameter setup from scratch).
};
