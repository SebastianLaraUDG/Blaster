// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()
public:
	AProjectileRocket();
protected:
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* HitOther, UPrimitiveComponent* OtherComp,
					   FVector NormalImpulse, const FHitResult& Hit) override;
private:
	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> RocketMesh;
	
	// The amount of damage to apply when an actor is on the outer radius.
	UPROPERTY(EditDefaultsOnly,Category = "Damage|Explosion Area", meta = (AllowPrivateAccess = true, ClampMin = 0.000001))
	float MinimumDamage = 0.1f;
	
	// Radius of the full damage area.
	UPROPERTY(EditDefaultsOnly,Category = "Damage|Explosion Area", meta = (AllowPrivateAccess = true, ClampMin = 0.000001))
	float DamageInnerRadius = 10.f;
	
	// Radius of the minimum damage area.
	UPROPERTY(EditDefaultsOnly,Category = "Damage|Explosion Area", meta = (AllowPrivateAccess = true, ClampMin = 0.000001))
	float DamageOuterRadius = 25.f;
	
	// Falloff exponent of damage from inner to outer radius.
	UPROPERTY(EditDefaultsOnly,Category = "Damage|Explosion Area", meta = (AllowPrivateAccess = true, ClampMin = 0.000001))
	float DamageFalloff = 1.f;
};
