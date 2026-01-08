// Sebastian Lara. All rights reserved.


#include "ProjectileBullet.h"

#include "Kismet/GameplayStatics.h"

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
	UGameplayStatics::ApplyDamage(OtherActor, Damage, GetOwner()->GetInstigatorController(), this, UDamageType::StaticClass());
	
	// Called Super at the end because it calls Destroy().
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
