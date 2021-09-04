// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnActor.h"
#include"Components/BoxComponent.h"
#include"Kismet/KismetMathLibrary.h"
#include"Engine/World.h"
#include"MyProject03/MyGameInstance.h"

// Sets default values
ASpawnActor::ASpawnActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
	SpawnBox->SetupAttachment(GetRootComponent());

	LuckyCloverCount = 7;
}

// Called when the game starts or when spawned
void ASpawnActor::BeginPlay()
{
	Super::BeginPlay();
	UMyGameInstance * MyGameInstance = Cast<UMyGameInstance>(GetGameInstance());
	if (MyGameInstance&&SpawnActorClassesArray.Num()!=0)
	{
		SpawnActor(SpawnBox, SpawnActorClassesArray, Cast<UMyGameInstance>(GetGameInstance())->EnemyCount);
		
	}
	if (SpawnLuckyCloverClassesArray.Num()!=0)
	{
		SpawnActor(SpawnBox, SpawnLuckyCloverClassesArray, LuckyCloverCount);
	}
	
}

// Called every frame
void ASpawnActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASpawnActor::GetSpawnLocation(UBoxComponent * SpawnBox)
{
	FVector Origin = SpawnBox->GetComponentLocation();
	FVector Extent = SpawnBox->GetScaledBoxExtent();
	FVector RandLocation= UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
	FVector SpawnLocation = FVector(RandLocation.X,RandLocation.Y,108.0f);
	//UE_LOG(LogTemp, Warning, TEXT("ScaledBoxExtentX=%f"), Extent.X);

	return SpawnLocation;
}

TSubclassOf<AActor> ASpawnActor::GetSpawnActorClass(TArray<TSubclassOf<AActor>> SpawnActorClassesArray)
{
	if (SpawnActorClassesArray.Num()>0)
	{
		int Index = FMath::RandRange(0, SpawnActorClassesArray.Num() - 1);

		return SpawnActorClassesArray[Index];
	}
	else
	{
		return nullptr;
	}
	
}

void ASpawnActor::SpawnActor(UBoxComponent * SpawnBox, TArray<TSubclassOf<AActor>> SpawnActorClassesArray,int Count)
{
	for (size_t i = 0; i < Count; i++)
	{
		TSubclassOf<AActor> SpawnClass = GetSpawnActorClass(SpawnActorClassesArray);
		FVector SpawnLocation = GetSpawnLocation(SpawnBox);
		if (SpawnClass)
		{
			GetWorld()->SpawnActor<AActor>(SpawnClass, SpawnLocation, FRotator(0.0f));
		}
	}
	
}

