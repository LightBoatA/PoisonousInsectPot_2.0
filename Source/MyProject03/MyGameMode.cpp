// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameMode.h"

void AMyGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AMyGameMode::DecreaseEnemyCount()
{
	EnemyCount--;
}

int AMyGameMode::GetPeopleCount()
{
	return EnemyCount + 1;
}

void AMyGameMode::SetEnemyCount(int Count)
{
	EnemyCount = Count;
}

int AMyGameMode::GetEnemyCount()
{
	return EnemyCount;
}
