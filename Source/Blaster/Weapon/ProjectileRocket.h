// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

/**
 * NOTE: this class spawns default impact particle and sound on hit (does not take into
 * account impact particles Map).
 */
UCLASS()
class BLASTER_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()

public:
	AProjectileRocket();
	virtual void BeginPlay() override;
	void DestroyTimerFinished();
	virtual void Destroyed() override;

protected:
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* HitOther, UPrimitiveComponent* OtherComp,
	                   FVector NormalImpulse, const FHitResult& Hit) override;

	UPROPERTY(EditDefaultsOnly, Category="VFX|Projectile")
	TObjectPtr<UNiagaraSystem> TrailSystem;
	
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> TrailSystemComponent;
	
	UPROPERTY(EditDefaultsOnly,Category="SFX|Projectile")
	TObjectPtr<USoundCue> ProjectileLoop;
	
	UPROPERTY()
	TObjectPtr<UAudioComponent> ProjectileLoopComponent;
	
	UPROPERTY(EditDefaultsOnly,Category="SFX|Projectile")
	TObjectPtr<USoundAttenuation> LoopingSoundAttenuation;

private:
	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> RocketMesh;

	// The amount of damage to apply when an actor is on the outer radius.
	UPROPERTY(EditDefaultsOnly, Category = "Damage|Explosion Area",
		meta = (AllowPrivateAccess = true, ClampMin = 0.000001))
	float MinimumDamage = 0.1f;

	// Radius of the full damage area.
	UPROPERTY(EditDefaultsOnly, Category = "Damage|Explosion Area",
		meta = (AllowPrivateAccess = true, ClampMin = 0.000001))
	float DamageInnerRadius = 10.f;

	// Radius of the minimum damage area.
	UPROPERTY(EditDefaultsOnly, Category = "Damage|Explosion Area",
		meta = (AllowPrivateAccess = true, ClampMin = 0.000001))
	float DamageOuterRadius = 25.f;

	// Falloff exponent of damage from inner to outer radius.
	UPROPERTY(EditDefaultsOnly, Category = "Damage|Explosion Area",
		meta = (AllowPrivateAccess = true, ClampMin = 0.000001))
	float DamageFalloff = 1.f;
	
	FTimerHandle DestroyTimer;
	
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0.1f))
	float DestroyTime = 3.f;
};
