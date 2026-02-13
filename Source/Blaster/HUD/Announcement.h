// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Announcement.generated.h"

class UTextBlock;

/**
 * A user widget to display announcements like the remaining time
 * to start match, etc.
 */
UCLASS()
class BLASTER_API UAnnouncement : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WarmupTime;
	
	// Main announcement text (e.g. "Match starts in:").
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AnnouncementText;
	
	// Put  here info like tips or controls.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InfoText;
};
