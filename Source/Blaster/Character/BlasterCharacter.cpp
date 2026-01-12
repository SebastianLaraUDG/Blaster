// Sebastian Lara. All rights reserved.


#include "BlasterCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "BlasterAnimInstance.h"
#include "Blaster/Blaster.h"
#include "Blaster/BlasterComponents/HealthComponent.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "TimerManager.h"

ABlasterCharacter::ABlasterCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create camera and SpringArm.
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	SpringArmComponent->SetupAttachment(GetMesh());
	SpringArmComponent->TargetArmLength = 600.0f;
	SpringArmComponent->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	Camera->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// Make sure character can crouch.
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	// Block on visibility channel to allow for crosshairs change to red color.
	GetCharacterMovement()->RotationRate = FRotator(0.f, 850.f, 0.f);

	// Overhead Widget.
	OverheadWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidgetComp->SetupAttachment(RootComponent);

	// Combat component.
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComponent->SetIsReplicated(true);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	SetNetUpdateFrequency(66.f);
	SetMinNetUpdateFrequency(33.f);

	// Health component.
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->SetIsReplicated(true);
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*
	 * Aim offsets for both local players and simulated proxies.
	 * 
	 */

	// Only autonomous and authority proxies controlled by a player will calculate aim offsets.
	if (GetLocalRole() > ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	// Simulated proxies will update their Aim offsets every certain amount of time determined regardless of whether they moved or not.
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > TimeForMovementReplicationUpdate)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
	
	HideCharacterIfCameraClose();
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* const PlayerController = Cast<APlayerController>(GetController());

	// Mapping context.
	if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		EnhancedInputSubsystem->ClearAllMappings();
		EnhancedInputSubsystem->AddMappingContext(InputMappingContext, 0);

		// Equip Weapon Mapping context.
		EnhancedInputSubsystem->AddMappingContext(EquipWeaponMappingContext, 1);
		// Weapon combat mapping context.
		EnhancedInputSubsystem->AddMappingContext(WeaponCombatInputMappingContext, 2);
	}

	// Input actions.
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInput)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enhanced Input Component is NULL in %s"), *GetNameSafe(this));
		return;
	}
	// Bind movement and camera rotation.
	EnhancedInput->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	EnhancedInput->BindAction(TurnInputAction, ETriggerEvent::Triggered, this, &ThisClass::Turn);
	EnhancedInput->BindAction(JumpInputAction, ETriggerEvent::Started, this, &ThisClass::Jump);
	EnhancedInput->BindAction(JumpInputAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EnhancedInput->BindAction(CrouchInputAction, ETriggerEvent::Started, this, &ThisClass::HandleCrouchRequest);

	// Bind Equip Weapon.
	EnhancedInput->BindAction(EquipWeaponInputAction, ETriggerEvent::Triggered, this, &ThisClass::EquipButtonPressed);

	// Bind weapon combat.
	EnhancedInput->BindAction(AimInputAction, ETriggerEvent::Started, this, &ThisClass::AimStarted);
	EnhancedInput->BindAction(AimInputAction, ETriggerEvent::Completed, this, &ThisClass::AimStopped);
	EnhancedInput->BindAction(FireInputAction, ETriggerEvent::Started, this, &ThisClass::FireWeaponPressed);
	EnhancedInput->BindAction(FireInputAction, ETriggerEvent::Completed, this, &ThisClass::FireWeaponReleased);
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate overlapping weapon.
	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (CombatComponent)
	{
		CombatComponent->Character = this;
	}
}

void ABlasterCharacter::PlayFireMontage(bool bAiming)
{
	if (!CombatComponent || !IsWeaponEquipped()) return;

	UAnimInstance* const AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		const FName SectionName = bAiming ? FName("RifleAim") : FName("RifleHip"); // TODO: make variables?
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayElimMontage()
{
	UAnimInstance* const AnimInstance = GetMesh()->GetAnimInstance();
	if (ElimMontage && AnimInstance)
	{
		AnimInstance->Montage_Play(ElimMontage);
	}
}

void ABlasterCharacter::PlayHitReactMontage() const
{
	if (!CombatComponent || !IsWeaponEquipped()) return;

	if (const auto AnimInstance = GetMesh()->GetAnimInstance(); AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		const FName SectionName("FromFront"); // TEMP: add more sections to play here in code.
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Bind health changed.
	// We don't bind it only on server to allow for HitMontage play for every character in all machines.
	if (HealthComponent)
	{
		HealthComponent->OnHealthChanged.AddUObject(this, &ABlasterCharacter::OnHealthChanged);
	}
	// Initialize HUD health values.
	UpdateHUD();
}

void ABlasterCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Val = Value.Get<FVector2D>();

	if (Controller != nullptr && !Val.IsZero())
	{
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		const FVector ForwardDirection(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		const FVector RightDirection(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(ForwardDirection, Val.X);
		AddMovementInput(RightDirection, Val.Y);
	}
}

void ABlasterCharacter::Turn(const FInputActionValue& Value)
{
	const FVector2D Val = Value.Get<FVector2D>();
	AddControllerYawInput(Val.X);
	// Negate Y axis.
	AddControllerPitchInput(-Val.Y);
}

void ABlasterCharacter::HandleCrouchRequest()
{
	IsCrouched() ? UnCrouch() : Crouch();
}

void ABlasterCharacter::EquipButtonPressed()
{
	if (CombatComponent)
	{
		// Case: Called from the server.
		if (HasAuthority())
		{
			CombatComponent->EquipWeapon(OverlappingWeapon);
		}
		// Case: called from a client.
		else
		{
			ServerEquipButtonPressed();
		}
	}
}

void ABlasterCharacter::AimStarted()
{
	if (CombatComponent)
	{
		CombatComponent->SetAiming(true);
	}
}

void ABlasterCharacter::AimStopped()
{
	if (CombatComponent)
	{
		CombatComponent->SetAiming(false);
	}
}

void ABlasterCharacter::FireWeaponPressed()
{
	if (CombatComponent)
	{
		CombatComponent->FireButtonPressed(true);
		UE_LOG(LogTemp, Display, TEXT("Character started trying to shoot."));
	}
}

void ABlasterCharacter::FireWeaponReleased()
{
	if (CombatComponent)
	{
		CombatComponent->FireButtonPressed(false);
		UE_LOG(LogTemp, Display, TEXT("Character stopped shooting."));
	}
}

float ABlasterCharacter::CalculateSpeed() const
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if (!IsWeaponEquipped()) return;

	const float Speed = CalculateSpeed();
	const bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir) // Standing still, not jumping.
	{
		bRotateRootBone = true;
		FRotator CurrentRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;

		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning) // Not turning.
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir) // Running or jumping
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}
	CalculateAO_Pitch();
}

void ABlasterCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;

	// Convert compressed value to a valid range [-90,90].
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// Map pitch from the range [270,360) to [-90,0)
		const FVector2D InRange(270.f, 360.f);
		const FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ABlasterCharacter::SimProxiesTurn()
{
	// if (!CombatComponent || !CombatComponent->EquippedWeapon) return;
	if (!IsWeaponEquipped()) return;

	// Simulated proxies do not rotate bones.
	bRotateRootBone = false;

	// Is moving.
	if (CalculateSpeed() > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	// Delta Yaw rotation between frames.
	const float ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	// UE_LOG(LogTemp, Display, TEXT("Proxy Yaw: %f"), ProxyYaw);

	// Delta rotation exceeds threshold.
	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		// Turned right.
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		// Turned left.
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		// Not turning.
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

void ABlasterCharacter::Jump()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}

	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning) // We are turning.
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, TurningInterpolationSpeed);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.0f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			// Reset Starting Aim Rotation.
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void ABlasterCharacter::HideCharacterIfCameraClose() const
{
	if (!IsLocallyControlled()) return;

	const bool bCameraTooCloseToCharacter = (Camera->GetComponentLocation() - GetActorLocation()).Size() <
		CameraThreshold;

	// In case camera is too close to character, mesh must not be visible.
	GetMesh()->SetVisibility(!bCameraTooCloseToCharacter);

	// Weapon has a valid mesh.
	if (CombatComponent && CombatComponent->EquippedWeapon && CombatComponent->EquippedWeapon->GetMesh())
	{
		// In case camera is too close to character, character should not see weapon.
		CombatComponent->EquippedWeapon->GetMesh()->SetOwnerNoSee(bCameraTooCloseToCharacter);
	}
}

void ABlasterCharacter::OnHealthChanged(float NewHealth, float DeltaHealth, AController* InstigatorController)
{
	UpdateHUD();
	PlayHitReactMontage();
	
	// Register death in game mode.
	if (HealthComponent->GetCurrentHealth() == 0.f)
	{
		if (const auto BlasterGameMode= GetWorld()->GetAuthGameMode<ABlasterGameMode>())
		{
			const auto AttackerController = Cast<ABlasterPlayerController>(InstigatorController);
			BlasterGameMode->PlayerEliminated(this,
				BlasterPlayerController? BlasterPlayerController : nullptr,
				AttackerController ? AttackerController : nullptr
				);
		}
	}
}



void ABlasterCharacter::UpdateHUD()
{
	// Only local players need to update and see HUD.
	if (!IsLocallyControlled()) return;
	
	// Make sure controller is valid.
	if (!BlasterPlayerController)
	{
		BlasterPlayerController = Cast<ABlasterPlayerController>(Controller);
	}
	if (!BlasterPlayerController) return;
	BlasterPlayerController->SetHUDHealth(HealthComponent->GetCurrentHealth(), HealthComponent->GetMaxHealth());
}

void ABlasterCharacter::Elim()
{
	MulticastElim();

	// Start respawn timer.
	GetWorldTimerManager().SetTimer(ElimTimer, this, &ABlasterCharacter::ElimTimerFinished, ElimDelay);
}

void ABlasterCharacter::ElimTimerFinished()
{
	if (const auto BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>())
	{
		BlasterGameMode->RequestRespawn(this, Controller);
	}
}

void ABlasterCharacter::MulticastElim_Implementation()
{
	PlayElimMontage();
}

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	// Hide widget if end overlap.
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (CombatComponent)
	{
		CombatComponent->EquipWeapon(OverlappingWeapon);
	}
}

bool ABlasterCharacter::IsWeaponEquipped() const
{
	return (CombatComponent && CombatComponent->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming() const
{
	return (CombatComponent && CombatComponent->bIsAiming);
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled() && OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
}

AWeapon* ABlasterCharacter::GetEquippedWeapon() const
{
	return CombatComponent ? CombatComponent->EquippedWeapon : nullptr;
}

FVector ABlasterCharacter::GetHitTarget() const
{
	return CombatComponent ? CombatComponent->HitTarget : FVector();
}
