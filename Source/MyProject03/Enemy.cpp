// Fill out your copyright notice in the Description page of Project Settings.


#include"Enemy.h"
#include"Components/SphereComponent.h"
#include"Components/CapsuleComponent.h"
#include"Components/SkeletalMeshComponent.h"
#include"AIController.h"
#include"MyProject03/MainPlayer.h"
#include"Animation/AnimInstance.h"
#include"Kismet/GameplayStatics.h"
#include"Kismet/KismetMathLibrary.h"
#include"Components/WidgetComponent.h"
#include"Components/ProgressBar.h"
#include"Blueprint/UserWidget.h"
#include"Components/EditableText.h"
#include"Components/TextBlock.h"
#include"Components/BoxComponent.h"
#include"Engine/SkeletalMeshSocket.h"
#include"Sound/SoundCue.h"
#include"TimerManager.h"
#include"Components/PrimitiveComponent.h"
#include"MyProject03/InteractableItem.h"
#include"Engine/World.h"
#include"MyProject03/MyGameInstance.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//网格模型和胶囊体对摄像机碰撞忽略
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	WalkVolumeRadius = 2000.0f;
	ChaseVolumeRadius = 800.0f;
	AttackVolumeRadius = 100.0f;

	WalkVolume = CreateDefaultSubobject<USphereComponent>(TEXT("WalkVolume"));
	WalkVolume->SetupAttachment(GetRootComponent());
	WalkVolume->InitSphereRadius(WalkVolumeRadius);
	WalkVolume->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	WalkVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WalkVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	ChaseVolume = CreateDefaultSubobject<USphereComponent>(TEXT("ChaseVolume"));
	ChaseVolume->SetupAttachment(GetRootComponent());
	ChaseVolume->InitSphereRadius(ChaseVolumeRadius);
	ChaseVolume->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	ChaseVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ChaseVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	AttackVolume = CreateDefaultSubobject<USphereComponent>(TEXT("AttackVolume"));
	AttackVolume->SetupAttachment(GetRootComponent());
	AttackVolume->InitSphereRadius(AttackVolumeRadius);
	AttackVolume->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	AttackVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AttackVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	

	bAttackVolumeIsOverlapping = false;
	bChaseVolumeIsOverlapping = false;

	InterpSpeed = 15.0f;
	bInterpToPlayer = false;

	//血条
	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComponent->SetupAttachment(GetRootComponent());

	//Attack
	LeftHandAttackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHandAttackCollision"));
	LeftHandAttackCollision->SetupAttachment(GetMesh(), "LeftHandSocket");
	DeactiveAttackCollision(LeftHandAttackCollision);

	RightHandAttackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandAttackCollision"));
	RightHandAttackCollision->SetupAttachment(GetMesh(), "RightHandSocket");
	DeactiveAttackCollision(RightHandAttackCollision);

	LeftFootAttackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftFootAttackCollision"));
	LeftFootAttackCollision->SetupAttachment(GetMesh(), "LeftFootSocket");
	DeactiveAttackCollision(LeftFootAttackCollision);

	RightFootAttackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightFootAttackCollision"));
	RightFootAttackCollision->SetupAttachment(GetMesh(), "RightFootSocket");
	DeactiveAttackCollision(RightFootAttackCollision);

	
	MaxHealthValue = 100.0f;
	BossMaxHealthValue = 200.0f;

	BossScaleRatio = 1.2f;

	CurrentHealthValue = 100.0f;

	HadKilledHeads = 3;
	LuckyValue = 1;

	Damage = 8.0f;
	BossDamage = 15.0f;

	bEnemyIsDead = false;

	bFriend = false;
	FriendPlayer = nullptr;
	isAttacking = false;
	
	bChase = false;
	
	bToWalk = false;
	bWalkVolumeIsOverlapping = false;
	
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	WalkVolume->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnWalkVolumeOverlapBegin);
	WalkVolume->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnWalkVolumeOverlapEnd);

	ChaseVolume->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnChaseVolumeOverlapBegin);
	ChaseVolume->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnChaseVolumeOverlapEnd);

	AttackVolume->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnAttackVolumeOverlapBegin);
	AttackVolume->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnAttackVolumeOverlapEnd);

	LeftHandAttackCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnAttackCollisionOverlapBegin);
	LeftHandAttackCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnAttackCollisionOverlapEnd);
	
	RightHandAttackCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnAttackCollisionOverlapBegin);
	RightHandAttackCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnAttackCollisionOverlapEnd);
	
	LeftFootAttackCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnAttackCollisionOverlapBegin);
	LeftFootAttackCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnAttackCollisionOverlapEnd);

	RightFootAttackCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnAttackCollisionOverlapBegin);
	RightFootAttackCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnAttackCollisionOverlapEnd);

	UUserWidget* UserWidget = HealthBarWidgetComponent->GetUserWidgetObject();
	if (UserWidget)
	{
		HealthBar = (Cast<UProgressBar>(UserWidget->GetWidgetFromName("HealthBar")));
		if (HealthBar)
		{
			HealthBar->SetPercent(CurrentHealthValue / MaxHealthValue);
		}
		HadKilledHeadsText = (Cast<UTextBlock>(UserWidget->GetWidgetFromName("HeadValue")));
		LuckyValueText = (Cast<UTextBlock>(UserWidget->GetWidgetFromName("LuckyValue")));
		if (HadKilledHeadsText)
		{
			HadKilledHeadsText->SetText(FText::FromString(FString::FromInt(HadKilledHeads)));
		}
		if (LuckyValueText)
		{
			LuckyValueText->SetText(FText::FromString(FString::FromInt(MaxHealthValue)));
		}
	}
	


	//通过SpawnActor生成的物体，get不到AIController,所以生成一个Controller;
	SpawnDefaultController();
	AIController = Cast<AAIController>(GetController());
	
	if (!AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("can't get ai controller"));

	}
	HealthBarWidgetComponent->SetVisibility(false, true);
	InitEnemyStatus();
	UE_LOG(LogTemp, Warning, TEXT("get ai controller"));
	//判断和玩家位置，
	float DistanceToPlayer = (GetActorLocation() - (UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation())).Size();
	//如果在攻击范围以内，攻击
	if (DistanceToPlayer<=AttackVolumeRadius)
	{
		HealthBarWidgetComponent->SetVisibility(true, true);
		bAttackVolumeIsOverlapping = true;
		Cast<AMainPlayer>(UGameplayStatics::GetPlayerPawn(this, 0))->UpdateAttackTarget();
		AttackPlayer();
	}
	else if (DistanceToPlayer<=ChaseVolumeRadius)
	{
		bChase = true;
		HealthBarWidgetComponent->SetVisibility(true, true);
		bChaseVolumeIsOverlapping = true;
		MoveToTarget(UGameplayStatics::GetPlayerPawn(this, 0));
	}
	else if (DistanceToPlayer<=WalkVolumeRadius)
	{
		bWalkVolumeIsOverlapping = true;
		Walk();
	}
	//在追逐范围内：追逐
	//在走动范围内，走动
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//攻击玩家时候时刻朝向玩家
	if (!bEnemyIsDead&&bInterpToPlayer&&HasValidTarget())
	{

		float LookAtRotationYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation()).Yaw;
		FRotator InterpRotator = FRotator(0.0f, LookAtRotationYaw, 0.0f);
		FRotator NewRotator = FMath::RInterpTo(GetActorRotation(), InterpRotator, DeltaTime, InterpSpeed);
		SetActorRotation(NewRotator);
	}
	//攻击敌人时候时刻朝向敌人
	if (!bEnemyIsDead&&bFriend&&FriendPlayer&&FriendPlayer->TargetEnemy&&isAttacking)
	{
		FVector TargetEnemyLocation = FriendPlayer->TargetEnemy->GetActorLocation();
		float LookAtRotationYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetEnemyLocation).Yaw;
		FRotator InterpRotator = FRotator(0.0f, LookAtRotationYaw, 0.0f);
		FRotator NewRotator = FMath::RInterpTo(GetActorRotation(), InterpRotator, DeltaTime, InterpSpeed);
		SetActorRotation(NewRotator);
		
	}
	//作为朋友，不攻击时跟着玩家
	if (bFriend&&FriendPlayer!=nullptr&&!isAttacking)
	{
		if ((GetActorLocation()-FriendPlayer->GetActorLocation()).Size()>20.0f)
		{
			MoveToTarget(FriendPlayer);
		}
	}

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::OnWalkVolumeOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
	{
		AMainPlayer * MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer && !MainPlayer->bPlayerIsDead&&!bFriend&&!bChase)
		{
			bWalkVolumeIsOverlapping = true;
			Walk();
			
		}
	}
}

