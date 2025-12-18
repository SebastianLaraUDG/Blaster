// Sebastian Lara. All rights reserved.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(const float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (BlasterCharacter == nullptr)
	{
		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}
	if (BlasterCharacter == nullptr) return;

	FVector Velocity = BlasterCharacter->GetVelocity();
	Velocity.Z = 0.0f;
	Speed = Velocity.Size();

	// In air or grounded.
	bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();
	// 
	bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	// Weapon equipped.
	bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
	// Crouched.
	bIsCrouched = BlasterCharacter->IsCrouched();
	// Is aiming weapon.
	bIsAiming = BlasterCharacter->IsAiming();
}
