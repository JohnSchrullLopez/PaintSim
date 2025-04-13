// Fill out your copyright notice in the Description page of Project Settings.

//TODO: Create Dynamic Material Instance for hit materials

#include "PaintGameCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "PaintableActorComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Managers/PaintGameManager.h"

// Sets default values
APaintGameCharacter::APaintGameCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//GameCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Game Camera"));
	GameCamera = GetComponentByClass<UCameraComponent>();
	if( GameCamera)
	{
		UE_LOG(LogTemp, Warning, TEXT("CAMERA INITIALIZED"));
	}

	RayWidth = 5.0f;
	RayResolution = 10;
}

// Called when the game starts or when spawned
void APaintGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerCameraManager* CameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)))
	{
		CameraManager->StartCameraFade(1.25f, 0.0f, 3.0f, FLinearColor::Black, false, false);
	}

	//Attach camera to mesh
	GameCamera->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	GameCamera->bUsePawnControlRotation = true;
	bUseControllerRotationPitch = true;
	PaintGameManager = Cast<APaintGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APaintGameManager::StaticClass()));
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
		enhancedInputComponent->BindAction(PaintAction, ETriggerEvent::Completed, this, &APaintGameCharacter::TriggerRTUpdates);
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
	FVector cameraLocation = GameCamera->GetComponentLocation();
	FVector rayDirection_L = GameCamera->GetForwardVector().GetSafeNormal();
	FVector2D UV(0.0f, 0.0f);
	bool Ray;
	FVector rayDirection;
	float offset = (RayWidth * 2) / RayResolution;
	
	for (int i = 0; i < RayResolution; i++)
	{
		UV = FVector2D::ZeroVector;

		rayDirection = rayDirection_L.RotateAngleAxis(-RayWidth + offset * i, GameCamera->GetUpVector());
		
		Ray = GetWorld()->LineTraceSingleByChannel(Hit, cameraLocation, cameraLocation + rayDirection * 1000, ECC_WorldDynamic, TraceParams);
		UGameplayStatics::FindCollisionUV(Hit, 0, UV);
		AActor* HitActor = Hit.GetActor();
		
		if (Ray && HitActor)
		{
			UPaintableActorComponent* PaintComponent = HitActor->GetComponentByClass<UPaintableActorComponent>();
			if (PaintComponent)
			{
				float scale = PaintComponent->ObjectScale;
				PaintComponent->OnPaintHit(UV, 1 / scale);
				PaintGameManager->AddRTToUpdatePool(PaintComponent);
			}	
		}
	}
}

void APaintGameCharacter::PaintCharacterJump(const FInputActionValue& value)
{
	APaintGameCharacter::Jump();
}

void APaintGameCharacter::TriggerRTUpdates(const FInputActionValue& value)
{
	PaintGameManager->ProcessRTPool();
}

