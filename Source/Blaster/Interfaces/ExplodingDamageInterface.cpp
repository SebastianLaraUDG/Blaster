// Sebastian Lara. All rights reserved.


#include "ExplodingDamageInterface.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"


// Add default functionality here for any IExplodingDamageInterface functions that are not pure virtual.
void IExplodingDamageInterface::ExplodeDamage(float Damage, float MinimumDamage, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff)
{
	AActor* ActorThis = Cast<AActor>(this->_getUObject());
	if (!ActorThis || !ActorThis->HasAuthority()) return;
	
	auto FiringPawn = ActorThis->GetInstigator();
	if (!FiringPawn) return;
	
	auto FiringController = FiringPawn->GetController();
	if (FiringController)
	{
		UGameplayStatics::ApplyRadialDamageWithFalloff(
		ActorThis,
		Damage,
		MinimumDamage,
		ActorThis->GetActorLocation(),
		DamageInnerRadius,
		DamageOuterRadius,
		DamageFalloff,
		UDamageType::StaticClass(),
		TArray<AActor*>(), // All actors within radius will receive damage.
		ActorThis,
		FiringController
	);
		
		UE_LOG(LogLevel, Display, TEXT("Drawing debug spheres from actor: %s. In script: %s"), *ActorThis->GetName(),TEXT("Exploding Damage Interface"))
		// Draw sphere at hit location.
		UKismetSystemLibrary::DrawDebugSphere(ActorThis, ActorThis->GetActorLocation(), 10.f, 12, FLinearColor::Gray, 15.f);
		// Draw inner radius
		UKismetSystemLibrary::DrawDebugSphere(ActorThis, ActorThis->GetActorLocation(), DamageInnerRadius, 12, FLinearColor::Red, 15.f);
		// Draw outer radius
		UKismetSystemLibrary::DrawDebugSphere(ActorThis, ActorThis->GetActorLocation(), DamageOuterRadius, 12, FLinearColor::Yellow, 15.f);
	}
}