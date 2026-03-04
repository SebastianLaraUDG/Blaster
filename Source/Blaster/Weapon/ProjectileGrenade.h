// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Blaster/Interfaces/ExplodingDamageInterface.h"
#include "ProjectileGrenade.generated.h"

UCLASS()
class BLASTER_API AProjectileGrenade : public AProjectile, public IExplodingDamageInterface
{
	GENERATED_BODY()

public:
	AProjectileGrenade();
protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	
	UFUNCTION()
	void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);
private:
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> BounceSound;
};
