// Sebastian Lara. All rights reserved.


#include "HitScanWeapon.h"

#include "NiagaraFunctionLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetMathLibrary.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	const auto World = GetWorld();
	if (!World) return;

	const auto OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	const auto InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetMesh()->GetSocketByName(MuzzleSocketName);
	if (!MuzzleFlashSocket) return;

	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetMesh());
	const FVector Start = SocketTransform.GetLocation();
	
	if (GetWeaponType() != EWeaponType::EWT_Shotgun) // For all weapons that do not scatter.
	{
		//
	}
	else
	{
		for (uint32 Index = 0; Index < NumberOfPellets; Index++)
		{
			FVector End = TraceEndWithScatter(Start, HitTarget);
		}
	}
	
	const FVector End = Start + (HitTarget - Start).GetSafeNormal() * MaxTraceDistance;
	FVector BeamEnd = End;

	// Trace will ignore owner.
	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(OwnerPawn);

	World->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility, CollisionParams);
	if (Hit.bBlockingHit) // There was an impact.
	{
		BeamEnd = Hit.ImpactPoint;
		if (HasAuthority() && InstigatorController) // Apply damage.
		{
			UGameplayStatics::ApplyDamage(
				Hit.GetActor(),
				Damage,
				InstigatorController,
				this,
				UDamageType::StaticClass()
			);
		}
		if (ImpactEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(World,
			                                               ImpactEffect,
			                                               Hit.ImpactPoint,
			                                               Hit.ImpactNormal.Rotation());
		}
	}
	if (BeamEffectLegacy)
	{
		UParticleSystemComponent* BeamLegacy = UGameplayStatics::SpawnEmitterAtLocation(
			World, BeamEffectLegacy, SocketTransform);

		BeamLegacy->SetVectorParameter(FName("Target"), BeamEnd);
	}
}

FVector AHitScanWeapon::TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget) const
{
	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
	const FVector EndLoc = SphereCenter + RandVec;
	const FVector ToEndLoc = EndLoc - TraceStart;
	
	DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red, true);
	
	DrawDebugSphere(GetWorld(), EndLoc, 4.f, 12, FColor::Orange, true);
	
	DrawDebugLine(
		GetWorld(),
		TraceStart,
		FVector(TraceStart + ToEndLoc * MaxTraceDistance / ToEndLoc.Size()),
		FColor::Cyan,
		true);
	return FVector(TraceStart + ToEndLoc * MaxTraceDistance / ToEndLoc.Size());
}
