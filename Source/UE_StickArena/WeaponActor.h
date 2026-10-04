// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UE_StickArena.h"
#include "GameFramework/Actor.h"
#include "WeaponActor.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	UNEQUIPED UMETA(DisplayName = "Unequipped"),
	EQUIPED UMETA(DisplayName = "Equipped"),
};

UCLASS()
class UE_STICKARENA_API AWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponActor();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "WeaponInfo")
	EWeaponState GetWeaponState() const { return WeaponState; }

	UFUNCTION(BlueprintCallable, Category = "WeaponInfo")
	void SetWeaponState(EWeaponState NewState);

	EWeaponType GetWeaponType() { return WeaponType; }
	float GetDamage() { return Damage; }
	float GetMoveSpeed() { return MoveSpeed; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void InitializeGlowParticles();

	UFUNCTION()
	void OnTriggerBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WeaponTrigger", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* TriggerBoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WeaponEffects", meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* GlowParticleComponent;

	UPROPERTY(EditDefaultsOnly, Category = "WeaponEffects")
	class UMaterialInterface* GlowParticleMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponInfo", meta = (AllowPrivateAccess = "true"))
	EWeaponState WeaponState = EWeaponState::UNEQUIPED;

	// Unequipped yaw rotation speed in degrees per second.
	UPROPERTY(EditAnywhere, Category = "WeaponInfo")
	float RotationSpeed = 30.0f;

	UPROPERTY(EditDefaultsOnly, Category="SceneComponent")
	USceneComponent* SceneComponent;
	UPROPERTY(EditDefaultsOnly, Category="StaticMesh")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category="WeaponInfo")
	EWeaponType WeaponType;
	UPROPERTY(EditDefaultsOnly, Category = "WeaponInfo")
	float Damage;
	UPROPERTY(EditDefaultsOnly, Category = "WeaponInfo")
	float MoveSpeed;
};
