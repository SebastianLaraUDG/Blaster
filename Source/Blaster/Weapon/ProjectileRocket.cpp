// Sebastian Lara. All rights reserved.


#include "ProjectileRocket.h"

#include "Kismet/GameplayStatics.h"

AProjectileRocket::AProjectileRocket()
{
	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket Mesh"));
	RocketMesh->SetupAttachment(RootComponent);
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComp, AActor* HitOther, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	const auto FiringPawn = GetInstigator();
	const auto FiringController = FiringPawn->GetController();
	if (!FiringPawn || !FiringController) return;
	
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
	
	// TEST DE LOG DE INSTIGATORS
	UE_LOG(LogTemp, Display, TEXT("Firing controller: %s"),*FiringController->GetName());
	UE_LOG(LogTemp, Display, TEXT("My instigator controller: %s"),*GetInstigatorController()->GetName());
	UE_LOG(LogTemp,Display,TEXT("Firing controller instigator controller: %s"),*FiringPawn->GetInstigatorController()->GetName());
	
	// Draw sphere at hit location.
	UKismetSystemLibrary::DrawDebugSphere(this, GetActorLocation(),10.f,12, FLinearColor::Gray, 15.f);
	// Draw inner radius
	UKismetSystemLibrary::DrawDebugSphere(this, GetActorLocation(),DamageInnerRadius,12, FLinearColor::Red, 15.f);
	// Draw outer radius
	UKismetSystemLibrary::DrawDebugSphere(this, GetActorLocation(),DamageOuterRadius,12, FLinearColor::Yellow, 15.f);
	
	Super::OnHit(HitComp, HitOther, OtherComp, NormalImpulse, Hit);
}
