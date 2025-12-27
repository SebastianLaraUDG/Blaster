// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

class USoundCue;
/*
 * Casing class. For cosmetic purpose only.
 * No gameplay functionality.
 */
UCLASS()
class BLASTER_API ACasing : public AActor
{
	GENERATED_BODY()
	
public:	
	ACasing();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION() virtual void OnHit(
		UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> CasingMesh;
	
	UPROPERTY(EditDefaultsOnly)
	bool bPlayShellSoundOnCollisionOnce = true;
	
	UPROPERTY(EditAnywhere)
	float ShellEjectionImpulse;
	
	/*
	 * Sound to play when shell collides with something.
	 * TODO: adapt so that when the shell collides with 
	 * different surface types, it plays different sounds.
	 */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundCue> ShellSound;
};
