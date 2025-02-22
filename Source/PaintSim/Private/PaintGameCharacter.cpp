// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintGameCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "MovieSceneTracksComponentTypes.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APaintGameCharacter::APaintGameCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GameCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Game Camera"));
}

// Called when the game starts or when spawned
void APaintGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerCameraManager* CameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)))
	{
		CameraManager->StartCameraFade(1.25f, 0.0f, 3.0f, FLinearColor::Black, false, false);
	}
	
	
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

	//Attach camera to mesh
	GameCamera->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	GameCamera->bUsePawnControlRotation = true;
	bUseControllerRotationPitch = true;
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
		enhancedInputComponent->BindAction(PaintAction, ETriggerEvent::Started, this, &APaintGameCharacter::Paint);
		enhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APaintGameCharacter::PaintCharacterJump);
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
	FHitResult Hit;
	//FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnFaceIndex = true;
	

	//GetWorld()->LineTraceSingleByChannel(OUT Hit, GameCamera->GetComponentLocation(), GetCameraForwardDir() * 100, ECollisionChannel::ECC_EngineTraceChannel1, TraceParams);
	bool paintableObjectHit = GetWorld()->LineTraceSingleByChannel(Hit, GameCamera->GetComponentLocation(), GetCameraForwardDir() * 100000, ECC_WorldDynamic, TraceParams, FCollisionResponseParams());
	//AActor* ActorHit = Hit.GetActor();
	if (paintableObjectHit && Hit.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *(Hit.GetActor()->GetName()));

		FVector2D UV(0.0f, 0.0f);
		UGameplayStatics::FindCollisionUV(Hit, 0, UV);

		UE_LOG(LogTemp, Warning, TEXT("X: %f Y: %f"), UV.X, UV.Y);
	}
	//UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *(ActorHit->GetName()));
}

void APaintGameCharacter::PaintCharacterJump(const FInputActionValue& value)
{
	APaintGameCharacter::Jump();
}

