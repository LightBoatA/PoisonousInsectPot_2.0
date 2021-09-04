// Fill out your copyright notice in the Description page of Project Settings.


#include "Sword.h"
#include"Components/BoxComponent.h"
#include"Components/StaticMeshComponent.h"
#include"Particles/ParticleSystemComponent.h"
#include"Components/SkeletalMeshComponent.h"
#include"MyProject03/MainPlayer.h"
#include"Engine/SkeletalMeshSocket.h"
#include"Sound/SoundCue.h"
#include"Kismet/GameplayStatics.h"
#include"Particles/ParticleSystemComponent.h"
#include"GameFramework/CharacterMovementComponent.h"
#include"MyProject03/Enemy.h"

// Sets default values
ASword::ASword()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	RootComponent = TriggerVolume;

	DisplaySkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DisplaySkeletalMesh"));
	DisplaySkeletalMesh->SetupAttachment(GetRootComponent());

	IdleParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("IdleParticleComponent"));
	IdleParticleComponent->SetupAttachment(GetRootComponent());



	bCanPick = true;
	ActiveDisplayMeshCollision();

	//攻击
	AttackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackCollision"));
	AttackCollision->SetupAttachment(DisplaySkeletalMesh, "WeaponSocket");
	DeactiveAttackCollision();

	bShouldReserveIdleParticle = false;
	
	Damage = 50.0f;
}

// Called when the game starts or when spawned
void ASword::BeginPlay()
{
	Super::BeginPlay();

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ASword::OnOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &ASword::OnOverlapEnd);

	AttackCollision->OnComponentBeginOverlap.AddDynamic(this, &ASword::OnAttackCollisionOverlapBegin);
	AttackCollision->OnComponentEndOverlap.AddDynamic(this, &ASword::OnAttackCollisionOverlapEnd);
}

// Called every frame
void ASword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASword::OnOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer&&bCanPick==true)
		{
			MainPlayer->OverlappingSword = this;
		}
	}

}

void ASword::OnOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			MainPlayer->OverlappingSword = nullptr;
		}
	}
}

void ASword::Equip(AMainPlayer* MainPlayer)
{
	if (MainPlayer)
	{

		//获取socket
		//装备武器

		//关闭碰撞
		//武器状态：已经被装备
		//角色已经有武器了
		//角色装备的武器是我
		//角色靠近的武器没有了
		//武器不旋转了
		//武器播放装备声音
		//武器粒子还显不显示
		const USkeletalMeshSocket *RightHandSocket=MainPlayer->GetMesh()->GetSocketByName("RightHandSocket");
		if (RightHandSocket&&bCanPick==true)
		{
			RightHandSocket->AttachActor(this, MainPlayer->GetMesh());

			//关闭碰撞
			DeactiveDisplayMeshCollision();
			//一系列状态修改
			bCanPick = false;
			MainPlayer->HadHoldSword = this;
			MainPlayer->OverlappingSword = nullptr;

			if (OnEquippedSound)
			{
				UGameplayStatics::PlaySound2D(this, OnEquippedSound);
			}
			if (!bShouldReserveIdleParticle)
			{
				IdleParticleComponent->Deactivate();
			}
		}
	}
}

void ASword::UnEquip(class AMainPlayer* MainPlayer)
{
	//判断是否在运动中
	if (MainPlayer && !(MainPlayer->GetMovementComponent()->IsFalling()))
	{
		
		MainPlayer->HadHoldSword = nullptr;
		bCanPick = true;
		

		
		//看看放下武器的地方是不是有别的新武器，如果没有，此武器将是靠近武器；如果有，新武器才是靠近武器。
		if (MainPlayer->OverlappingSword == nullptr)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Unuip!!!!!!!!"));

			MainPlayer->OverlappingSword = this;
		}
		//脱离插槽
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		//开启碰撞
		ActiveDisplayMeshCollision();

		//设置旋转和缩放
		SetActorRotation(FRotator(0.0f));
		SetActorScale3D(FVector(1.0f));
		SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, 60.0f));

		if (IdleParticleComponent)
		{
			IdleParticleComponent->Activate();
		}
		
		
	}
}

//--------------------------------------------------武器网格碰撞控制-----------------------------------
void ASword::ActiveDisplayMeshCollision()
{
	DisplaySkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DisplaySkeletalMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	DisplaySkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DisplaySkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

void ASword::DeactiveDisplayMeshCollision()
{
	DisplaySkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}
//-------------------------------------------------Attack-------------------------------------------
void ASword::OnAttackCollisionOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{

	if (OtherActor)
	{
		AEnemy* BeHitEnemy = Cast<AEnemy>(OtherActor);
		if (BeHitEnemy)
		{

			//Particles
			if (BeHitEnemy->BeHitParticleSystem)
			{
				//Weapon's Socket 
				const USkeletalMeshSocket *Socket = ((USkeletalMeshComponent*)DisplaySkeletalMesh)->GetSocketByName("WeaponSocket");
				if (Socket)
				{
					//SocketLocation
					FVector SocketLocation = Socket->GetSocketLocation((USkeletalMeshComponent*)DisplaySkeletalMesh);
					//AddParticles
					if (BeHitEnemy != Cast<AMainPlayer>(UGameplayStatics::GetPlayerPawn(this, 0))->FriendAEnemy)
					{
						UGameplayStatics::SpawnEmitterAtLocation(this, BeHitEnemy->BeHitParticleSystem, SocketLocation, FRotator(0.0f), true);

					}
				}
			}
			//Sound
			if (BeHitEnemy->BeHitSoundCue)
			{
				UGameplayStatics::PlaySound2D(this, BeHitEnemy->BeHitSoundCue);
			}
			//Damage
			if (DamageTypeClass)
			{
				if (BeHitEnemy!= Cast<AMainPlayer>(UGameplayStatics::GetPlayerPawn(this, 0))->FriendAEnemy)
				{
					UGameplayStatics::ApplyDamage(BeHitEnemy, Damage, WeaponOwner, this, DamageTypeClass);
				}
				
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("821  sword  OnAttackCollisionOverlapEnd"));

}

void ASword::OnAttackCollisionOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{

}

void ASword::ActiveAttackCollision()
{
	AttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	AttackCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AttackCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void ASword::DeactiveAttackCollision()
{
	AttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
