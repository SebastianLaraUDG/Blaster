// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterPreviewActor.generated.h"

struct FCharacterCustomization;
/*
 * Note: this class was not designed to be scalable.
 * It was designed in the last development cycle focusing to release only.
 * This functionality could be improved, specially in the sense of organization and scalability.
 */
UCLASS()
class BLASTER_API ACharacterPreviewActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ACharacterPreviewActor();
	virtual void BeginPlay() override;
	
	// Apply customization to mesh at runtime.
	UFUNCTION(BlueprintCallable, Category = Customization)
	void ApplyCustomization(const FCharacterCustomization& Customization);
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> PreviewMesh;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMaterialInstanceDynamic> Mat0;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMaterialInstanceDynamic> Mat1;
	
	static const FName Param_Color1;
	static const FName Param_Color2;
	static const FName Param_Color3;
	static const FName Param_EmissiveMultiplier;
	static const FName Param_EmissivePower;
};
