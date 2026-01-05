// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

/*
 * A struct with all crosshair positions.
 */
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
	float CrosshairSpread;
	FLinearColor CrosshairsColor;
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
	void DrawCrosshair(UTexture2D* Texture, const FVector2D& ViewportCenter, const FVector2D& Spread, const FLinearColor& CrosshairColor);
	
	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;
public:
	FORCEINLINE void SetHudPackage(const FHUDPackage& HudPackage) { HUDPackage = HudPackage; }
};
