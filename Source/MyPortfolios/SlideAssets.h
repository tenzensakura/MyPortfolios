#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Sound/SoundCue.h"
#include "Engine/Texture2D.h"
#include "SlideAssets.generated.h"

USTRUCT(BlueprintType)
struct FSlideAssets
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide")
	UTexture2D* SlideTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide")
	USoundCue* SlideSound;
};

