// Sebastian Lara. All rights reserved.


#include "ProjectileGrenade.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AProjectileGrenade::AProjectileGrenade()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	
	ProjectileMovement->SetIsReplicated(true);
	ProjectileMovement->bShouldBounce = true;
	
	ProjectileMovement->BounceVelocityStopSimulatingThreshold = 200.f; // When velocity is less than this value, stop simulating to prevent lots of bouncing sounds.
}

void AProjectileGrenade::BeginPlay()
{
	AActor::BeginPlay(); // Super version is not implemented because it binds OnHit with Destroy() and the grenades do not destroy on contact, but after a delay.
	SpawnTrailSystem();
	StartDestroyTimer();
	ProjectileMovement->OnProjectileBounce.AddDynamic(this, &ThisClass::OnBounce);
}

void AProjectileGrenade::Destroyed()
{
	ExplodeDamage(Damage, MinimumDamage, DamageInnerRadius, DamageOuterRadius, DamageFalloff);
	Super::Destroyed();
}

void AProjectileGrenade::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (BounceSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, BounceSound, GetActorLocation());
	}
}
