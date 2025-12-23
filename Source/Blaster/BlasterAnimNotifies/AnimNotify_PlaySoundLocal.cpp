// Sebastian Lara. All rights reserved.


#include "AnimNotify_PlaySoundLocal.h"
#include "Kismet/GameplayStatics.h"

void UAnimNotify_PlaySoundLocal::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                        const FAnimNotifyEventReference& EventReference)
{
	APawn* const Owner = Cast<APawn>(MeshComp->GetOwner());
	if (Owner && Owner->IsLocallyControlled())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Sound, VolumeMultiplier, PitchMultiplier);
	}
	
	Super::Notify(MeshComp, Animation, EventReference);
}
