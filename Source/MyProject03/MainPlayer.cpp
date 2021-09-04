// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayer.h"
#include"GameFramework/SpringArmComponent.h"
#include"Camera/CameraComponent.h"
#include"Components/CapsuleComponent.h"
#include"Components/InputComponent.h"
#include"GameFramework/PlayerController.h"
#include"GameFramework/CharacterMovementComponent.h"
#include"MyProject03/Sword.h"
#include"Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include"MyProject03/Enemy.h"
#include"Kismet/KismetMathLibrary.h"
#include"Kismet/GameplayStatics.h"
#include"Sound/SoundCue.h"
#include"TimerManager.h"
#include"MyProject03/MyGameInstance.h"

// Sets default values
AMainPlayer::AMainPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//组件生成
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.0f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	GetCapsuleComponent()->SetCapsuleSize(35.0f, 100.0f);

	//关闭Pawn和Carmera的各个方向视角旋转，只开SpringArm
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	FollowCamera->bUsePawnControlRotation = false;

	SpringArm->bUsePawnControlRotation = true;

	//角色面向加速度方向
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 700.0f, 0.0f);

	//跳跃
	GetCharacterMovement()->JumpZVelocity = 500.0f;
	GetCharacterMovement()->AirControl = 0.18f;

	//角色状态值初始化
	MaxHealthValue = 300.0f;
	MaxStaminaValue = 300.0f;

	LuckyValue = 0;
	KilledHeadValue = 0;

	AttackDecreaseStaminaRate = 3.0f;

	StaminaDecreaseRate = 0.2f;
	StaminaIncreaseRate = 0.2f;

	//佩剑
	bHaveSword = false;
	HadHoldSword = nullptr;
	OverlappingSword = nullptr;
	//攻击
	bAttackKeyDown = false;
	bIsAttacking = false;
	//转向敌人
	InterpSpeed = 15.0f;
	bInterpToEnemy = false;
	//CloseEnemy = nullptr;
	TargetEnemy = nullptr;

	bPlayerIsDead = false;

	FriendAEnemy = nullptr;
	bHaveFriend = false;

	bAvoid = false;
	UE_LOG(LogTemp, Warning, TEXT("929292constructionnnnnnnnnnnn"));
	
	CurrentHealthValue = MaxHealthValue;
	CurrentStaminaValue = MaxStaminaValue;
	
}

// Called when the game starts or when spawned
void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();
	/*UMyGameInstance * MyGameInstance = (Cast<UMyGameInstance>(GetGameInstance()));
	if (MyGameInstance)
	{
		if (MyGameInstance->MainPlayerMaxHealthValue != 0.0f)
		{
			MaxHealthValue = MyGameInstance->MainPlayerMaxHealthValue;
			MaxStaminaValue = MyGameInstance->MainPlayerMaxStaminaValue;
			CurrentHealthValue = MaxHealthValue;
			CurrentStaminaValue = MaxStaminaValue;

		}
	}
	UE_LOG(LogTemp, Warning, TEXT("929292BeginPlayyyyyyyyyyyy"));*/
}

// Called every frame
void AMainPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (GetVelocity().Size()!=0.0f&&!bPlayerIsDead)
	{
		DecreaseStaminaValue(StaminaDecreaseRate*DeltaTime);
	}
	else
	{
		IncreaseStaminaValue(StaminaIncreaseRate*DeltaTime);
	}
	

	if (!bPlayerIsDead&&bInterpToEnemy&&TargetEnemy)
	{

		float LookAtRotationYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetEnemy->GetActorLocation()).Yaw;
		FRotator InterpRotator = FRotator(0.0f, LookAtRotationYaw, 0.0f);
		FRotator NewRotator = FMath::RInterpTo(GetActorRotation(), InterpRotator, DeltaTime, InterpSpeed);
		SetActorRotation(NewRotator);
	}

}

// Called to bind functionality to input
void AMainPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainPlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainPlayer::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMainPlayer::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMainPlayer::LookUp);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainPlayer::Jump);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMainPlayer::Interact);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AMainPlayer::AttackKeyDown);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &AMainPlayer::AttackKeyUp);
}

void AMainPlayer::MoveForward(float Value)
{
	if (!bPlayerIsDead&&Controller!=nullptr&&Value!=0.0f)
	{
		FRotator CtrlRotator = Controller->GetControlRotation();
		FRotator YawRotator(0.0f, CtrlRotator.Yaw, 0.0f);
		//旋转矩阵 获取朝向X的单位向量
		FVector Dirction = FRotationMatrix(YawRotator).GetUnitAxis(EAxis::X);
		AddMovementInput(Dirction, Value);
	}
	
}

void AMainPlayer::MoveRight(float Value)
{
	if (!bPlayerIsDead&&Controller != nullptr&&Value != 0.0f)
	{
		FRotator CtrlRotator = Controller->GetControlRotation();
		FRotator YawRotator(0.0f, CtrlRotator.Yaw, 0.0f);
		FVector Dirction = FRotationMatrix(YawRotator).GetUnitAxis(EAxis::Y);
		AddMovementInput(Dirction, Value);
	}
	
}

void AMainPlayer::Turn(float Value)
{
	if (Value != 0.0f)
	{
		AddControllerYawInput(Value);
	}
}

