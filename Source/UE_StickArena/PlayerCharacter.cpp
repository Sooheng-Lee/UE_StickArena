// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "WeaponActor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Arm"));
	springArm->SetupAttachment(RootComponent);
	springArm->bUsePawnControlRotation = false;
	springArm->bInheritPitch = false;
	springArm->bInheritYaw = false;
	springArm->bInheritRoll = false;
	springArm->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));
	springArm->TargetArmLength = 800.0f;
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	camera->SetupAttachment(springArm);
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_Player(TEXT("/Game/Inputs/IMC_Player"));
	if (IMC_Player.Succeeded())
	{
		playerInputMapping = IMC_Player.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Move(TEXT("/Game/Inputs/IA_Movement"));
	if (IA_Move.Succeeded())
	{
		moveInputAction = IA_Move.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Attack(TEXT("/Game/Inputs/IA_Attack"));
	if (IA_Attack.Succeeded())
	{
		attackInputAction = IA_Attack.Object;
	}

	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetRelativeScale3D(FVector(100.0f, 100.0f, 100.0f));
	currentInput = FVector::Zero();
	playerState = EPlayerState::IDLE;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* controller = Cast<APlayerController>(GetController());
	if (!controller) return;
	FInputModeGameAndUI inputMode;
	inputMode.SetHideCursorDuringCapture(false);
	inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	controller->SetInputMode(inputMode);
	controller->bShowMouseCursor = true;
	ULocalPlayer* localPlayer = controller->GetLocalPlayer();
	if (!localPlayer) return;

	UE_LOG(LogTemp, Warning, TEXT("Hello 0"));
	UEnhancedInputLocalPlayerSubsystem* inputSubsystem = localPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (inputSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hello 1"));
		if (playerInputMapping == nullptr) return;
		inputSubsystem->AddMappingContext(playerInputMapping, 0);
		UE_LOG(LogTemp, Warning, TEXT("Hello 2"));
	}
	animInstance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	switch (playerState)
	{
	case EPlayerState::IDLE:
		break;
	case EPlayerState::ATTACK:
		RotateToMouse(DeltaTime);
		break;
	default:
		break;
	}

	if (!animInstance) return;
	// movement 입력이 없는 경우 IDLE animation으로 부드럽게 되돌리는 코드
	if (GetLastMovementInputVector().IsZero())
	{
		currentInput = currentInput.IsNearlyZero() ? FVector::Zero() : FMath::Lerp(currentInput, FVector::ZeroVector, 0.1f);
	}
	animInstance->SetInputDirection(currentInput.X, currentInput.Y);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (enhancedInputComponent != nullptr)
	{
		if (moveInputAction) {
			enhancedInputComponent->BindAction(moveInputAction, ETriggerEvent::Triggered, this, &APlayerCharacter::MovePlayer);
			enhancedInputComponent->BindAction(attackInputAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Attack);
		}
	}
}

void APlayerCharacter::RotateToMouse(float DeltaTime)
{
	APlayerController* controller = Cast<APlayerController>(GetController());
	if (controller == nullptr) return;

	FHitResult hitResult;
	if (controller->GetHitResultUnderCursor(
		ECC_Visibility,
		false,
		hitResult))
	{
		const FVector charLoc = GetActorLocation();
		const FVector cursorLoc = hitResult.ImpactPoint;

		FVector direction = cursorLoc - charLoc;
		direction.Z = 0.0f;
		if (direction.IsNearlyZero()) return;
		const FRotator targetRot = direction.Rotation();
		const FRotator newRot = FMath::RInterpTo(
			GetActorRotation(),
			FRotator(0.0f, targetRot.Yaw, 0.0f),
			DeltaTime,
			rotSpeed
		);
		SetActorRotation(newRot);
	}
}

void APlayerCharacter::MovePlayer(const FInputActionInstance& instance)
{
	FVector inputVector = instance.GetValue().Get<FVector>();
	switch (playerState)
	{
	case EPlayerState::IDLE:
		// 입력 방향에 대한 회전값 보정
		FRotator newRot = FMath::Lerp<FRotator>(GetActorRotation(), -1 * inputVector.Rotation() + FRotator(0.0f, 90.0f, 0.0f), 0.1f);
		SetActorRotation(newRot);
		if (!inputVector.IsZero()) currentInput = FMath::Lerp<FVector>(currentInput, FVector(0, 1, 0), 0.1f);
		AddMovementInput(GetActorForwardVector(), GetWorld()->DeltaTimeSeconds * moveSpeed);
		break;
	case EPlayerState::ATTACK:
		FRotator rot = GetActorRotation().GetNormalized();
		FVector MoveDir = FVector::Zero();
		if (-45.0f <= rot.Yaw && rot.Yaw < 45.0f)
		{
			MoveDir = GetActorForwardVector() * inputVector.Y + GetActorRightVector() * inputVector.X;
			currentInput = FVector::Distance(currentInput, inputVector) < 0.1f ? inputVector : FMath::Lerp<FVector>(currentInput, inputVector, 0.1f);
		}
		else if (45.0f <= rot.Yaw && rot.Yaw < 135.0f)
		{
			MoveDir = GetActorRightVector() * -inputVector.Y + GetActorForwardVector() * inputVector.X;
			currentInput = FVector::Distance(currentInput, inputVector) < 0.1f ? inputVector : FMath::Lerp<FVector>(currentInput, FVector(-inputVector.Y, inputVector.X, 0), 0.1f);
		}
		else if (135.0f <= rot.Yaw || rot.Yaw < -135.0f)
		{
			MoveDir = GetActorForwardVector() * -inputVector.Y + GetActorRightVector() * -inputVector.X;
			currentInput = FVector::Distance(currentInput, inputVector) < 0.1f ? inputVector : FMath::Lerp<FVector>(currentInput, FVector(inputVector.X, -inputVector.Y, 0), 0.1f);
		}
		else
		{
			MoveDir = GetActorRightVector() * inputVector.Y + GetActorForwardVector() * -inputVector.X;
			currentInput = FVector::Distance(currentInput, inputVector) < 0.1f ? inputVector : FMath::Lerp<FVector>(currentInput, FVector(inputVector.Y, -inputVector.X, 0), 0.1f);
		}
		MoveDir = MoveDir.GetSafeNormal();
		AddMovementInput(MoveDir, GetWorld()->DeltaTimeSeconds * moveSpeed);
		break;
	case EPlayerState::DEAD:
		break;
	}
	
}

void APlayerCharacter::Attack(const FInputActionInstance& instance)
{
	bool bPressed = instance.GetValue().Get<bool>();
	UE_LOG(LogTemp, Warning, TEXT("pressed : %d"), bPressed);
	switch (playerState)
	{
	case EPlayerState::IDLE:
		animInstance->InitComboIdx();
	case EPlayerState::ATTACK:
		playerState = bPressed ? EPlayerState::ATTACK : EPlayerState::IDLE;
		break;
	default:
		break;
	}

	switch (WeaponType)
	{
	case EWeaponType::NONE:
		animInstance->PlayPunchAttack();
		break;
	case EWeaponType::SWORD:
		animInstance->PlaySwordAttack();
		break;
	}
}
