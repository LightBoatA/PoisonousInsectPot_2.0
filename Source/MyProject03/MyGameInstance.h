// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT03_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int EnemyCount=15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MainPlayerMaxHealthValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MainPlayerMaxStaminaValue = 0.0f;

};
