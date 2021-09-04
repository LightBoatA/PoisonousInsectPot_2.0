// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProject03GameModeBase.h"
#include "MyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT03_API AMyGameMode : public AMyProject03GameModeBase
{
	GENERATED_BODY()

public:

	int EnemyCount=20;

protected:
	virtual void BeginPlay() override;

public:

	void DecreaseEnemyCount();

	UFUNCTION(BlueprintCallable)
	void SetEnemyCount(int Count);

	UFUNCTION(BlueprintCallable)
	int GetPeopleCount();

	UFUNCTION(BlueprintCallable)
	int GetEnemyCount();
};
