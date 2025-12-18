// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterCharacter.generated.h"

class UCombatComponent;
class UWidgetComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AWeapon;

struct FInputActionValue;

/*
 * 
 */
UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
protected:
	virtual void BeginPlay() override;

	// Gameplay basic movement.
	void Move(const FInputActionValue& Value);
	void Turn(const FInputActionValue& Value);
	void HandleCrouchRequest();
	
	// Equipping Weapon.
	void EquipButtonPressed();

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
	
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

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
	
	// Equip weapon Input.
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputMappingContext> EquipWeaponMappingContext;
	
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> EquipWeaponInputAction;
	
	/*~ Fin de seccion de inputs. */
	
	bool IsWeaponEquipped() const;
	
	void SetOverlappingWeapon(AWeapon* Weapon);
};
