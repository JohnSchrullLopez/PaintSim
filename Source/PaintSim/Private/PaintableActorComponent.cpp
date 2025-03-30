// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintableActorComponent.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/BlueprintTypeConversions.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
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

	//Render Target Init
	RenderTarget = NewObject<UTextureRenderTarget2D>();
	if(RenderTarget)
	{
		RenderTarget->InitCustomFormat(512, 512, PF_R8G8B8A8, false);
		RenderTarget->ClearColor = FColor::Black;
		RenderTarget->UpdateResourceImmediate(true);
		UE_LOG(LogTemp, Warning, TEXT("RENDERTEXTURE INITIALIZED"));
	}

	PaintableObjectMaterial = Mesh->CreateDynamicMaterialInstance(0, BasePaintableMaterial);
	PaintableObjectMaterial->SetTextureParameterValue("BaseTexture", BaseTexture);
	PaintableObjectMaterial->SetTextureParameterValue("RenderTarget", RenderTarget);

	PaintMaterial = Mesh->CreateDynamicMaterialInstance(0, BasePaintMaterial);
	Mesh->SetMaterial(0, PaintableObjectMaterial);

	UE_LOG(LogTemp, Warning, TEXT("INITIALIZATION SUCCESSFUL"));
	
}


// Called every frame
void UPaintableActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPaintableActorComponent::OnPaintHit(FVector2D UV_L, FVector2D UV_R)
{
	if (IsValid(RenderTarget) && IsValid(PaintableObjectMaterial))
	{
		PaintMaterial->SetVectorParameterValue("UV_L", FVector(UV_L.X, UV_L.Y, 0.0f));
		PaintMaterial->SetVectorParameterValue("UV_R", FVector(UV_R.X, UV_R.Y, 0.0f));
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), RenderTarget, PaintMaterial);
		UE_LOG(LogTemp, Warning, TEXT("L: %f		R: %f"), UV_L.X, UV_R.X);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MATERIAL NOT INITIALIZED"));
	}
}
