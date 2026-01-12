// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class AController;


DECLARE_MULTICAST_DELEGATE_ThreeParams(FHealthChangedSignature, float NewHealth, float DeltaHealth, AController* InstigatorController);

/*
 * A health component. It is designed to be
 * reusable in different projects.
 * Supports replication.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	FHealthChangedSignature OnHealthChanged;
	
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void ReceiveDamage(AActor* DamagedActor, float Damage, const /*class*/ UDamageType* DamageType, /*class*/ AController* InstigatedBy, AActor* DamageCauser);
	
	UFUNCTION()
	virtual void OnRep_Health(float OldHealth);
private:
	
	/*
	 * Player health.
	 */
	
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = Health)
	float CurrentHealth = 89.5f;	// TODO: initialize at low value. It's 89 just for testing HUD.
	
	UPROPERTY(EditAnywhere, Category = Health, meta = (ClampMin = 0.00001))
	float MaxHealth = 100.f;
	
	// The instigator controller of the last damage taken event.
	// Saved to call on OnRep_Health.
	TObjectPtr<AController> LastInstigatorController;
public:
	
	UFUNCTION(BlueprintPure, Category = Health)
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	
	UFUNCTION(BlueprintPure, Category = Health)
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
};
