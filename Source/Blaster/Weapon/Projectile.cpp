// Sebastian Lara. All rights reserved.


#include "Projectile.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Blaster/Blaster.h"

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
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECR_Block);
	CollisionBox->bReturnMaterialOnMove = true; // Specify hits return physical materials.
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
	// Hit events on server only.
	if (HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	}
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* HitOther, UPrimitiveComponent* OtherComp,
                        FVector NormalImpulse, const FHitResult& Hit)
{
	// Hit was a block and physical material is defined, then show impact effects on all clients and server.
	
	if (Hit.bBlockingHit && Hit.PhysMaterial != nullptr)
	{
		MulticastImpactEffects(Hit.PhysMaterial.Get());
	}
	
	// TODO: Instead of destroying, when implementing object pool, make invisible, disable collision and return to pool.
	// The downside of doing that is that I will have to implement replication.
	Destroy();
}

void AProjectile::MulticastImpactEffects_Implementation(UPhysicalMaterial* HitMaterial)
{
	const FImpactEffect* const CurrentEffect = ImpactEffects.Find(HitMaterial->SurfaceType);
	// An effect was found with the provided hit material.
	if (CurrentEffect && CurrentEffect->ImpactParticle)
	{
		ImpactParticle = CurrentEffect->ImpactParticle;
	}
	else
	{
		ImpactParticle = DefaultImpactParticle;
	}

	// Same with sound
	if (CurrentEffect && CurrentEffect->ImpactSound)
	{
		ImpactSound = CurrentEffect->ImpactSound;
	}
	else
	{
		ImpactSound = DefaultImpactSound;
	}
}

void AProjectile::Destroyed()
{
	Super::Destroyed();

	if (ImpactParticle)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactParticle, GetActorLocation(), GetActorRotation());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), GetActorRotation());
	}
}
