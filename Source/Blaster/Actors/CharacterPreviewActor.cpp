// Sebastian Lara. All rights reserved.


#include "CharacterPreviewActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Blaster/BlasterTypes/CharacterCustomization.h"

/*
 * These params names are hardcoded and refer to the param names of a specific material in this project.
 */
const FName ACharacterPreviewActor::Param_Color1 = FName("Color1");
const FName ACharacterPreviewActor::Param_Color2 = FName("Color2");
const FName ACharacterPreviewActor::Param_Color3 = FName("Color3");
const FName ACharacterPreviewActor::Param_EmissiveMultiplier = FName("EmissiveMultiplyer"); // typo del material
const FName ACharacterPreviewActor::Param_EmissivePower = FName("emissivePower");


ACharacterPreviewActor::ACharacterPreviewActor()
{
	PrimaryActorTick.bCanEverTick = false;

	PreviewMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PreviewMesh"));
	SetRootComponent(PreviewMesh);
}

void ACharacterPreviewActor::BeginPlay()
{
	Super::BeginPlay();
	
	Mat0 = PreviewMesh->CreateAndSetMaterialInstanceDynamic(0);
	Mat1 = PreviewMesh->CreateAndSetMaterialInstanceDynamic(1);
}

void ACharacterPreviewActor::ApplyCustomization(const FCharacterCustomization& Customization)
{	
	// Armor
	if (Mat0)
	{
		Mat0->SetVectorParameterValue(Param_Color1, Customization.Mat0_Light);
		Mat0->SetVectorParameterValue(Param_Color2, Customization.Mat0_Major);
		Mat0->SetVectorParameterValue(Param_Color3, Customization.Mat0_Minor);
		Mat0->SetScalarParameterValue(Param_EmissiveMultiplier, Customization.Mat0_EmissiveMultiplier);
		Mat0->SetScalarParameterValue(Param_EmissivePower, Customization.Mat0_EmissivePower);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("material0 is null"))
	}

	if (Mat1)
	{
		Mat1->SetVectorParameterValue(Param_Color1, Customization.Mat1_Light);
		Mat1->SetVectorParameterValue(Param_Color2, Customization.Mat1_Major);
		Mat1->SetVectorParameterValue(Param_Color3, Customization.Mat1_Minor);
		Mat1->SetScalarParameterValue(Param_EmissiveMultiplier, Customization.Mat1_EmissiveMultiplier);
		Mat1->SetScalarParameterValue(Param_EmissivePower, Customization.Mat1_EmissivePower);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("material0 is null"))
	}
}
