// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/BlasterTypes/CharacterCustomization.h"
#include "GameFramework/PlayerState.h"
#include "BlasterPlayerState.generated.h"

class UBlasterGameInstance;
class ABlasterPlayerController;
class ABlasterCharacter;

/**
 *  In charge of score and defeats.
 */
UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * Replication notifies.
	 */
	virtual void OnRep_Score() override;	
	
	UFUNCTION()
	virtual void OnRep_Defeats();
	
	UFUNCTION()
	void OnRep_CustomizationData();
	
	void AddToScore(const float ScoreAmount);
	
	void AddToDefeats(const int32 DefeatsAmount);
	
	void SetCustomizationDataFromGameInstance(UBlasterGameInstance* GameInstance);
	
	UPROPERTY(ReplicatedUsing = OnRep_CustomizationData, BlueprintReadOnly)
	FCharacterCustomization CustomizationData;
	
private:
	UPROPERTY()
	TObjectPtr<ABlasterCharacter> Character = nullptr;
	
	UPROPERTY()
	TObjectPtr<ABlasterPlayerController> Controller = nullptr;
	
	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;
	
	// Check character and if it's not valid, try to cast it.
	// Does not guarantee that the pointer will be valid.
	void CheckCharacter();
	
	// Check character's controller and if it's not valid, try to cast it.
	// Does not guarantee that the pointer will be valid.
	void CheckController();
};
