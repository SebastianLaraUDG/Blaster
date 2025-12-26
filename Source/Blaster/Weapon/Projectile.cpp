// Sebastian Lara. All rights reserved.


#include "Projectile.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	// Box Collision.
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox Component"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	// Projectile movement.
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	ProjectileMovement->bRotationFollowsVelocity = true;

	// Tracer Particle.
	TracerNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Tracer Niagara Component"));
	TracerNiagaraComponent->SetupAttachment(RootComponent);
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (TracerAsset)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			TracerAsset, RootComponent, NAME_None, GetActorLocation(),
		       GetActorRotation(), EAttachLocation::Type::KeepWorldPosition,
		       true);
	}
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
