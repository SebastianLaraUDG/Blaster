// Sebastian Lara. All rights reserved.


#include "BlasterHUD.h"

void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();
	if (GEngine)
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		const float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		if (HUDPackage.CrosshairCenter)
		{
			DrawCrosshair(HUDPackage.CrosshairCenter, ViewportCenter, FVector2D::ZeroVector);
		}
		if (HUDPackage.CrosshairTop)
		{
			DrawCrosshair(HUDPackage.CrosshairTop, ViewportCenter, FVector2D(0.f, -SpreadScaled));
		}
		if (HUDPackage.CrosshairBottom)
		{
			DrawCrosshair(HUDPackage.CrosshairBottom, ViewportCenter, FVector2D(0.f, SpreadScaled));
		}
		if (HUDPackage.CrosshairLeft)
		{
			DrawCrosshair(HUDPackage.CrosshairLeft, ViewportCenter, FVector2D(-SpreadScaled, 0.f));
		}
		if (HUDPackage.CrosshairRight)
		{
			DrawCrosshair(HUDPackage.CrosshairRight, ViewportCenter, FVector2D(SpreadScaled, 0.f));
		}
	}
}

void ABlasterHUD::DrawCrosshair(UTexture2D* Texture, const FVector2D& ViewportCenter, const FVector2D& Spread)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y
	);
	DrawTexture(Texture, TextureDrawPoint.X, TextureDrawPoint.Y, TextureWidth, TextureHeight, 0.f, 0.f, 1.f, 1.f);
}
