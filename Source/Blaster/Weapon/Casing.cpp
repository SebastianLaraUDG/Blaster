// Sebastian Lara. All rights reserved.


#include "Casing.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = false;

	// Casing mesh.
	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);
	// Camera response.
	CasingMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);
	CasingMesh->SetNotifyRigidBodyCollision(true);
	ShellEjectionImpulse = 10.f;
	
	SetLifeSpan(5.f); // TODO: Objet pool of casings?
}


void ACasing::BeginPlay()
{
	Super::BeginPlay();

	// Add impulse.
	CasingMesh->AddImpulse(GetActorForwardVector() * ShellEjectionImpulse);
	// Bind OnHit.
	CasingMesh->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
}

// On hit play sound and destroy.
void ACasing::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                    FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
	}
	
	// Disable collision notify to play sound only once (without this, sounds accumulate, and it hears horrible).
	if (bPlayShellSoundOnCollisionOnce)
	{
		CasingMesh->SetNotifyRigidBodyCollision(false);
	}
}
