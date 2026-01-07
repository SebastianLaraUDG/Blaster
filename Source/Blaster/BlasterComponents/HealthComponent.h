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
	
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:
	
	/*
	 * Player health.
	 */
	
	UPROPERTY(VisibleAnywhere, Category = Health)
	float CurrentHealth;	// TODO: setup replication?
	
	UPROPERTY(VisibleAnywhere, Category = Health, meta = (ClampMin = 0.00001))
	float MaxHealth = 100.f;
		
};
