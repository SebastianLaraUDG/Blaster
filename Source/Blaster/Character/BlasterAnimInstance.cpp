// Sebastian Lara. All rights reserved.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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

	// Offset Yaw for strafing.
	FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();
	FRotator YawRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(YawRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation,DeltaRot,DeltaSeconds,StrafingInterpSpeed);
	YawOffset = DeltaRotation.Yaw;
	
	// Lean.
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = BlasterCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaSeconds;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaSeconds, LeanInterpSpeed);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);
}
