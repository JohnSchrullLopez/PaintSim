// Fill out your copyright notice in the Description page of Project Settings.

//TODO: Create Dynamic Material Instance for hit materials

#include "PaintGameCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "PaintableActorComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APaintGameCharacter::APaintGameCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

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
		enhancedInputComponent->BindAction(PaintAction, ETriggerEvent::Triggered, this, &APaintGameCharacter::Paint);
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
	
	//Trace Parameters 
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnFaceIndex = true;
	TraceParams.TraceTag = FName("DebugRay");

	GetWorld()->DebugDrawTraceTag = FName("DebugRay");

	//Get Camera View Vector
	FRotator rotation(GameCamera->GetComponentToWorld().GetRotation());
	
	FVector rayDirection_L = FRotationMatrix(rotation).GetScaledAxis(EAxis::X);
	FVector rayDirection_R = FRotationMatrix(rotation).GetScaledAxis(EAxis::X);

	//Find and store UV from collision
	FVector2D UV_L(0.0f, 0.0f);
	FVector2D UV_R(0.0f, 0.0f);

	FVector cameraLocation = GameCamera->GetComponentLocation();
	
	bool Ray_L = GetWorld()->LineTraceSingleByChannel(Hit, cameraLocation, rayDirection_L * 1000, ECC_WorldDynamic, TraceParams, FCollisionResponseParams());
	UGameplayStatics::FindCollisionUV(Hit, 0, UV_L);
	
	bool Ray_R = GetWorld()->LineTraceSingleByChannel(Hit, cameraLocation, rayDirection_R * 1000, ECC_WorldDynamic, TraceParams, FCollisionResponseParams());
	UGameplayStatics::FindCollisionUV(Hit, 0, UV_R);

	if (Ray_L && Ray_R && Hit.GetActor())
	{
		if (Hit.GetActor()->GetComponentByClass<UPaintableActorComponent>())
		{
			//UE_LOG(LogTemp, Display, TEXT("PAINT COMPONENT FOUND"));
			Hit.GetActor()->GetComponentByClass<UPaintableActorComponent>()->OnPaintHit(UV_L, UV_R);
		}
		//UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *(Hit.GetActor()->GetName()));

		//Get Material from collision
		if (UMaterialInterface* hitMaterial = Hit.GetComponent()->GetMaterial(0))
		{
			//Check if material is dynamic instance
			if (UMaterialInstanceDynamic* HitDIM = Cast<UMaterialInstanceDynamic>(hitMaterial))
			{
				//Send collision UV to shader
				//UE_LOG(LogTemp, Warning, TEXT("X: %s"), *HitDIM->GetName());
				//HitDIM->SetVectorParameterValue("CustomUV", FVector(HitUV.X, HitUV.Y, 0.0f));
			}
			else
			{
				//UE_LOG(LogTemp, Warning, TEXT("Failed to set parameter"));
			}
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("Failed to get Material"));
		}
		//UE_LOG(LogTemp, Warning, TEXT("X: %f Y: %f"), UV.X, UV.Y);
	}
}

void APaintGameCharacter::PaintCharacterJump(const FInputActionValue& value)
{
	APaintGameCharacter::Jump();
}

