// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chaos/ChaosEngineInterface.h"
#include "Projectile.generated.h"


class UProjectileMovementComponent;
class UBoxComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class USoundCue;

USTRUCT(BlueprintType)
struct FImpactEffect
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactParticle;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> ImpactSound;
};

	
/*
 *
 */
UCLASS()
class BLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	AProjectile();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* HitOther, UPrimitiveComponent* OtherComp,
	                   FVector NormalImpulse, const FHitResult& Hit);
	
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastImpactEffects(UPhysicalMaterial* HitMaterial);
	
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0.000001))
	float Damage = 1.f; // TODO: Wouldn't it be better to implement this in an interface?

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> CollisionBox;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	/* VFX. */
	UPROPERTY(EditDefaultsOnly, Category=VFX)
	TObjectPtr<UNiagaraSystem> TracerAsset;

	UPROPERTY(EditAnywhere, Category=VFX)
	TObjectPtr<UNiagaraComponent> TracerNiagaraComponent;
	
	// The particle to spawn on a hit.
	UPROPERTY(VisibleAnywhere, Category=ImpactEffects)
	TObjectPtr<UNiagaraSystem> ImpactParticle;
	
	// Default particle to spawn if hit surface does not have a physics material set.
	UPROPERTY(EditDefaultsOnly, Category=ImpactEffects)
	TObjectPtr<UNiagaraSystem> DefaultImpactParticle;
	
	// Different impact effects depending on surface hit.
	UPROPERTY(EditDefaultsOnly, Category=ImpactEffects)
	TMap<TEnumAsByte<EPhysicalSurface>, FImpactEffect> ImpactEffects;
	
	UPROPERTY(VisibleAnywhere, Category=ImpactEffects)
	TObjectPtr<USoundCue> ImpactSound;

	// Default sound to play if hit surface does not have a physics material set.
	UPROPERTY(EditAnywhere, Category=ImpactEffects)
	TObjectPtr<USoundCue> DefaultImpactSound;
public:
};
