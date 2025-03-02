// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintableActorComponent.h"

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

	// ...
	
}


// Called every frame
void UPaintableActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPaintableActorComponent::OnPaintHit_Implementation(FVector2D HitUV)
{
	UE_LOG(LogTemp, Warning, TEXT("C++ FUNCTION CALLED"));
}
