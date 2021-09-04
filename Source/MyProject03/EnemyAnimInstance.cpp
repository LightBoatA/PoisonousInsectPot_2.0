// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include"MyProject03/Enemy.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	EnemyRef = Cast<AEnemy>(TryGetPawnOwner());
}

void UEnemyAnimInstance::UpdateAnimationProperties()
{
	if (EnemyRef == nullptr)
	{
		EnemyRef = Cast<AEnemy>(TryGetPawnOwner());

	}
	if (EnemyRef)
	{
		FVector SpeedVector = EnemyRef->GetVelocity();
		FVector PlanarVector = FVector(SpeedVector.X, SpeedVector.Y, 0.0f);
		Speed = PlanarVector.Size();
		UE_LOG(LogTemp, Warning, TEXT("test Speed:%f"), Speed);
	}
}