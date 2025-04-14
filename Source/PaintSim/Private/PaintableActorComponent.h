// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PaintableActorComponent.generated.h"


class APaintGameManager;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class UPaintableActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPaintableActorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION() void InitializeComponent() override;

	UPROPERTY(VisibleAnywhere) UTextureRenderTarget2D* RenderTarget;
	UPROPERTY() UMaterialInstanceDynamic* PaintableObjectMaterial;
	UPROPERTY() UMaterialInstanceDynamic* PaintMaterial;
	UPROPERTY() FVector2D HitUV;
	UPROPERTY() UStaticMeshComponent* Mesh;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION() void OnPaintHit(FVector2D UV, float ScaleFactor);
	UFUNCTION() float GetPercentPainted();
	FORCEINLINE void SetID(const FVector2D ID) { CompletionPercentID = ID; }
	FORCEINLINE APaintGameManager* GetPaintGameManager() { return PaintGameManager; }
	FORCEINLINE UTextureRenderTarget2D* GetRenderTarget() { return RenderTarget; }
	
	UPROPERTY(EditAnywhere) UTexture2D* BaseTexture;
	UPROPERTY(EditAnywhere) UTexture2D* ORMTexture;
	UPROPERTY(EditAnywhere) UTexture2D* NormalTexture;
	UPROPERTY(EditAnywhere) UTexture2D* OpacityMask;
	UPROPERTY(EditAnywhere) UMaterial* BasePaintableMaterial;
	UPROPERTY(EditAnywhere) UMaterial* BasePaintMaterial;
	UPROPERTY() APaintGameManager* PaintGameManager;
	UPROPERTY(VisibleAnywhere) FVector2D CompletionPercentID;
	UPROPERTY() float ObjectScale = 1;
};
