// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UE_StickArena.h"
#include "GameFramework/Actor.h"
#include "WeaponActor.generated.h"

UCLASS()
class UE_STICKARENA_API AWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponActor();
	EWeaponType GetWeponType() { return WeaponType; }
	float GetDamage() { return Damage; }
	float GetMoveSpeed() { return MoveSpeed; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
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