void AEnemy::OnWalkVolumeOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainPlayer * MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer && !MainPlayer->bPlayerIsDead && !bFriend && !bChase)
		{
			bWalkVolumeIsOverlapping = false;
			AIController->StopMovement();
		}
	}
}


void AEnemy::InitEnemyStatus()
{
	bool bBoss = FMath::RandBool();
	if (bBoss)
	{
		MaxHealthValue = BossMaxHealthValue;
		Damage = BossDamage;
		FVector Scale = FVector(BossScaleRatio);
		SetActorScale3D(Scale);
		HadKilledHeads = FMath::RandRange(3, 8);

	}
	else
	{
		HadKilledHeads = FMath::RandRange(0, 4);
	}

	int Percent = FMath::RandRange(2, 10);
	CurrentHealthValue = MaxHealthValue * Percent / 10;
	FRotator InitRotator = FRotator(0.0f, FMath::RandRange(0.0f, 360.0f), 0.0f);
	SetActorRotation(InitRotator);


	HealthBar->SetPercent(CurrentHealthValue / MaxHealthValue);
	LuckyValueText->SetText(FText::FromString(FString::FromInt(MaxHealthValue)));
	HadKilledHeadsText->SetText(FText::FromString(FString::FromInt(HadKilledHeads)));
	UE_LOG(LogTemp, Warning, TEXT("InitEnemyStatus()over"));

}



