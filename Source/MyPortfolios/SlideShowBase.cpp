#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "SlideShowBase.h"

ASlideShowBase::ASlideShowBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	CurrentSlideIndex = 0;

	RootMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootMesh"));
	SetRootComponent(RootMesh);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Plane"));
	if (MeshAsset.Succeeded())
	{
		RootMesh->SetStaticMesh(MeshAsset.Object);
		RootMesh->SetRelativeScale3D(FVector(4.0f, 2.25f, 1.0f));
	}

	for (int32 i = 0; i < NumDecorationMeshes; ++i)
	{
		UStaticMeshComponent* NewMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName(*FString::Printf(TEXT("DecorationMesh%d"), i)));
		NewMesh->SetupAttachment(RootMesh);
		if (MeshAsset.Succeeded())
		{
			NewMesh->SetStaticMesh(MeshAsset.Object);
			NewMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 10.f * (i + 1)));
			DecorationMeshes.Add(NewMesh);
		}
	}

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FARFilter Filter;
	Filter.ClassNames.Add(UTexture2D::StaticClass()->GetFName());
	Filter.PackagePaths.Add("/Game/SlideShows/SlideShow_01/Textures");
	TArray<FAssetData> TextureDataArray;
	AssetRegistryModule.Get().GetAssets(Filter, TextureDataArray);
	TextureDataArray.Sort([](const FAssetData& A, const FAssetData& B)
	{
		return A.AssetName < B.AssetName;
	});
	for (FAssetData TextureData : TextureDataArray)
	{
		UTexture2D* Texture = Cast<UTexture2D>(TextureData.GetAsset());
		if (Texture)
		{
			SlideTextures.Add(Texture);
		}
	}

	Filter.ClassNames.Empty();
	Filter.PackagePaths.Empty();
	Filter.ClassNames.Add(USoundCue::StaticClass()->GetFName());
	Filter.PackagePaths.Add("/Game/SlideShows/SlideShow_01/Sounds/Ques");
	TArray<FAssetData> SoundDataArray;
	AssetRegistryModule.Get().GetAssets(Filter, SoundDataArray);
	SoundDataArray.Sort([](const FAssetData& A, const FAssetData& B)
	{
		return A.AssetName < B.AssetName;
	});
	for (FAssetData SoundData : SoundDataArray)
	{
		USoundCue* SoundCue = Cast<USoundCue>(SoundData.GetAsset());
		if (SoundCue)
		{
			SlideSounds.Add(SoundCue);
		}
	}
}

void ASlideShowBase::BeginPlay()
{
	Super::BeginPlay();
	ACameraActor* TargetCamera = nullptr;
	for (TActorIterator<ACameraActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ACameraActor* CameraActor = *ActorItr;
		if (CameraActor)
		{
			TargetCamera = CameraActor;
			break;
		}
	}
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController && TargetCamera)
	{
		float BlendTime = 0.0f;
		EViewTargetBlendFunction BlendFunc = VTBlend_Linear;
		PlayerController->SetViewTargetWithBlend(TargetCamera, BlendTime, BlendFunc);
	}

	ASlideShowBase::ShowSlideAndPlaySound();
}

void ASlideShowBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASlideShowBase::OnAudioFinished()
{
	++CurrentSlideIndex;
	ASlideShowBase::ShowSlideAndPlaySound();
}

void ASlideShowBase::ShowSlideAndPlaySound()
{
	if (SlideTextures.Num() > 0 && CurrentSlideIndex < SlideTextures.Num())
	{
		UMaterial* BaseMaterial = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), nullptr, TEXT("/Game/Materials/M_Slide")));
		UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		if (DynamicMaterial && SlideTextures.IsValidIndex(CurrentSlideIndex) && SlideTextures[CurrentSlideIndex])
		{
			DynamicMaterial->SetTextureParameterValue(FName("SlideTexture"), SlideTextures[CurrentSlideIndex]);
			RootMesh->SetMaterial(0, DynamicMaterial);
		}

		if (SlideSounds.IsValidIndex(CurrentSlideIndex) && SlideSounds[CurrentSlideIndex])
		{
			AudioComponent->SetSound(SlideSounds[CurrentSlideIndex]);
			AudioComponent->Play();
			AudioComponent->OnAudioFinished.AddDynamic(this, &ASlideShowBase::OnAudioFinished);
		}
	}
}

