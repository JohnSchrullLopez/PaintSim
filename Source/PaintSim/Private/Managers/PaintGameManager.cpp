// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/PaintGameManager.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"

// Sets default values
APaintGameManager::APaintGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CompletionRTIndex = 0;
}

// Called when the game starts or when spawned
void APaintGameManager::BeginPlay()
{
	Super::BeginPlay();

	CompletionStateRT->Filter = TF_Trilinear;
	CompletionStateRT->MipsSamplerFilter = TF_Trilinear;
	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), CompletionStateRT, FLinearColor(1,1,1,1));
	
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

TTuple<int, int> APaintGameManager::RegisterPaintableObject(UPaintableActorComponent* PaintComponent)
{
	return TTuple<int, int>(0,0);
}

void APaintGameManager::UpdateCompletionStateRT(int ID, UTextureRenderTarget2D* ObjectMask)
{
	if (CompletionStateRT && CompletionMaterialInstance)
	{
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), CompletionStateRT, CompletionMaterialInstance);	
	}
}

