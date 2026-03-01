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
	
	TMap<AActor*, uint32> HitMap; // Pair of values for a character and the number of hits they receive.
	for (uint32 Index = 0; Index < NumberOfPellets; Index++)
	{
		// Trace will ignore owner.
		FHitResult Hit;
		WeaponTraceHit(Start, HitTarget, Hit);
		
		auto HitActor = Hit.GetActor();
		
		if (HitActor && HasAuthority() && InstigatorController) // Trace hit something.
		{
			if (HitMap.Contains(HitActor))
			{
				HitMap[HitActor]++; // Increase the number of hits.
			}
			else
			{
				HitMap.Emplace(HitActor,1);
			}
		}
		/* Impact VFX/SFX. */
		if (ImpactEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				World,
				ImpactEffect,
				Hit.ImpactPoint,
				Hit.ImpactNormal.Rotation()
				);
		}
		// Hit sound.
	}
	// Apply damage to all actors according to the number of hits received.
	for (auto HitPair : HitMap)
	{
		if (HitPair.Key && HasAuthority() && InstigatorController)
		{
			UGameplayStatics::ApplyDamage(
				HitPair.Key,
				Damage * HitPair.Value, // Number of pellets that hit multiplied by damage.
				InstigatorController,
				this,
				UDamageType::StaticClass()
			);
		}
	}
}

FVector AHitScanWeapon::TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget) const
{
	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
	const FVector EndLoc = SphereCenter + RandVec;
	const FVector ToEndLoc = EndLoc - TraceStart;

	DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red, false, 10.f);

	DrawDebugSphere(GetWorld(), EndLoc, 4.f, 12, FColor::Orange, false, 10.f);

	DrawDebugLine(
		GetWorld(),
		TraceStart,
		FVector(TraceStart + ToEndLoc * MaxTraceDistance / ToEndLoc.Size()),
		FColor::Cyan,
		false, 10.f);

	return FVector(TraceStart + ToEndLoc * MaxTraceDistance / ToEndLoc.Size());
}

void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit) const
{
	const auto World = GetWorld();
	if (!World) return;

	const FVector End = bUseScatter
		                    ? TraceEndWithScatter(TraceStart, HitTarget)
		                    : TraceStart + (HitTarget - TraceStart) * MaxTraceDistance;
	
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());
	World->LineTraceSingleByChannel(OutHit, TraceStart, End, ECollisionChannel::ECC_Visibility, CollisionParams);

	FVector BeamEnd = End;
	if (OutHit.bBlockingHit)
	{
		BeamEnd = OutHit.ImpactPoint;
	}
	if (BeamEffectLegacy)
	{
		UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
			World,
			BeamEffectLegacy,
			TraceStart
		);
		if (Beam)
		{
			Beam->SetVectorParameter(FName("Target"), BeamEnd);
		}
	}
}
