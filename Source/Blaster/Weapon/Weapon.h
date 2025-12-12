// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class UWidgetComponent;
class USphereComponent;
class USkeletalMeshComponent;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	
	EWS_MAX UMETA(DisplayName = "DefaultMax")	
};

/*
 * Base Weapon class.
 */
UCLASS(Abstract, Blueprintable)
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool FromSweep, const FHitResult&  SweepResult);

private:
	/* Mesh */
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	/* Area Sphere */
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<USphereComponent> AreaSphere;
	
	/* Weapon State */
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;
	
	/* Pickup widget */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> PickupWidget;
};
