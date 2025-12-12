// Sebastian Lara. All rights reserved.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Blaster/Character/BlasterCharacter.h"


AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	// Mesh. Block everything but pawns. No collision.
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn,ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Area Sphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Pickup widget.
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Pickup Widget Component"));
	PickupWidget->SetupAttachment(RootComponent);
}


void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	// Hide the pickup widget at start.
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
	
	// If on server.
	if (HasAuthority())
	{
		// Enable collision.
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
		// Bind BeginOverlap.
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this,&ThisClass::OnSphereBeginOverlap);
	}
}

void AWeapon::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult)
{
	ABlasterCharacter* const BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter && PickupWidget)
	{
		PickupWidget->SetVisibility(true);
	}
}


void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

