// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpItem.h"
#include"Kismet/GameplayStatics.h"
#include"Sound/SoundCue.h"
#include"MyProject03/MainPlayer.h"

APickUpItem::APickUpItem() {}

void APickUpItem::OnOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (OtherActor)
	{
		const AMainPlayer * MainCharacter = Cast<AMainPlayer>(OtherActor);
		if (MainCharacter)
		{
			if (OverlapParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(this, OverlapParticle, GetActorLocation(), FRotator(0.0f));
			}
			if (OverlapSound)
			{
				UGameplayStatics::PlaySound2D(this, OverlapSound);
			}
			OnPickup(MainCharacter);
			Destroy();
		}
	}
}

void APickUpItem::OnOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

}

