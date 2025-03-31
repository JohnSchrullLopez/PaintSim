// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintableActorComponent.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/BlueprintTypeConversions.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Managers/PaintGameManager.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"

// Sets default values for this component's properties
UPaintableActorComponent::UPaintableActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UPaintableActorComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeComponent();
	CompletionPercentID = FVector2D::ZeroVector;
	// ...
	
}

void UPaintableActorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (IsValid(RenderTarget))
	{
		RenderTarget = nullptr;
	}
}

void UPaintableActorComponent::InitializeComponent()
{
	Super::InitializeComponent();

	Mesh = GetOwner()->GetComponentByClass<UStaticMeshComponent>();
	PaintGameManager = Cast<APaintGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APaintGameManager::StaticClass()));

	//Render Target Init
	RenderTarget = NewObject<UTextureRenderTarget2D>();
	if(RenderTarget)
	{
		RenderTarget->InitCustomFormat(512, 512, PF_B8G8R8A8, false);
		//RenderTarget->InitCustomFormat(512, 512, EPixelFormat::PF_R8G8B8A8_UINT, false);
		RenderTarget->ClearColor = FColor::Black;
		//RenderTarget->SRGB = true;
		RenderTarget->UpdateResourceImmediate(true);
		UE_LOG(LogTemp, Warning, TEXT("RENDERTEXTURE INITIALIZED"));
	}

	PaintableObjectMaterial = Mesh->CreateDynamicMaterialInstance(0, BasePaintableMaterial);
	PaintableObjectMaterial->SetTextureParameterValue("BaseTexture", BaseTexture);
	PaintableObjectMaterial->SetTextureParameterValue("RenderTarget", RenderTarget);

	PaintMaterial = Mesh->CreateDynamicMaterialInstance(0, BasePaintMaterial);
	Mesh->SetMaterial(0, PaintableObjectMaterial);

	CompletionPercentID = PaintGameManager->RegisterPaintableObject(this);
	PaintGameManager->GetPercentCompletionValue(CompletionPercentID);
	PaintGameManager->UpdateCompletionStateRT(CompletionPercentID, RenderTarget);
	UE_LOG(LogTemp, Warning, TEXT("ID === %f , %f"), CompletionPercentID.X, CompletionPercentID.Y);
	
	UE_LOG(LogTemp, Warning, TEXT("INITIALIZATION SUCCESSFUL"));
	
}


// Called every frame
void UPaintableActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPaintableActorComponent::OnPaintHit(FVector2D UV)
{
	if (IsValid(RenderTarget) && IsValid(PaintableObjectMaterial))
	{
		PaintMaterial->SetVectorParameterValue("UV", FVector(UV.X, UV.Y, 0.0f));
		//PaintMaterial->SetVectorParameterValue("UV_R", FVector(UV_R.X, 0.0f));
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), RenderTarget, PaintMaterial);
		//UE_LOG(LogTemp, Warning, TEXT("L: %f		R: %f"), UV.X, UV_R.X);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MATERIAL NOT INITIALIZED"));
	}
}

float UPaintableActorComponent::CalculatePercentPainted()
{
	float Percent = 0.0f;

	FColor temp = UKismetRenderingLibrary::ReadRenderTargetPixel(GetWorld(), RenderTarget, 0, 0);
	UE_LOG(LogTemp, Warning, TEXT("%i"), temp.A);
	/* TArray<FColor> temp;// = UKismetRenderingLibrary::ReadRenderTargetPixel(GetWorld(), RenderTarget, 0, 0);
	UKismetRenderingLibrary::ReadRenderTarget(GetWorld(), RenderTarget, temp);
	
	for (int i = 0; i < temp.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("Calculate Percent %i"), temp[i].A);
	} */
	//UE_LOG(LogTemp, Warning, TEXT("Percent: %f"), Percent / (RenderTarget->SizeX * RenderTarget->SizeY));

	//UKismetRenderingLibrary::Read
	return 0.0f;//Percent / (RenderTarget->SizeX * RenderTarget->SizeY);
}
