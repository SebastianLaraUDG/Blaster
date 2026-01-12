// Sebastian Lara. All rights reserved.


#include "HealthComponent.h"

#include "Net/UnrealNetwork.h"
#include "GameFramework/Controller.h" 


UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UHealthComponent, CurrentHealth);
}


void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind owners OnTakeAnyDamage (only on server)
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		GetOwner()->OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
	}
}

void UHealthComponent::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	const float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);
	LastInstigatorController = InstigatedBy;
	OnHealthChanged.Broadcast(CurrentHealth, CurrentHealth - OldHealth, LastInstigatorController);
}

void UHealthComponent::OnRep_Health(float OldHealth)
{
	OnHealthChanged.Broadcast(CurrentHealth, CurrentHealth - OldHealth, LastInstigatorController);
}