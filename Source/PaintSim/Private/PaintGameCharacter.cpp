// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintGameCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APaintGameCharacter::APaintGameCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GameCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Game Camera"));
	GameCamera->bUsePawnControlRotation = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

// Called when the game starts or when spawned
void APaintGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerCameraManager* CameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)))
	{
		CameraManager->StartCameraFade(1.25f, 0.0f, 3.0f, FLinearColor::Black, false, false);
	}

	//GetMesh();
	//AttachToComponent
}

void APaintGameCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			subsystem->AddMappingContext(MappingContext, 0);
		}
	}
}

// Called every frame
void APaintGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APaintGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Action Binds
		enhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APaintGameCharacter::MoveCharacter);
		enhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APaintGameCharacter::Look);
		enhancedInputComponent->BindAction(PaintAction, ETriggerEvent::Triggered, this, &APaintGameCharacter::Paint);
	}
}

FVector APaintGameCharacter::GetCameraForwardDir() const
{
	FVector cameraFwd = GameCamera->GetForwardVector();
	cameraFwd.Z = 0;
	return cameraFwd.GetSafeNormal();
}

FVector APaintGameCharacter::GetCameraRightDir() const
{
	return GameCamera->GetRightVector();
}

void APaintGameCharacter::MoveCharacter(const FInputActionValue& value)
{
	FVector2D input = value.Get<FVector2D>();
	input.Normalize();
	AddMovementInput(input.Y * GetCameraForwardDir() + input.X * GetCameraRightDir());
}

void APaintGameCharacter::Look(const FInputActionValue& value)
{
	const FVector2D lookAxisVector = value.Get<FVector2D>();

	AddControllerPitchInput(lookAxisVector.Y);
	AddControllerYawInput(lookAxisVector.X);
}

void APaintGameCharacter::Paint(const FInputActionValue& value)
{
	return;
}

