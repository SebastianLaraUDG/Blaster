// Sebastian Lara. All rights reserved.


#include "ProjectileWeapon.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* const InstigatorPawn = Cast<APawn>(GetOwner());
	if (!ProjectileClass || !InstigatorPawn) return;
	
	
	if (USkeletalMeshSocket const* MuzzleFlashSocket = GetMesh()->GetSocketByName(MuzzleSocketName))
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetMesh());
		// From muzzle flash socket to hit location from TraceUnderCrosshairs.
		const FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		const FRotator TargetRotation = ToTarget.Rotation();
		
		// Spawn projectile. TODO: Optimize with Object Pool.
		if (UWorld* World = GetWorld())
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();// this;
			SpawnParams.Instigator = InstigatorPawn;
			World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(),TargetRotation,SpawnParams);
		}
	}
}
