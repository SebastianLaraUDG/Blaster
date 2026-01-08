// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

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
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void ReceiveDamage(AActor* DamagedActor, float Damage, const /*class*/ UDamageType* DamageType, /*class*/ AController* InstigatedBy, AActor* DamageCauser);
private:
	
	/*
	 * Player health.
	 */
	
	UPROPERTY(Replicated, VisibleAnywhere, Category = Health)
	float CurrentHealth = 1.f;	// TODO: setup replication?
	
	UPROPERTY(EditAnywhere, Category = Health, meta = (ClampMin = 0.00001))
	float MaxHealth = 100.f;
public:
	
	UFUNCTION(BlueprintPure, Category = Health)
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	
	UFUNCTION(BlueprintPure, Category = Health)
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
};
