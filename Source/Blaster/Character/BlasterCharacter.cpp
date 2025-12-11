// Sebastian Lara. All rights reserved.


#include "BlasterCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"


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
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* const PlayerController = Cast<APlayerController>(GetController());
	
	// Mapping context.
	if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		EnhancedInputSubsystem->ClearAllMappings();
		EnhancedInputSubsystem->AddMappingContext(InputMappingContext, 0);
	}

	// Input actions.
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInput) return;

	EnhancedInput->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	EnhancedInput->BindAction(TurnInputAction, ETriggerEvent::Triggered, this, &ThisClass::Turn);
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
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
