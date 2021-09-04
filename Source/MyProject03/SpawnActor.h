// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnActor.generated.h"

UCLASS()
class MYPROJECT03_API ASpawnActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnActor();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Spawn")
	class UBoxComponent* SpawnBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TArray<TSubclassOf<AActor>> SpawnActorClassesArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	int LuckyCloverCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TArray<TSubclassOf<AActor>> SpawnLuckyCloverClassesArray;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector GetSpawnLocation(UBoxComponent* SpawnBox);

	TSubclassOf<AActor> GetSpawnActorClass(TArray<TSubclassOf<AActor>> SpawnActorClassesArray);

	void SpawnActor(UBoxComponent * SpawnBox, TArray<TSubclassOf<AActor>> SpawnActorClassesArray,int Count);
};
