// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UE_StickArena.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UE_STICKARENA_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	void SetInputDirection(float x, float y);
	void PlayPunchAttack();
	void PlaySwordAttack();
	UFUNCTION()
	void AnimNotify_HitboxOn();
	UFUNCTION()
	void AnimNotify_EnableInput();
	void InitComboIdx();

protected:
	UPROPERTY(BlueprintReadOnly, Category="Movement")
	float axisX = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category="Movement")
	float axisY = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Montage")
	TObjectPtr<UAnimMontage> punchMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Montage")
	TObjectPtr<UAnimMontage> SwordMontage;

private:
	UPROPERTY()
	TObjectPtr<APawn> OwnerPawn;
	bool bAttackEnabled;
	UINT currentIdx = 0;
};
