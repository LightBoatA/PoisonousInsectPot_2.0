// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableItem.generated.h"

UCLASS()
class MYPROJECT03_API AInteractableItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableItem();

	//组件
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USphereComponent*TriggerVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UMeshComponent* DisplayMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UParticleSystemComponent* IdleParticleComponent;

	//资源
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interatable Item|Particles")
	class UParticleSystem * OverlapParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interatable Item|Sounds")
	class USoundCue* OverlapSound;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent*  OverlappedComponent, AActor*  OtherActor, UPrimitiveComponent* OtherComp, int32  OtherBodyIndex, bool  bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent*  OverlappedComponent, AActor*  OtherActor, UPrimitiveComponent* OtherComp, int32  OtherBodyIndex);

};
