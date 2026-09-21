// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "UE_StickArena.h"
#include "PlayerAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class UE_STICKARENA_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	EWeaponType GetWeponType() const { return WeaponType; };

private:
	void RotateToMouse(float DeltaTime);
	void MovePlayer(const FInputActionInstance& instance);
	void Attack(const FInputActionInstance& instance);

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USpringArmComponent> springArm;
	TObjectPtr<UCameraComponent> camera;
	UPlayerAnimInstance* animInstance;
	UPROPERTY(BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> playerInputMapping;
	UPROPERTY(BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> moveInputAction;
	UPROPERTY(BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> attackInputAction;
	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType = EWeaponType::SWORD;

	EPlayerState playerState;

private:
	FVector forwardVector;
	FVector rightVector;
	FVector currentInput;
	float moveSpeed = 50.0f;
	float rotSpeed = 8.5f;
};