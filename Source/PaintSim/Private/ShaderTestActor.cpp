// Fill out your copyright notice in the Description page of Project Settings.


#include "ShaderTestActor.h"

#include "HairStrandsInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/Material.h"

// Sets default values
AShaderTestActor::AShaderTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PercentageValue = 0;
}

// Called when the game starts or when spawned
void AShaderTestActor::BeginPlay()
{
	Super::BeginPlay();
	DynamicPaintMaterialInstance = UMaterialInstanceDynamic::Create(PaintMaterial, this);
	//DynamicPaintMaterialInstance->SetScalarParameterValue("PercentCompletion", 0.5f);
	GetComponentByClass<UStaticMeshComponent>()->SetMaterial(0, DynamicPaintMaterialInstance);
}

// Called every frame
void AShaderTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DynamicPaintMaterialInstance->GetScalarParameterValue(FName("PercentCompletion"), PercentageValue);
	
	UE_LOG(LogTemp, Warning, TEXT("TestValue: %f"), PercentageValue);
}

