// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Blaster/Interfaces/ExplodingDamageInterface.h"
#include "ProjectileRocket.generated.h"

/**
 * NOTE: this class spawns default impact particle and sound on hit (does not take into
 * account impact particles Map).
 */
UCLASS()
class BLASTER_API AProjectileRocket : public AProjectile, public IExplodingDamageInterface
{
	GENERATED_BODY()

public:
	AProjectileRocket();
	virtual void BeginPlay() override;
	
	virtual void Destroyed() override;

protected:
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* HitOther, UPrimitiveComponent* OtherComp,
	                   FVector NormalImpulse, const FHitResult& Hit) override;
		
	UPROPERTY(EditDefaultsOnly,Category="SFX|Projectile")
	TObjectPtr<USoundCue> ProjectileLoop;
	
	UPROPERTY()
	TObjectPtr<UAudioComponent> ProjectileLoopComponent;
	
	UPROPERTY(EditDefaultsOnly,Category="SFX|Projectile")
	TObjectPtr<USoundAttenuation> LoopingSoundAttenuation;
};
