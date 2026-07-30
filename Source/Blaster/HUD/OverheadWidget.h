// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"

class UTextBlock;

/**
 * A Widget which has functionality to display the network Local Role.
 */
UCLASS()
class BLASTER_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()
	
	public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DisplayText;
	
	void SetDisplayText(const FString& TextToDisplay) const;
	
	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(APawn* InPawn);
	
	// Display online player name (i.e. steam username).
	UFUNCTION(BlueprintCallable)
	void ShowPlayerName(APawn* InPawn) const;
	
	protected:
	virtual void NativeDestruct() override;
};
