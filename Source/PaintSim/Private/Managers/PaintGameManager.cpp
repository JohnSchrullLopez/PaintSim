// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/PaintGameManager.h"

#include "PaintableActorComponent.h"
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
	
	UE_LOG(LogTemp, Warning, TEXT("%s registered"), *PaintComponent->GetName());

	//Convert 1D ID value to 2D
	FVector2D GeneratedID = FVector2D(CurrentID % CompletionStateRT->SizeX, std::floor(CurrentID / CompletionStateRT->SizeX));
	CurrentID++;
	return GeneratedID;
}

void APaintGameManager::UpdateCompletionStateRT(FVector2D ID, UTextureRenderTarget2D* ObjectMask)
{
	if (CompletionStateRT && CompletionMaterialInstance)
	{
		CompletionMaterialInstance->SetVectorParameterValue("ID", FVector(ID.X, ID.Y, 0));
		CompletionMaterialInstance->SetScalarParameterValue("RTSize", CompletionStateRT->SizeX);
		CompletionMaterialInstance->SetTextureParameterValue("ObjectMask", ObjectMask);
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), CompletionStateRT, CompletionMaterialInstance);	
	}
}

float APaintGameManager::GetPercentCompletionValue(FVector2D ID)
{
	FColor value = UKismetRenderingLibrary::ReadRenderTargetPixel(GetWorld(), CompletionStateRT, ID.X, ID.Y);
	return value.R;
}

