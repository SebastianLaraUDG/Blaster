#pragma once
#include "CoreMinimal.h"
#include "CharacterCustomization.generated.h"

USTRUCT(BlueprintType)
struct FCharacterCustomization
{
	GENERATED_BODY()
	
	// UPROPERTY(BlueprintReadWrite, EditAnywhere) Cut content. I wanted to develop this , but I need to get this project done (I don't have time left).
	// bool bIsMale = true;
	
	// Material 0 (armor)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor Mat0_Light = FLinearColor(FColor::FromHex(TEXT("FF5AD2FF")));

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor Mat0_Major = FLinearColor(FColor::FromHex(TEXT("264CC3FF")));

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor Mat0_Minor = FLinearColor(FColor::FromHex(TEXT("C245BBFF")));

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Mat0_EmissiveMultiplier = 10.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Mat0_EmissivePower = 10.f;

	// Material 1 (inner suit)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor Mat1_Light = FLinearColor(FColor::FromHex(TEXT("FF3CF1FF")));

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor Mat1_Major = FLinearColor(FColor::FromHex(TEXT("000000FF")));

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor Mat1_Minor = FLinearColor(FColor::FromHex(TEXT("000000FF")));

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Mat1_EmissiveMultiplier = 10.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Mat1_EmissivePower = 10.f;
};