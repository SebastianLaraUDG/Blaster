// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SniperScope.generated.h"

/**
 * A widget for sniper rifle aiming. Can play a zoom in animation.
 */
UCLASS()
class BLASTER_API USniperScope : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ScopeZoomIn;
};
