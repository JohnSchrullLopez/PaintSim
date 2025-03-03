// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintableActorComponent.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"

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

void UPaintableActorComponent::OnPaintHit_Implementation(FVector2D UV)
{
	UE_LOG(LogTemp, Warning, TEXT("C++ FUNCTION CALLED"));
}
