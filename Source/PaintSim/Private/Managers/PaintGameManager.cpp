// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/PaintGameManager.h"

#include "MaxPossiblePercentPainted.h"
#include "PaintableActorComponent.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"

// Sets default values
APaintGameManager::APaintGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentID = 0;
}

// Called when the game starts or when spawned
void APaintGameManager::BeginPlay()
{
	Super::BeginPlay();

	CompletionStateRT->Filter = TF_Nearest;
	CompletionStateRT->MipsSamplerFilter = TF_Nearest;
	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), CompletionStateRT, FLinearColor(0,0,0,1));
	
	CompletionMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), CalculationMaterialBase);

	if (CompletionMaterialInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *CompletionMaterialInstance->GetName());
	}
}

// Called every frame
void APaintGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector2D APaintGameManager::RegisterPaintableObject(UPaintableActorComponent* PaintComponent)
{
	//Check if new ID is out of texture bounds
	if (CurrentID > CompletionStateRT->SizeX * CompletionStateRT->SizeY)
	{
		UE_LOG(LogTemp, Warning, TEXT("ERROR: OUT OF BOUNDS. INCREASE RENDER TEXTURE SIZE"));
		return FVector2D::ZeroVector;
	}

	//Convert 1D ID value to 2D
	FVector2D GeneratedID = FVector2D(CurrentID % CompletionStateRT->SizeX, std::floor(CurrentID / CompletionStateRT->SizeX));
	CurrentID++;

	PaintableObjects.Add(PaintComponent);
	
	return GeneratedID;
}

void APaintGameManager::UpdateCompletionStateRT(FVector2D ID, UTextureRenderTarget2D* ObjectMask)
{
	if (CompletionStateRT && CompletionMaterialInstance)
	{
		CompletionMaterialInstance->SetVectorParameterValue("ID", FVector4(ID.X, ID.Y, 0, 0));
		CompletionMaterialInstance->SetScalarParameterValue("RTSize", CompletionStateRT->SizeX);
		CompletionMaterialInstance->SetTextureParameterValue("ObjectMask", ObjectMask);
		
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), CompletionStateRT, CompletionMaterialInstance);
	}
}

float APaintGameManager::GetPercentCompletionValue(FVector2D ID, float TargetPercentage)
{
	FColor value = UKismetRenderingLibrary::ReadRenderTargetPixel(GetWorld(), CompletionStateRT, ID.X, ID.Y);
	return value.R / TargetPercentage;
}

float APaintGameManager::GetRawAmountPainted(FVector2D ID)
{
	FColor value = UKismetRenderingLibrary::ReadRenderTargetPixel(GetWorld(), CompletionStateRT, ID.X, ID.Y);
	return value.R;
}

void APaintGameManager::AddRTToUpdatePool(UPaintableActorComponent* PaintComponent)
{
	RTToUpdatePool.AddUnique(PaintComponent);
}

void APaintGameManager::ProcessRTPool()
{
	for (int i = 0; i < RTToUpdatePool.Num(); i++)
	{
		UpdateCompletionStateRT(RTToUpdatePool[i]->CompletionPercentID, RTToUpdatePool[i]->GetRenderTarget());
		
		float percentPainted = GetPercentCompletionValue(RTToUpdatePool[i]->CompletionPercentID, RTToUpdatePool[i]->MaxPercentPaintedAmount);
		UE_LOG(LogTemp, Warning, TEXT("%f"), percentPainted);
		
		RTToUpdatePool.RemoveAt(i);
		i--;
	}
}

void APaintGameManager::SetMaxPercentCompletionValue()
{
	for (int i = 0; i < PaintableObjects.Num(); i++)
	{
		PaintableObjects[i]->MaxPercentPaintedAmount = GetRawAmountPainted(PaintableObjects[i]->CompletionPercentID);
	}

	UE_LOG(LogTemp, Warning, TEXT("Remember to save simulation changes in editor!"))
}

