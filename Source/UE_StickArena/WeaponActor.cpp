// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponActor.h"
#include "PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Distributions/DistributionFloatConstant.h"
#include "Distributions/DistributionVectorUniform.h"
#include "Materials/MaterialInterface.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSpriteEmitter.h"
#include "Particles/ParticleLODLevel.h"
#include "Particles/ParticleModuleRequired.h"
#include "Particles/Spawn/ParticleModuleSpawn.h"
#include "Particles/Lifetime/ParticleModuleLifetime.h"
#include "Particles/Size/ParticleModuleSize.h"
#include "Particles/Location/ParticleModuleLocation.h"
#include "Particles/Velocity/ParticleModuleVelocity.h"
#include "Particles/Color/ParticleModuleColor.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	UDistributionFloatConstant* MakeGlowFloat(UObject* Outer, float Value)
	{
		auto* Distribution = NewObject<UDistributionFloatConstant>(Outer);
		Distribution->Constant = Value;
		Distribution->bCanBeBaked = false;
		return Distribution;
	}

	UDistributionVectorUniform* MakeGlowVector(UObject* Outer, const FVector& Min, const FVector& Max)
	{
		auto* Distribution = NewObject<UDistributionVectorUniform>(Outer);
		Distribution->Min = Min;
		Distribution->Max = Max;
		Distribution->bCanBeBaked = false;
		return Distribution;
	}

}

// Sets default values
AWeaponActor::AWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	TriggerBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBoxComponent"));
	TriggerBoxComponent->SetupAttachment(RootComponent);
	TriggerBoxComponent->SetBoxExtent(FVector(75.0f));
	TriggerBoxComponent->SetCollisionProfileName(TEXT("Item"));
	TriggerBoxComponent->SetGenerateOverlapEvents(true);
	TriggerBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AWeaponActor::OnTriggerBoxBeginOverlap);

	GlowParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("GlowParticleComponent"));
	GlowParticleComponent->SetupAttachment(RootComponent);
	GlowParticleComponent->bAutoActivate = false;
	GlowParticleComponent->bAutoDestroy = false;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> GlowMaterial(
		TEXT("/Engine/EngineMaterials/DefaultParticle.DefaultParticle"));
	if (GlowMaterial.Succeeded())
	{
		GlowParticleMaterial = GlowMaterial.Object;
	}
}

// Called when the game starts or when spawned
void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	InitializeGlowParticles();
	SetWeaponState(WeaponState);
}

void AWeaponActor::InitializeGlowParticles()
{
	// Keep any particle asset selected on the component in the editor.
	if (GlowParticleComponent->Template)
	{
		return;
	}

	auto* System = NewObject<UParticleSystem>(this, NAME_None, RF_Transient);
	auto* Emitter = NewObject<UParticleSpriteEmitter>(System);
	System->Emitters.Add(Emitter);
	System->LODDistances.Add(0.0f);
	Emitter->CreateLODLevel(0);
	UParticleLODLevel* LOD = Emitter->LODLevels[0];
	LOD->RequiredModule->Material = GlowParticleMaterial;
	LOD->RequiredModule->bUseLocalSpace = true;
	LOD->RequiredModule->bKillOnDeactivate = true;
	LOD->RequiredModule->EmitterLoops = 0;
	LOD->SpawnModule->Rate.Distribution = MakeGlowFloat(LOD->SpawnModule, 12.0f);

	auto* Lifetime = NewObject<UParticleModuleLifetime>(System);
	Lifetime->Lifetime.Distribution = MakeGlowFloat(Lifetime, 1.5f);
	LOD->Modules.Add(Lifetime);

	auto* Size = NewObject<UParticleModuleSize>(System);
	Size->StartSize.Distribution = MakeGlowVector(Size, FVector(3.0f), FVector(6.0f));
	LOD->Modules.Add(Size);

	auto* Location = NewObject<UParticleModuleLocation>(System);
	Location->StartLocation.Distribution = MakeGlowVector(Location, FVector(-20.0f, -20.0f, 0.0f), FVector(20.0f, 20.0f, 35.0f));
	LOD->Modules.Add(Location);

	auto* Velocity = NewObject<UParticleModuleVelocity>(System);
	Velocity->StartVelocity.Distribution = MakeGlowVector(Velocity, FVector(-3.0f, -3.0f, 8.0f), FVector(3.0f, 3.0f, 18.0f));
	Velocity->StartVelocityRadial.Distribution = MakeGlowFloat(Velocity, 0.0f);
	LOD->Modules.Add(Velocity);

	auto* Color = NewObject<UParticleModuleColor>(System);
	Color->StartColor.Distribution = MakeGlowVector(Color, FVector(5.0f, 3.0f, 0.5f), FVector(5.0f, 3.0f, 0.5f));
	Color->StartAlpha.Distribution = MakeGlowFloat(Color, 1.0f);
	LOD->Modules.Add(Color);

	for (UParticleModule* Module : LOD->Modules)
	{
		Module->LODValidity = 1;
	}
	Emitter->UpdateModuleLists();
	GlowParticleComponent->SetTemplate(System);
}

void AWeaponActor::SetWeaponState(EWeaponState NewState)
{
	WeaponState = NewState;
	const bool bShouldTick = WeaponState == EWeaponState::UNEQUIPED;
	// SetActorTickEnabled requires bCanEverTick to be true, including when re-enabling.
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(bShouldTick);
	PrimaryActorTick.bCanEverTick = bShouldTick;
	TriggerBoxComponent->SetGenerateOverlapEvents(bShouldTick);
	TriggerBoxComponent->SetCollisionProfileName(TEXT("Item"));

	if (bShouldTick)
	{
		if (!GlowParticleComponent->IsActive())
		{
			GlowParticleComponent->SetVisibility(true);
			GlowParticleComponent->Activate(true);
		}
	}
	else
	{
		GlowParticleComponent->DeactivateImmediate();
		GlowParticleComponent->SetVisibility(false);
	}
}

void AWeaponActor::OnTriggerBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (WeaponState != EWeaponState::UNEQUIPED) return;

	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Display, TEXT("Equipped"));
		SetWeaponState(EWeaponState::EQUIPED);
		PlayerCharacter->SetWeaponActor(this);

	}
}

void AWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	switch(WeaponState)
	{
	case EWeaponState::UNEQUIPED:
		FRotator Rotation = GetActorRotation();
		Rotation.Yaw += RotationSpeed * DeltaTime;
		SetActorRotation(Rotation);
		break;
	}
}
