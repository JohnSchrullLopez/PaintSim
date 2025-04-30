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
	MinPercentToCountAsComplete = .9;
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

	GetWorld()->GetTimerManager().SetTimer(RTUpdateTimerHandle, this, &APaintGameManager::TimerTest, RTUpdateFrequency, true);
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
	if (PaintableObjects.Contains(PaintComponent))
	{
		RTUpdateQueue.AddUnique(PaintComponent);	
	}
}

void APaintGameManager::ProcessRTPool()
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ProcessPool");
	for (int i = 0; i < RTUpdateQueue.Num(); i++)
	{
		UpdateCompletionStateRT(RTUpdateQueue[i]->CompletionPercentID, RTUpdateQueue[i]->GetRenderTarget());
		
		float percentPainted = GetPercentCompletionValue(RTUpdateQueue[i]->CompletionPercentID, RTUpdateQueue[i]->MaxPercentPaintedAmount);
		if (FMath::IsWithin(percentPainted, MinPercentToCountAsComplete, 2.0f))
		{
			RTUpdateQueue[i]->GetBasePaintMaterial()->SetScalarParameterValue("IsFullyPainted", 1.0f);
			PaintableObjects.Remove(RTUpdateQueue[i]);
			
			if (PaintableObjects.Num() <= 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("ALL OBJECT PAINTED"));
			}
		}

		for (int j = 0; j < PaintableObjects.Num(); j++)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *PaintableObjects[j]->GetOwner()->GetActorNameOrLabel());
		}
		
		RTUpdateQueue.RemoveAt(i);
		i--;
	}
}

void APaintGameManager::CheckForGameCompletion()
{
	if (PaintableObjects.Num() <= 0)
	{
		GEngine->AddOnScreenDebugMessage(1, 10, FColor::Red, TEXT("ALL OBJECTS PAINTED"));
	}
	/*
	else
	{
		for (int i = 0; i < PaintableObjects.Num(); i++)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *PaintableObjects[i]->GetOwner()->GetActorNameOrLabel());
		}
	}
	*/
}

void APaintGameManager::SetMaxPercentCompletionValue()
{
	for (int i = 0; i < PaintableObjects.Num(); i++)
	{
		PaintableObjects[i]->MaxPercentPaintedAmount = GetRawAmountPainted(PaintableObjects[i]->CompletionPercentID);
	}

	UE_LOG(LogTemp, Warning, TEXT("Remember to save simulation changes in editor!"))
}

void APaintGameManager::TimerTest()
{
	if (RTUpdateQueue.Num() <= 0 || PaintableObjects.Num() <= 0) return;

	UpdateCompletionStateRT(RTUpdateQueue[0]->CompletionPercentID, RTUpdateQueue[0]->GetRenderTarget());
	
	float percentPainted = GetPercentCompletionValue(RTUpdateQueue[0]->CompletionPercentID, RTUpdateQueue[0]->MaxPercentPaintedAmount);
	if (FMath::IsWithin(percentPainted, MinPercentToCountAsComplete, 200.0f))
	{
		int indexInPaintableObjects = PaintableObjects.Find(RTUpdateQueue[0]);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *PaintableObjects[indexInPaintableObjects]->GetOwner()->GetActorNameOrLabel());
		
		RTUpdateQueue[0]->GetBasePaintMaterial()->SetScalarParameterValue("IsFullyPainted", 1.0f);
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("%s PAINTED"), *RTUpdateQueue[0]->GetOwner()->GetActorNameOrLabel()));
		
		PaintableObjects.RemoveAt(indexInPaintableObjects);
		UE_LOG(LogTemp, Warning, TEXT("%i"), PaintableObjects.Num());
		CheckForGameCompletion();
	}
	
	RTUpdateQueue.RemoveAt(0);
}

