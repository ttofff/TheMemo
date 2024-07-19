// Copyright Epic Games, Inc. All Rights Reserved.

#include "TheMemoCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Runtime/Core/Public/Misc/DateTime.h"
#include "Kismet/KismetStringLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ATheMemoCharacter

ATheMemoCharacter::ATheMemoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->GravityScale = 0.f;
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

FString ATheMemoCharacter::GetDayOfWeek()
{
	FDateTime Time = FDateTime::Now();
	FString CAndYear = FString::FromInt(Time.GetYear());
	FString Century = UKismetStringLibrary::GetSubstring(CAndYear, 0, 2);
	FString Y = UKismetStringLibrary::GetSubstring(CAndYear, 2, 4);
	int32 C = FCString::Atoi(*Century);
	int32 Year = FCString::Atoi(*Y);
	int32 Month = Time.GetMonth();
	int32 Day = Time.GetDay();

	int32 W = (static_cast<int>(C / 4.0) - 2 * C + Year + static_cast<int>(Year / 4.0) + static_cast<int>(26 * (Month + 1) / 10.0) + Day - 1)%7;

	FString Weekday;
	
	if (W) {
		switch(W) {
		case 0 :
			Weekday = TEXT("周日");
			//此处也可写为Weekday = L("星期日");
			break;
		case 1:
			Weekday = TEXT("周一");
			break;
		case 2:
			Weekday = TEXT("周二");
			break;
		case 3:
			Weekday = TEXT("周三");
			break;
		case 4:
			Weekday = TEXT("周四");
			break;
		case 5:
			Weekday = TEXT("周五");
			break;
		case 6:
			Weekday = TEXT("周六");
			break;
		}
	}
	return Weekday;
}

float ATheMemoCharacter::GetDistance(float Lat1, float Lon1, float Lat2, float Lon2)
{
	// 将经纬度转换为弧度
	float DegToRad = PI / 180.0f;
	float RadToDeg = 180.0f / PI;
	float R = 6371.0f; // 地球半径（单位：公里）

	float Lat1Rad = Lat1 * DegToRad;
	float Lon1Rad = Lon1 * DegToRad;
	float Lat2Rad = Lat2 * DegToRad;
	float Lon2Rad = Lon2 * DegToRad;

	// 使用球面三角法计算距离
	float DeltaLat = Lat2Rad - Lat1Rad;
	float DeltaLon = Lon2Rad - Lon1Rad;
	float A = FMath::Square(FMath::Sin(DeltaLat / 2.0f)) + FMath::Cos(Lat1Rad) * FMath::Cos(Lat2Rad) * FMath::Square(FMath::Sin(DeltaLon / 2.0f));
	float C = 2.0f * FMath::Atan2(FMath::Sqrt(A), FMath::Sqrt(1.0f - A));
	float Distance = R * C;

	return Distance;
}

void ATheMemoCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATheMemoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {}
}

