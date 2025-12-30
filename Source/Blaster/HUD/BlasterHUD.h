// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
	// Initialize as nullptr every field explicitly.
	FHUDPackage() : CrosshairCenter(nullptr), CrosshairLeft(nullptr), CrosshairRight(nullptr), CrosshairTop(nullptr), CrosshairBottom(nullptr)
	{
	}

	TObjectPtr<UTexture2D> CrosshairCenter;
	TObjectPtr<UTexture2D> CrosshairLeft;
	TObjectPtr<UTexture2D> CrosshairRight;
	TObjectPtr<UTexture2D> CrosshairTop;
	TObjectPtr<UTexture2D> CrosshairBottom;
};

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

private:
	FHUDPackage HUDPackage;

public:
	FORCEINLINE void SetHudPackage(const FHUDPackage& HudPackage) { HUDPackage = HudPackage; }
};
