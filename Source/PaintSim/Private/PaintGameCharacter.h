// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "PaintGameCharacter.generated.h"

class APaintGameManager;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

UCLASS()
class APaintGameCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APaintGameCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PawnClientRestart() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = "Paint") float RayWidth;
	UPROPERTY(EditAnywhere, Category = "Paint") uint8 RayResolution;

private:
	UFUNCTION() FVector GetCameraForwardDir() const;
	UFUNCTION() FVector GetCameraRightDir() const;
	UFUNCTION() void MoveCharacter(const FInputActionValue& value);
	UFUNCTION() void Look(const FInputActionValue& value);
	UFUNCTION() void Paint(const FInputActionValue& value);
	UFUNCTION() void PaintCharacterJump(const FInputActionValue& value);

	//Render Texture Pooling
	UFUNCTION() void TriggerRTUpdates(const FInputActionValue& value);
	
	// Input
	UPROPERTY(EditAnywhere, Category="Input") UInputMappingContext* MappingContext;
	UPROPERTY(EditAnywhere, Category="Input") UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category="Input") UInputAction* LookAction;
	UPROPERTY(EditAnywhere, Category="Input") UInputAction* PaintAction;
	UPROPERTY(EditAnywhere, Category="Input") UInputAction* JumpAction;
	
	UPROPERTY(EditAnywhere, Category = "Paint") APaintGameManager* PaintGameManager;
	UPROPERTY(EditAnywhere, Category = "Paint") float SprayDistance;
	// Camera
	UPROPERTY(VisibleAnywhere) UCameraComponent* GameCamera;
};