void AEnemy::OnChaseVolumeOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
	{
		AMainPlayer * MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer&&!MainPlayer->bPlayerIsDead)
		{
			if (AIController)
			{
				AIController->StopMovement();
			}
			
			bChase = true;
			if (HealthBarWidgetComponent)
			{
				HealthBarWidgetComponent->SetVisibility(true, true);
			}
			
			bChaseVolumeIsOverlapping = true;
			MoveToTarget(MainPlayer);

		}
	}
}

void AEnemy::OnChaseVolumeOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainPlayer * MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer && !MainPlayer->bPlayerIsDead)
		{
			HealthBarWidgetComponent->SetVisibility(false,true);
			bChaseVolumeIsOverlapping = false;
			if (AIController)
			{
				AIController->StopMovement();
				bChase = false;
				if (bWalkVolumeIsOverlapping&&!bFriend)
				{
					Walk();
				}
			}
		}
	}
}

void AEnemy::OnAttackVolumeOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
	{
		AMainPlayer * MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer && !MainPlayer->bPlayerIsDead)
		{
			//AIController->StopMovement();
			HealthBarWidgetComponent->SetVisibility(true, true);
			bAttackVolumeIsOverlapping = true;
			MainPlayer->UpdateAttackTarget();
			AttackPlayer();
		}
	}
}

void AEnemy::OnAttackVolumeOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainPlayer * MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer && !MainPlayer->bPlayerIsDead)
		{
			bAttackVolumeIsOverlapping = false;
			//is chase
			MainPlayer->UpdateAttackTarget();
			if (bChaseVolumeIsOverlapping)
			{
				MoveToTarget(MainPlayer);
			}
		}
	}
}


void AEnemy::MoveToTarget(AActor * TargetPlayer)
{
	if (AIController&&!bEnemyIsDead)
	{
		UE_LOG(LogTemp, Warning, TEXT("77777ai controller"));

		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(TargetPlayer);
		MoveRequest.SetAcceptanceRadius(10.0f);

		FNavPathSharedPtr NavPath;

		AIController->MoveTo(MoveRequest, &NavPath);
		UE_LOG(LogTemp, Warning, TEXT("77777MoveToOver"));
	}
	UE_LOG(LogTemp, Warning, TEXT("77777no ai controller"));

}

void AEnemy::PlayAttackMontage()
{
	UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance&&AttackMontage)
	{
		
		float PlayRate = FMath::RandRange(1.25f, 1.75f);
		FString SectionName = FString::FromInt(FMath::RandRange(1, 4));
		AnimInstance->Montage_Play(AttackMontage, PlayRate);
		AnimInstance->Montage_JumpToSection(FName(*SectionName), AttackMontage);
	}
}

