// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AWeapon;
class ABlasterCharacter;

/*
 * Combat component. Responsible for all combat functionality.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	friend class ABlasterCharacter;
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	void EquipWeapon(AWeapon* WeaponToEquip);

protected:
	virtual void BeginPlay() override;
	void SetAiming(bool NewAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool NewAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	void FireButtonPressed(bool bPressed);

	UFUNCTION(Server, Reliable)
	void ServerFire();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire();

	void TraceUnderCrosshairs(FHitResult& TraceHitResult) const;

private:
	TObjectPtr<ABlasterCharacter> Character;

	// Hand Socket Name.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta =(AllowPrivateAccess = true))
	FName HandSocketName = FName("RightHandSocket");

	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeapon, VisibleAnywhere, BlueprintReadOnly, Category = Combat,
		meta = (AllowPrivateAccess = true))
	TObjectPtr<AWeapon> EquippedWeapon;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	bool bIsAiming;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	float AimWalkSpeed;

	bool bFireButtonPressed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float TraceLength = 100000.f;
};
