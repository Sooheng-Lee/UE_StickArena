// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "DrawDebugHelpers.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	bAttackEnabled = true;
}

void UPlayerAnimInstance::SetInputDirection(float x, float y)
{
	axisX = x;
	axisY = y;
}

void UPlayerAnimInstance::PlayPunchAttack()
{
	if (!punchMontage || !SwordMontage) return;
	if (!bAttackEnabled) return;

	bAttackEnabled = false;

	if (currentIdx==0)
	{
		Montage_Play(punchMontage, 1.0f);
		Montage_JumpToSection(FName("ComboA"), punchMontage);
		currentIdx = 1;
	}
	else
	{
		Montage_Play(punchMontage, 1.0f);
		Montage_JumpToSection(FName("ComboB"), punchMontage);
		currentIdx = 0;
	}

	
}

void UPlayerAnimInstance::PlaySwordAttack()
{
	if (!SwordMontage) return;
	if (!bAttackEnabled) return;
	bAttackEnabled = false;

	if (currentIdx == 0)
	{
		Montage_Play(SwordMontage, 1.0f);
		Montage_JumpToSection(FName("ComboA"), SwordMontage);
		currentIdx = 1;
	}
	else
	{
		Montage_Play(SwordMontage, 1.0f);
		Montage_JumpToSection(FName("ComboB"), SwordMontage);
		currentIdx = 0;
	}
}

void UPlayerAnimInstance::AnimNotify_HitboxOn()
{
	APawn* pawn = TryGetPawnOwner();
	if (!OwnerPawn) nullptr;

	FHitResult hitResult;
	FVector start = pawn->GetActorLocation();
	FVector forward = pawn->GetActorForwardVector();
	FVector end = start + forward * 100.0f;

	FCollisionQueryParams params;
	params.AddIgnoredActor(pawn);

	bool bHit = GetWorld()->SweepSingleByChannel(
		hitResult,
		start,
		end,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(20.0f),
		params
	);

	if (bHit)
	{
		AActor* hitActor = hitResult.GetActor();
		if (hitActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor : %s"), *hitActor->GetName());
			DrawDebugSphere(
				GetWorld(),
				hitResult.ImpactPoint,
				20.f,
				12,
				FColor::Green,
				false,
				0.5f
			);
		}
	}
}

void UPlayerAnimInstance::AnimNotify_HitboxOff()
{
	UE_LOG(LogTemp, Warning, TEXT("Hitbox Off"));
}

void UPlayerAnimInstance::AnimNotify_EnableInput()
{
	bAttackEnabled = true;
}

void UPlayerAnimInstance::InitComboIdx()
{
	currentIdx = 0;
}
