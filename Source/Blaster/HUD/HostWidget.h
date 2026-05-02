// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HostWidget.generated.h"

class UButton;
/**
 * A widget for the host of a match.
 */
UCLASS()
class BLASTER_API UHostWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartMatchButton;
protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnStartMatchClick();
};
