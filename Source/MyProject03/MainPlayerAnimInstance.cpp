// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerAnimInstance.h"
#include"MyProject03/MainPlayer.h"
#include"GameFramework/CharacterMovementComponent.h"


void UMainPlayerAnimInstance::NativeInitializeAnimation()
{
	MainPlayerRef = Cast<AMainPlayer>(TryGetPawnOwner());
}

void UMainPlayerAnimInstance::UpdateAnimProperties()
{
	if (MainPlayerRef==nullptr)
	{
		MainPlayerRef = Cast<AMainPlayer>(TryGetPawnOwner());
	}
	if (MainPlayerRef)
	{
		FVector SpeedVector = MainPlayerRef->GetVelocity();
		FVector TwoDVector = FVector(SpeedVector.X, SpeedVector.Y, 0.0f);
		Speed = TwoDVector.Size();

		bIsInAir = MainPlayerRef->GetCharacterMovement()->IsFalling();
	}
}
