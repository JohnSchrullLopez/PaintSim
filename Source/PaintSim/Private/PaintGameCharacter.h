// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "PaintGameCharacter.generated.h"

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

private:
	UFUNCTION() FVector GetCameraForwardDir() const;
	UFUNCTION() FVector GetCameraRightDir() const;
	UFUNCTION() void MoveCharacter(const FInputActionValue& value);
	UFUNCTION() void Look(const FInputActionValue& value);
	UFUNCTION() void Paint(const FInputActionValue& value);

	// Input
	UPROPERTY(EditAnywhere, Category="Input") UInputMappingContext* MappingContext;
	UPROPERTY(EditAnywhere, Category="Input") UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category="Input") UInputAction* LookAction;
	UPROPERTY(EditAnywhere, Category="Input") UInputAction* PaintAction;

	// Camera
	UPROPERTY(VisibleAnywhere, Category="Input") UCameraComponent* GameCamera;
};
