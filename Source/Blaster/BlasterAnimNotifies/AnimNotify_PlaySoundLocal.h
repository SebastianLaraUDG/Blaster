// Sebastian Lara. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify_PlaySound.h"
#include "AnimNotify_PlaySoundLocal.generated.h"

/**
 * Anim notify for playing sounds only on clients. Useful for walking while crouching sounds.
 */
UCLASS(Const, HideCategories=Object, CollapseCategories, Config = Game, meta=(DisplayName="Play Sound Locally"))
class BLASTER_API UAnimNotify_PlaySoundLocal : public UAnimNotify_PlaySound
{
	GENERATED_BODY()
	
	protected:
	virtual  void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