void AEnemy::AttackPlayer()
{
	//不是朋友才会攻击
	if (!bFriend&&bAttackVolumeIsOverlapping&&!Cast<AMainPlayer>(UGameplayStatics::GetPlayerPawn(this, 0))->bPlayerIsDead)
	{
		bInterpToPlayer = true;
		PlayAttackMontage();
	}
}

void AEnemy::AttackPlayerEnd()
{
	
	if (!bFriend&&bAttackVolumeIsOverlapping)
	{
		AttackPlayer();
	}
	bInterpToPlayer = false;
}

void AEnemy::AttackEnemy()
{
	if (bFriend&&FriendPlayer&&FriendPlayer->TargetEnemy&&!FriendPlayer->bPlayerIsDead)
	{
		isAttacking = true;
		MoveToTarget(FriendPlayer->TargetEnemy);
		PlayAttackMontage();		
	}
}

void AEnemy::AttackEnemyEnd()
{
	if (bFriend&&FriendPlayer&&FriendPlayer->TargetEnemy)
	{
		AttackEnemy();
	}
	isAttacking = false;
}

bool AEnemy::HasValidTarget()
{
	//->PlayerMovementStatus != EPlayerMovementStatus::EPMS_Dead;
	if (Cast<AMainPlayer>(UGameplayStatics::GetPlayerPawn(this, 0))&& !Cast<AMainPlayer>(UGameplayStatics::GetPlayerPawn(this, 0))->bPlayerIsDead)
	{
		return true;
	}
	else
	{
		return false;
	}
	
}


void AEnemy::OnAttackCollisionOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	
		if (OtherActor&&OverlappedComponent)
	{
		//Weapon's Socket 
		FName OverlappdeComponentName = OverlappedComponent->GetFName();
		if (OverlappdeComponentName != "")
		{
			UE_LOG(LogTemp, Warning, TEXT("202077BeHitActor OverlappedCompnentsNameis%s"), *OverlappdeComponentName.ToString());
			FName SocketName = "";
			if (OverlappdeComponentName == "RightHandAttackCollision")
			{
				SocketName = "RightHandSocket";
			}
			else if (OverlappdeComponentName == "RightFootAttackCollision")
			{
				SocketName = "RightFootSocket";
			}
			else if (OverlappdeComponentName == "LeftHandAttackCollision")
			{
				SocketName = "LeftHandSocket";
			}
			else if (OverlappdeComponentName == "LeftFootAttackCollision")
			{
				SocketName = "LeftFootSocket";
			}
			const USkeletalMeshSocket *Socket = GetMesh()->GetSocketByName(SocketName);

		
			AMainPlayer* BeHitPlayer = Cast<AMainPlayer>(OtherActor);
			AEnemy* BeHitEnemy = Cast<AEnemy>(OtherActor);
			//AEnemy* BeHitEnemy = nullptr;

			if (BeHitPlayer&&!bFriend)
			{
				//Particles
				if (BeHitPlayer->BeHitParticleSystem)
				{
				
						if (Socket)
						{
							//SocketLocation
							FVector SocketLocation = Socket->GetSocketLocation(GetMesh());
							//AddParticles
							UGameplayStatics::SpawnEmitterAtLocation(this, BeHitPlayer->BeHitParticleSystem, SocketLocation, FRotator(0.0f), true);
						}
				}
				//Sound
				if (BeHitPlayer->BeHitSoundCue)
				{
					UGameplayStatics::PlaySound2D(this, BeHitPlayer->BeHitSoundCue);
				}
				//Damage
				if (DamageTypeClass)
				{
					if (BeHitPlayer->bAvoid)
					{
						UGameplayStatics::ApplyDamage(BeHitPlayer, Damage*0.5, AIController, this, DamageTypeClass);

					}
					else
					{
						UGameplayStatics::ApplyDamage(BeHitPlayer, Damage, AIController, this, DamageTypeClass);

					}
				}
				UE_LOG(LogTemp, Warning, TEXT("202077HitPlayer"));

			}			
			else if (BeHitEnemy&&BeHitEnemy!=this)
			{
				//Particles
				if (BeHitEnemy->BeHitParticleSystem)
				{

					if (Socket)
					{
						//SocketLocation
						FVector SocketLocation = Socket->GetSocketLocation(GetMesh());
						//AddParticles
						UGameplayStatics::SpawnEmitterAtLocation(this, BeHitEnemy->BeHitParticleSystem, SocketLocation, FRotator(0.0f), true);
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

					UGameplayStatics::ApplyDamage(BeHitEnemy, Damage, AIController, this, DamageTypeClass);
				}
				UE_LOG(LogTemp, Warning, TEXT("202077HitEnemy"));

			}
		}
	}
}

