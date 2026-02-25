// Sebastian Lara. All rights reserved.


#include "HitScanWeapon.h"

#include "NiagaraFunctionLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	
	const auto World = GetWorld();
	if (!World) return;
	
	const auto OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;
	
	const auto OwnerController = OwnerPawn->GetController();
	if (!OwnerController) return;
	
	const USkeletalMeshSocket* MuzzleFlashSocket = GetMesh()->GetSocketByName(MuzzleSocketName);
	if (!MuzzleFlashSocket) return;
	
	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetMesh());
	const FVector Start = SocketTransform.GetLocation();
	const FVector End = Start + (HitTarget - Start) * 1.25f; // Multiply by 1.25 to make sure end location reaches hit location.
	
	FHitResult Hit;

	World->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility);
	if (Hit.bBlockingHit) // There was an impact.
	{
		if (HasAuthority()) // Apply damage.
		{
			UGameplayStatics::ApplyDamage(
				Hit.GetActor(),
				Damage,
				OwnerController,
				this,
				UDamageType::StaticClass()
				);
		}
		if (ImpactEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,
				ImpactEffect,
				Hit.ImpactPoint,
				Hit.ImpactNormal.Rotation());
		}
		DrawDebugSphere(World, Hit.ImpactPoint, 20.f, 12, FColor::Purple, true);
	}
}
