#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Materials/MaterialInterface.h"
#include "Engine/Texture2D.h"
#include "Sound/SoundCue.h"
#include "Camera/CameraActor.h"
#include "SlideShowBase.generated.h"

UCLASS()
class MYPORTFOLIOS_API ASlideShowBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ASlideShowBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void OnAudioFinished();

	void ShowSlideAndPlaySound();

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	UStaticMeshComponent* RootMesh;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TArray<UStaticMeshComponent*> DecorationMeshes;

	const int32 NumDecorationMeshes = 5;

	UPROPERTY(EditAnywhere, Category = "Slides")
	UMaterialInterface* SlideMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	UAudioComponent* AudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slides", meta = (AllowPrivateAccess = "true"))
	TArray<UTexture2D*> SlideTextures;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slides", meta = (AllowPrivateAccess = "true"))
	TArray<USoundCue*> SlideSounds;

	int32 CurrentSlideIndex = 0;
};
