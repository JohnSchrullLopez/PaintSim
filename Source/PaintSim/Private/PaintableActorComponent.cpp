// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintableActorComponent.h"

#include "GenerateMips.h"
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
	CompletionPercentID = FVector2D::ZeroVector;
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
	PaintGameManager = Cast<APaintGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APaintGameManager::StaticClass()));

	//Render Target Init
	RenderTarget = NewObject<UTextureRenderTarget2D>();
	if(RenderTarget)
	{
		//RenderTarget->UpdateResourceImmediate();
		RenderTarget->bAutoGenerateMips = true;
		RenderTarget->InitCustomFormat(RTSize, RTSize, PF_B8G8R8A8, false);
		RenderTarget->ClearColor = FColor::Black;
		RenderTarget->MipsSamplerFilter = TF_Trilinear;
		UE_LOG(LogTemp, Warning, TEXT("%i"), RenderTarget->GetNumMips());

		//RenderTarget->mip
		RenderTarget->UpdateResourceImmediate();
		UE_LOG(LogTemp, Warning, TEXT("RENDERTEXTURE INITIALIZED"));
	}

	PaintableObjectMaterial = Mesh->CreateDynamicMaterialInstance(0, BasePaintableMaterial);
	PaintableObjectMaterial->SetTextureParameterValue("BaseTexture", BaseTexture);
	PaintableObjectMaterial->SetTextureParameterValue("ORM", PackedTexture);
	PaintableObjectMaterial->SetTextureParameterValue("NormalMap", NormalTexture);
	PaintableObjectMaterial->SetTextureParameterValue("Opacity", OpacityMask);
	PaintableObjectMaterial->SetTextureParameterValue("RenderTarget", RenderTarget);

	PaintMaterial = Mesh->CreateDynamicMaterialInstance(0, BasePaintMaterial);
	Mesh->SetMaterial(0, PaintableObjectMaterial);

	SetID(PaintGameManager->RegisterPaintableObject(this));
	PaintGameManager->GetPercentCompletionValue(CompletionPercentID);
	PaintGameManager->UpdateCompletionStateRT(CompletionPercentID, RenderTarget);
	
	ObjectScale = GetOwner()->GetComponentByClass<UStaticMeshComponent>()->Bounds.GetBox().GetSize().Length();
}


// Called every frame
void UPaintableActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPaintableActorComponent::OnPaintHit(FVector2D UV, float ScaleFactor)
{
	if (IsValid(RenderTarget) && IsValid(PaintableObjectMaterial))
	{
		PaintMaterial->SetVectorParameterValue("UV", FVector(UV.X, UV.Y, 0.0f));
		PaintMaterial->SetScalarParameterValue("Scale", ScaleFactor);
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), RenderTarget, PaintMaterial);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MATERIAL NOT INITIALIZED"));
	}
}

float UPaintableActorComponent::GetPercentPainted()
{
	return PaintGameManager->GetPercentCompletionValue(CompletionPercentID);
}