void AEnemy::OnAttackCollisionOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
}

void AEnemy::ActiveAttackCollion(UBoxComponent * AttackCollision)
{
	if (AttackCollision)
	{
		AttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		AttackCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		AttackCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AttackCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
	
}

void AEnemy::DeactiveAttackCollision(UBoxComponent * AttackCollision)
{
	if (AttackCollision)
	{
		AttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
}

void AEnemy::AddHealthValue(float Value)
{
	CurrentHealthValue = FMath::Clamp(CurrentHealthValue + Value, 0.0f, MaxHealthValue);
	HealthBar->SetPercent(CurrentHealthValue / MaxHealthValue);
}

void AEnemy::DecreaseHealthValue(float Value)
{
	if (CurrentHealthValue - Value <= 0)
	{
		//die
		CurrentHealthValue = 0.0f;
		Die();
	}
	else
	{
		CurrentHealthValue -= Value;
		//UE_LOG(LogTemp, Warning, TEXT("202077The Enemy be Hit ,DamageCauser is%f"), Damage);

	}

	HealthBar->SetPercent(CurrentHealthValue / MaxHealthValue);

}


float AEnemy::TakeDamage(float Damage, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{

	DecreaseHealthValue(Damage);
	return CurrentHealthValue;
}

void AEnemy::Die()
{
	bEnemyIsDead = true;

	DeactiveAttackCollision(LeftHandAttackCollision);
	DeactiveAttackCollision(LeftFootAttackCollision);
	DeactiveAttackCollision(RightHandAttackCollision);
	DeactiveAttackCollision(RightFootAttackCollision);

	ChaseVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	HealthBarWidgetComponent->SetVisibility(false,true);

	Cast<AMainPlayer>(UGameplayStatics::GetPlayerPawn(this, 0))->UpdateAttackTarget();

	Cast<AMainPlayer>(UGameplayStatics::GetPlayerPawn(this, 0))->KilledHeadValue += HadKilledHeads;

	HadKilledHeads = 0;
	UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
	if (DieSound)
	{
		UGameplayStatics::PlaySound2D(this, DieSound);
	}
	//如果和玩家是朋友的话，玩家朋友的位子空下来了
	if (bFriend&&FriendPlayer)
	{
		FriendPlayer->FriendAEnemy = nullptr;
		FriendPlayer->bHaveFriend = false;
	}
	
}

void AEnemy::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;

	FTimerHandle Handle;
	
	auto Lamdba = [this]()
	{
		FVector DeadLocation = GetActorLocation();
		Destroy();
		//生成蓝瓶
		if (DeadSpawnActorClassType)
		{
			GetWorld()->SpawnActor<AActor>(DeadSpawnActorClassType, DeadLocation, FRotator(0.0f));
			//Sound
			if (DrugSpawnSound)
			{
				UGameplayStatics::PlaySound2D(this, DrugSpawnSound);
			}
		}





		UMyGameInstance * MyGameInstance = Cast<UMyGameInstance>(GetGameInstance());
		if (MyGameInstance)
		{
			MyGameInstance->EnemyCount--;
		}
	};

	GetWorldTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda(Lamdba), 0.1f, false);
}

void AEnemy::BeAFriend()
{
	//再确认一遍
	bFriend = true;
	FriendPlayer = Cast<AMainPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (FriendPlayer)
	{

		FriendPlayer->FriendAEnemy = this;
		FriendPlayer->bHaveFriend = true;
		//不可以把我当成攻击对象了
		UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance&&AttackMontage)
		{
			AnimInstance->Montage_Stop(0.25, AttackMontage);
			bInterpToPlayer = false;
		}
		FriendPlayer->UpdateAttackTarget();
		if (AIController)
		{
			AIController->StopMovement();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("no AIcontroller"));
		}
		
		bChase = false;
		MoveToTarget(FriendPlayer);
	}
	
}

void AEnemy::BreakFriendship()
{
	FriendPlayer->FriendAEnemy = nullptr;
	FriendPlayer->bHaveFriend = false;
	bFriend = false;
	FriendPlayer = nullptr;
}

