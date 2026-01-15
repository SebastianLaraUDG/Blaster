// Sebastian Lara. All rights reserved.


#include "CombatComponent.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/Weapon/Weapon.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;
}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Character)
	{
		// Setup character base walk speed according to this component value.
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
		// Save Default Camera FOV.
		if (Character->GetCamera())
		{
			DefaultFOV = Character->GetCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;
		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate overlapping weapon.
	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	// Replicate aiming state.
	DOREPLIFETIME(UCombatComponent, bIsAiming);
}

void UCombatComponent::SetAiming(bool NewAiming)
{
	bIsAiming = NewAiming;
	/*
	 * We set the value even before calling the ServerSetAiming() function for cosmetic (animation) effects.
	 * It will be quicker for the client to change the animation this way.
	 */
	ServerSetAiming(NewAiming);

	// Change walk speed locally.
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = NewAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool NewAiming)
{
	bIsAiming = NewAiming;
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = NewAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && Character)
	{
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped); /* We are calling this so that these properties are set here first before
		AttachActor gets called to avoid issues with attaching the weapon while it is marked to simulate physics or something like that.		
		*/
		// Attach to character's hand socket.
		if (const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(HandSocketName))
		{
			HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
		}
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}



void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;

	if (bFireButtonPressed && EquippedWeapon)
	{
	  	Fire();
	}
}

void UCombatComponent::Fire()
{
	if (!bCanFire) return;
	
	bCanFire = false;
	ServerFire(HitTarget);
	if (EquippedWeapon)
	{
		CrosshairShootingFactor += CrosshairsShootingFactorIncrement;
	}
	StartFireTimer();
}

void UCombatComponent::StartFireTimer()
{
	if (!EquippedWeapon || !Character) return;

	Character->GetWorldTimerManager().SetTimer(
		FireTimer, this, &ThisClass::FireTimerFinished, EquippedWeapon->FireDelay
	);
}

void UCombatComponent::FireTimerFinished()
{
	if (!EquippedWeapon) return;
	bCanFire = true;
	if (bFireButtonPressed && EquippedWeapon->bAutomatic)
	{
		Fire();
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (!EquippedWeapon) return;
	UE_LOG(LogTemp, Display, TEXT("FIRE BUTTON PRESSED: %d"), bFireButtonPressed);
	if (Character)
	{
		Character->PlayFireMontage(bIsAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize{};
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	const FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition, CrosshairWorldDirection;
	const bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection
	);
	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;
		// Add an offset to Start location. This is because starting the trace from the camera position can
		// lead to inaccurate shots and hand rotation.
		if (Character)
		{
			const float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + StartOfTraceOffset);
			DrawDebugSphere(GetWorld(), Start, 12.f, 12, FColor::Blue, false);
		}

		const FVector End = CrosshairWorldPosition + CrosshairWorldDirection * TraceLength;
		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult, Start, End, ECollisionChannel::ECC_Visibility
		);
		// Make crosshairs red if actor interacts with interface. Otherwise, white.
		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>())
		{
			HUDPackage.CrosshairsColor = FLinearColor::Red;
			bAimingAtEnemy = true;
		}
		else
		{
			HUDPackage.CrosshairsColor = FLinearColor::White;
			bAimingAtEnemy = false;
		}
		
		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
		}

		DrawDebugSphere(GetWorld(), TraceHitResult.ImpactPoint, 12.f, 12, FColor::Red, false);
	}
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (!Character || !Character->Controller) return;

	if (!Controller)
	{
		Controller = Cast<ABlasterPlayerController>(Character->GetController());
	}
	if (!Controller) return; // Abort if controller is null. This could still happen due to server travel. TODO: improve readability.

	if (!HUD)
	{
		HUD = Cast<ABlasterHUD>(Controller->GetHUD());
	}
	if (!HUD) return; // Abort if HUD is null. This could still happen due to server travel.  TODO: improve readability.
	
	if (EquippedWeapon)
	{
		HUDPackage.CrosshairCenter = EquippedWeapon->CrosshairsCenter;
		HUDPackage.CrosshairBottom = EquippedWeapon->CrosshairsBottom;
		HUDPackage.CrosshairTop = EquippedWeapon->CrosshairsTop;
		HUDPackage.CrosshairLeft = EquippedWeapon->CrosshairsLeft;
		HUDPackage.CrosshairRight = EquippedWeapon->CrosshairsRight;
	}
	// Else case should set every field to nullptr, that way HUD will not draw anything.

	// Calculate crosshair spread (including jump).
	const FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
	const FVector2D VelocityMultiplierRange(0.f, 1.f);
	FVector Velocity = Character->GetVelocity();
	Velocity.Z = 0.f;
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange,
	                                                            Velocity.Size());
	if (Character->GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
		// TODO: interp speed and Target to variable?
	}
	else
	{
		// Interpolate very quickly when landing.
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}
	// Include aiming factor.
	if (bIsAiming)
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, CrosshairsDisplacementWhileAiming, DeltaTime, 30.f /*TODO: convert to param? */);
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
	}
	// Shrink crosshairs when aiming at enemies.
	if (bAimingAtEnemy)
	{
		CrosshairEnemyFactor = FMath::FInterpTo(CrosshairEnemyFactor, AimAtEnemyShrinkFactor, DeltaTime, 30.f);
	}
	else
	{
		CrosshairEnemyFactor = FMath::FInterpTo(CrosshairEnemyFactor, 0.0f, DeltaTime, 30.f);
	}
	
	// Shooting factor should always interpolate back to zero.
	CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, CrosshairShootingFactorBackToZeroInterpSpeed);

	HUDPackage.CrosshairSpread = 0.5f /* Hard coded value to spread crosshairs a little by default.  TODO: convert to param.*/ +
		CrosshairVelocityFactor + CrosshairInAirFactor + CrosshairAimFactor +
		CrosshairShootingFactor - CrosshairEnemyFactor;
	
	
	HUD->SetHudPackage(HUDPackage);
}

void UCombatComponent::InterpFOV(const float& DeltaTime)
{
	/*
	 * NOTE: to avoid the blur effect when aiming at large or very short distances
	 * you must set the Camera's Depth of Field: Focal Distance and the Aperture (F-stop) 
	 * attributes to large values.
	 */
	if (!EquippedWeapon) return;
	if (!Character || !Character->GetCamera()) return;

	// Zoom in.
	if (bIsAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->ZoomedFOV, DeltaTime,
		                              EquippedWeapon->ZoomInterpSpeed);
	}
	// Stopped aiming. Go back to default zoom.
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	Character->GetCamera()->SetFieldOfView(CurrentFOV);
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (!Character || !WeaponToEquip) return;

	// Drop already equipped weapon.
	if (EquippedWeapon)
	{
		EquippedWeapon->Drop();
	}
	
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	// Attach to character's hand socket.
	if (const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(HandSocketName))
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->SetHUDAmmo();

	// Stop orienting rotation to movement. This is to allow leaning animations in animation blueprint.
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}