void AMainPlayer::LookUp(float Value)
{
	if (GetControlRotation().Pitch > 180.0f&&GetControlRotation().Pitch < 270.0f&&Value>0.0f)
	{
		return;
	}
	if (GetControlRotation().Pitch > 45.0f&&GetControlRotation().Pitch < 180.0f&&Value < 0.0f)
	{
		return;
	}

	AddControllerPitchInput(Value);
}



void AMainPlayer::Jump()
{
	if (!bPlayerIsDead)
	{
		Super::Jump();

	}
}


//----------------------------------------状态值修改-----------------------------------------------

void AMainPlayer::AddHealthValue(float Value)
{
	CurrentHealthValue = FMath::Clamp(CurrentHealthValue + Value, 0.0f, MaxHealthValue);
}

void AMainPlayer::DecreaseHealthValue(float Damage)
{
	if (CurrentHealthValue - Damage <= 0)
	{
		//die
		CurrentHealthValue = 0;
		Die();
	}
	else
	{
		CurrentHealthValue -= Damage;
		UE_LOG(LogTemp, Warning, TEXT("202077The Player be Hit ,DamageCauser is%f"), Damage);

	}

}

void AMainPlayer::AddLuckyValue(int value)
{
	LuckyValue += value;
}


void AMainPlayer::IncreaseStaminaValue(float Value)
{
	CurrentStaminaValue = FMath::Clamp(CurrentStaminaValue + Value, 0.0f, MaxStaminaValue);
}

void AMainPlayer::DecreaseStaminaValue(float Value)
{
	CurrentStaminaValue=FMath::Clamp(CurrentStaminaValue - Value, 0.0f, MaxStaminaValue);

}

//----------------------------------------交互-----------------------------------------------


void AMainPlayer::Interact()
{
	if (bPlayerIsDead)
	{
		return;
	}
	if (OverlappingSword)
	{
		if (HadHoldSword)
		{
			HadHoldSword->UnEquip(this);
			OverlappingSword->Equip(this);
			
		}
		else
		{
			//bHaveSword = true;
			OverlappingSword->Equip(this);
			
		}
		bHaveSword = true;
	}
	else
	{
		if (HadHoldSword)
		{
			HadHoldSword->UnEquip(this);
			
		}
		bHaveSword = false;
	}
}

//------------------------------------------攻击---------------------------------------------------
void AMainPlayer::AttackKeyDown()
{
	
	if (bPlayerIsDead)
	{
		return;
	}
	//UpdateAttackTarget();
	bAttackKeyDown = true;
	if (bHaveSword)
	{
		//UE_LOG(LogTemp, Warning, TEXT("aaaaAttackKeyDown()bHaveSword"));

		Attack();
			
	}
	else
	{
		bAvoid = true;
	}
	if (bHaveFriend&&TargetEnemy)
	{
		FriendAEnemy->AttackEnemy();
	}
	
	

}

void AMainPlayer::Attack()
{
	if (!bPlayerIsDead&&!bIsAttacking && !(GetMovementComponent()->IsFalling()))
	{
		bIsAttacking = true;
		bInterpToEnemy = true;
		UpdateAttackTarget();
		UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance&&AttackMontage)
		{
			
			float PlayRate = FMath::RandRange(1.25f, 1.75f);
			FString SectionName = FString::FromInt(FMath::RandRange(1, 2));
			AnimInstance->Montage_Play(AttackMontage, PlayRate);
			AnimInstance->Montage_JumpToSection(FName(*SectionName), AttackMontage);
			UE_LOG(LogTemp, Warning, TEXT("aaaaAnimInstance&&AttackMontage"));

			//减蓝
			DecreaseStaminaValue(AttackDecreaseStaminaRate);
		}

	}
}

void AMainPlayer::AttackEnd()
{
	
	bIsAttacking = false;
	bInterpToEnemy = false;
	bAvoid = false;
	if (bAttackKeyDown)
	{
		AttackKeyDown();
	}
}

//------------------------------------------转向敌人-------------------------------------------------
void AMainPlayer::UpdateAttackTarget()
{
	if (bPlayerIsDead)
	{
		return;
	}
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, EnemyFilter);
	TargetEnemy = nullptr;
	AEnemy * CloseEnemy = nullptr;
	AEnemy * CurEnemy = nullptr;
	float MinDistance = 1000.0f;
	float Distance = 0.0f;
	FVector PlayerLocation = GetActorLocation();

	for (auto Actor : OverlappingActors)
	{
		CurEnemy = Cast<AEnemy>(Actor);
		//不是朋友才可以
		if (CurEnemy&&!CurEnemy->bEnemyIsDead &&!CurEnemy->bFriend)
		{
			Distance = (CurEnemy->GetActorLocation() - PlayerLocation).Size();
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				CloseEnemy = CurEnemy;
			}
		}
	}

	TargetEnemy = CloseEnemy;

}

float AMainPlayer::TakeDamage(float Damage, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	DecreaseHealthValue(Damage);
	return CurrentHealthValue;
}

void AMainPlayer::Die()
{
	bPlayerIsDead = true;

	if (HadHoldSword)
	{
		HadHoldSword->DeactiveAttackCollision();
		HadHoldSword->DeactiveDisplayMeshCollision();
	}
	

	UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
	if (DieSound)
	{
		UGameplayStatics::PlaySound2D(this, DieSound);
	}
	
}

void AMainPlayer::DeathEnd()
{

}

void AMainPlayer::MakeAFriend()
{

}
