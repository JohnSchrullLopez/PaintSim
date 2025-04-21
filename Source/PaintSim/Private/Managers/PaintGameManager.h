// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaintGameManager.generated.h"

class UMaxPossiblePercentPainted;
class UPaintableActorComponent;

UCLASS()
class APaintGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APaintGameManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector2D RegisterPaintableObject(UPaintableActorComponent* PaintComponent);
	UFUNCTION(BlueprintCallable) void UpdateCompletionStateRT(FVector2D ID, UTextureRenderTarget2D* ObjectMask);
	UFUNCTION(BlueprintCallable) float GetPercentCompletionValue(FVector2D ID, float TargetPercentage);
	UFUNCTION(BlueprintCallable) float GetRawAmountPainted(FVector2D ID);
	UFUNCTION() void AddRTToUpdatePool(UPaintableActorComponent* PaintComponent);
	UFUNCTION() void ProcessRTPool();
	
	UFUNCTION(CallInEditor, Category="Paint Game Utilities") void SetMaxPercentCompletionValue();

	UPROPERTY(EditAnywhere, Category="Paint") UMaterial* CalculationMaterialBase;
	UPROPERTY(EditAnywhere, Category="Paint") UTextureRenderTarget2D* CompletionStateRT;
	UPROPERTY(EditAnywhere, Category="Paint") UMaxPossiblePercentPainted* MaxPossiblePercentMap;
	UPROPERTY() TArray<UPaintableActorComponent*> PaintableObjects;
	UPROPERTY() TArray<UPaintableActorComponent*> RTToUpdatePool;

private:
	int CurrentID = 0;
	UPROPERTY() UMaterialInstanceDynamic* CompletionMaterialInstance;
};
