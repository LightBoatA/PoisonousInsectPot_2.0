// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableItem.h"
#include "PickUpItem.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT03_API APickUpItem : public AInteractableItem
{
	GENERATED_BODY()
	
public:

	APickUpItem();

	virtual void OnOverlapBegin(UPrimitiveComponent*  OverlappedComponent, AActor*  OtherActor, UPrimitiveComponent* OtherComp, int32  OtherBodyIndex, bool  bFromSweep, const FHitResult & SweepResult)override;

	virtual void OnOverlapEnd(UPrimitiveComponent*  OverlappedComponent, AActor*  OtherActor, UPrimitiveComponent* OtherComp, int32  OtherBodyIndex)override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnPickup(const class AMainPlayer*  OtherActor);
};
