// Sebastian Lara. All rights reserved.


#include "ProjectileRocket.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystemInstanceController.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

AProjectileRocket::AProjectileRocket()
{
	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket Mesh"));
	RocketMesh->SetupAttachment(RootComponent);
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AProjectileRocket::BeginPlay()
{
	Super::BeginPlay();

	// Bind this only on clients because if not, this would cause the VFX|SFX to
	// be played only on the server.
	if (!HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectileRocket::OnHit);
	}

	// Spawn trail.
	if (TrailSystem)
	{
		TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TrailSystem, GetRootComponent(), FName(), GetActorLocation(), GetActorRotation(),
			EAttachLocation::Type::KeepWorldPosition, false);
	}
	// Spawn looping sound with attenuation.
	if (ProjectileLoop && LoopingSoundAttenuation)
	{
		ProjectileLoopComponent = UGameplayStatics::SpawnSoundAttached(
			ProjectileLoop, GetRootComponent(), FName(), GetActorLocation(), GetActorRotation(),
			EAttachLocation::Type::KeepWorldPosition, false, 1.f, 1.f, 0.f, LoopingSoundAttenuation, nullptr, false);
	}
}

// TODO: change to a lambda inside OnHit?
void AProjectileRocket::DestroyTimerFinished()
{
	Destroy();
}

void AProjectileRocket::Destroyed()
{
	Super::Destroyed();
}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComp, AActor* HitOther, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
	const auto FiringPawn = GetInstigator();
	const auto FiringController = FiringPawn->GetController();

	GetWorldTimerManager().SetTimer(
		DestroyTimer, this, &AProjectileRocket::DestroyTimerFinished,
		DestroyTime
	);

	// Spawn impact particles.
	if (DefaultImpactParticle)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, DefaultImpactParticle, GetActorLocation(), GetActorRotation());
	}
	
	// Spawn impact sound
	if (DefaultImpactSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, DefaultImpactSound, GetActorLocation());
	}
	// Hide mesh.
	if (RocketMesh)
	{
		RocketMesh->SetVisibility(false);
	}
	// Disable collision.
	if (CollisionBox)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	// Deactivate trail.
	if (TrailSystemComponent && TrailSystemComponent->GetSystemInstanceController())
	{
		TrailSystemComponent->GetSystemInstanceController()->Deactivate();
	}
	// Stop movement looping sound.
	if (ProjectileLoopComponent && ProjectileLoopComponent->IsPlaying())
	{
		ProjectileLoopComponent->Stop();
	}
	
	// Draw sphere at hit location.
	UKismetSystemLibrary::DrawDebugSphere(this, GetActorLocation(), 10.f, 12, FLinearColor::Gray, 15.f);
	// Draw inner radius
	UKismetSystemLibrary::DrawDebugSphere(this, GetActorLocation(), DamageInnerRadius, 12, FLinearColor::Red, 15.f);
	// Draw outer radius
	UKismetSystemLibrary::DrawDebugSphere(this, GetActorLocation(), DamageOuterRadius, 12, FLinearColor::Yellow, 15.f);

	if (!FiringPawn || !FiringController || !HasAuthority()) return;

	UGameplayStatics::ApplyRadialDamageWithFalloff(
		this,
		Damage,
		MinimumDamage,
		GetActorLocation(),
		DamageInnerRadius,
		DamageOuterRadius,
		DamageFalloff,
		UDamageType::StaticClass(),
		TArray<AActor*>(), // All actors within radius will receive damage.
		this,
		FiringController
	);

	/*
	 *Super::OnHit(HitComp, HitOther, OtherComp, NormalImpulse, Hit);
	 * This caused calling destroy and spawning impact particles and sound, but since it was called on
	 * destruction. FX disappeared on hit as well.
	 * This new approach allows particles and sound to live enough time even if the projectile was already destroyed. 
	 */
}
