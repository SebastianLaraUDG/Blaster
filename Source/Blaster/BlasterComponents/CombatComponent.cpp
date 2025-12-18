// Sebastian Lara. All rights reserved.


#include "CombatComponent.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Weapon/Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCombatComponent::SetAiming(bool NewAiming)
{
	bIsAiming = NewAiming;
	/*
	 * We set the value even before calling the function for cosmetic (animation) effects.
	 * It will be quicker for the client to change the animation this way.
	 */
	ServerSetAiming(NewAiming);
}

void UCombatComponent::ServerSetAiming_Implementation(bool NewAiming)
{
	bIsAiming = NewAiming;
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// Replicate overlapping weapon.
	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	// Replicate aiming state.
	DOREPLIFETIME(UCombatComponent, bIsAiming);
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (!Character || !WeaponToEquip) return;

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	// Attach to characters hand socket.
	if (const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(HandSocketName))
	{
		HandSocket->AttachActor(EquippedWeapon,Character->GetMesh());					
	}
	EquippedWeapon->SetOwner(Character);
}
