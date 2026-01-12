// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Blaster/Interfaces/InteractWithCrosshairsInterface.h"
#include "BlasterCharacter.generated.h"

class ABlasterPlayerController;
class UHealthComponent;
class UCombatComponent;
class UWidgetComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AWeapon;
class UAnimMontage;

struct FInputActionValue;

/*
 * Main player character.
 */
UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(bool bAiming);
	void PlayElimMontage();
	
	virtual void OnRep_ReplicatedMovement() override;

	UFUNCTION(BlueprintCallable, Category = HUD)
	void UpdateHUD();
	
	// For when the player is eliminated.
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();
protected:
	virtual void BeginPlay() override;

	// Gameplay basic movement.
	void Move(const FInputActionValue& Value);
	void Turn(const FInputActionValue& Value);
	void HandleCrouchRequest();

	// Equipping Weapon.
	void EquipButtonPressed();

	// Aiming weapon.
	void AimStarted();
	void AimStopped();
	// Firing weapon.
	void FireWeaponPressed();
	void FireWeaponReleased();
	// Ignores velocity in Z axis.
	float CalculateSpeed() const;

	// Calculate aim offsets (yaw and pitch).
	void AimOffset(float DeltaTime);
	// Considers network compression and sets pitch to an appropriate value. 
	void CalculateAO_Pitch();
	// Turn for Simulated Proxies only.
	void SimProxiesTurn();
	virtual void Jump() override;
	
	void PlayHitReactMontage() const;

private:
	/* Camera and spring arm*/
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<UCameraComponent> Camera;

	/* Overhead widget. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HUD, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> OverheadWidgetComp;

	/* Overlapping weapon. */
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	TObjectPtr<AWeapon> OverlappingWeapon;

	// My fist rep notify!
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	/* Combat component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCombatComponent> CombatComponent;
	
	/* Health component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Health", meta = (AllowPrivateAccess = true))
	TObjectPtr<UHealthComponent> HealthComponent;
	
	

	// My first Server RPC!
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Combat, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	float TurningInterpolationSpeed = 10.f;
	
	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);
	
	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<UAnimMontage> FireWeaponMontage;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<UAnimMontage> HitReactMontage;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<UAnimMontage> ElimMontage;
	
	void HideCharacterIfCameraClose() const;
	
	// Distance from the camera to the player at which character mesh and weapon will be invisible.
	UPROPERTY(EditAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float CameraThreshold = 200.f;
	
	bool bRotateRootBone;
	
	/* Higher threshold value for less Turning in place animation repetitions. */
	UPROPERTY(EditAnywhere, Category = "Movement|Replication", meta = (AllowPrivateAccess = "true"))
	float TurnThreshold = 0.5f;
	
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float TimeSinceLastMovementReplication;
	
	UPROPERTY(EditAnywhere, Category = "Movement|Replication", meta = (AllowPrivateAccess = "true"))
	float TimeForMovementReplicationUpdate = 0.25f; // TODO: I don't like this name. Maybe I could rename it for something more appropriate.
	
	TObjectPtr<ABlasterPlayerController> BlasterPlayerController;
	
	void OnHealthChanged(float NewHealth, float DeltaHealth, AController* InstigatorController);

public:
	/* Input Temporal aqui. No se si ponerlo en el controller.*/

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> MoveInputAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> TurnInputAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> CrouchInputAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> JumpInputAction;

	// Equip weapon Input.
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputMappingContext> EquipWeaponMappingContext;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> EquipWeaponInputAction;

	// Aiming weapon input.
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputMappingContext> WeaponCombatInputMappingContext;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> AimInputAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> FireInputAction;

	/*~ Fin de seccion de inputs. */

	bool IsWeaponEquipped() const;
	bool IsAiming() const;
	void SetOverlappingWeapon(AWeapon* Weapon);
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	AWeapon* GetEquippedWeapon() const;
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE UHealthComponent* GetHealthComponent() const { return HealthComponent; }
};
