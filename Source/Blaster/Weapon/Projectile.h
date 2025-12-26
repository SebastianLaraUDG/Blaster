// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UProjectileMovementComponent;
class UBoxComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class USoundCue;
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

	UPROPERTY(EditDefaultsOnly, Category=VFX)
	TObjectPtr<UNiagaraSystem> ImpactParticles;
	
	UPROPERTY(editDefaultsOnly, Category=SFX)
	TObjectPtr<USoundCue> ImpactSound;
public:
};
