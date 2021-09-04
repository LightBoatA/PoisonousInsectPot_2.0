// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainPlayer.generated.h"

UCLASS()
class MYPROJECT03_API AMainPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainPlayer();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USpringArmComponent * SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCameraComponent * FollowCamera;

	//角色状态值

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Player Status Value")
	float MaxHealthValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Status Value")
	float CurrentHealthValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Status Value")
	float MaxStaminaValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Status Value")
	float CurrentStaminaValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Status Value")
	int LuckyValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Status Value")
	float AttackDecreaseStaminaRate;

	UPROPERTY(BlueprintReadWrite, Category = "Player Status Value")
	int KilledHeadValue;


	//精力流逝速率
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Player Status Value")
	float StaminaDecreaseRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Status Value")
	float StaminaIncreaseRate;

	//佩剑
	class ASword* OverlappingSword;

	UPROPERTY(BlueprintReadOnly, Category = "Player Weapon Status")
	class ASword* HadHoldSword;

	UPROPERTY(BlueprintReadOnly, Category = "Player Weapon Status")
	bool bHaveSword;

	//攻击
	bool bAttackKeyDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsAttacking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	class UAnimMontage *AttackMontage;

	UPROPERTY(BlueprintReadOnly, Category = "Attack")
	bool bAvoid;

	//转向敌人
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	class AEnemy* TargetEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<AEnemy> EnemyFilter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	float InterpSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	bool bInterpToEnemy;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	//AEnemy * CloseEnemy;

	//被伤害粒子和声音
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeAttack")
	class UParticleSystem* BeHitParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeAttack")
	class USoundCue* BeHitSoundCue;

	UPROPERTY(BlueprintReadOnly, Category = "Die")
	bool bPlayerIsDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Die")
	class USoundCue* DieSound;

	UPROPERTY(BlueprintReadWrite, Category = "Friend")
	AEnemy *FriendAEnemy;

	UPROPERTY(BlueprintReadWrite, Category = "Friend")
	bool bHaveFriend;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//移动和视角旋转
	void MoveForward(float Value);
	void MoveRight(float Value);

	void Turn(float Value);
	void LookUp(float Value);

	//加血、加幸运值
	UFUNCTION(BlueprintCallable)
	void AddHealthValue(float Value);

	UFUNCTION(BlueprintCallable)
	void DecreaseHealthValue(float Damage);
	
	UFUNCTION(BlueprintCallable)
	void AddLuckyValue(int value);

	UFUNCTION(BlueprintCallable)
	void DecreaseStaminaValue(float Value);

	UFUNCTION(BlueprintCallable)
	void IncreaseStaminaValue(float Value);



	//佩剑
	void Interact();
	virtual void Jump() override;

	//攻击
	void AttackKeyDown();

	FORCEINLINE void AttackKeyUp() { bAttackKeyDown = false; };

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	//更新目标敌人
	void UpdateAttackTarget();

	//伤害
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void Die();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	UFUNCTION(BlueprintCallable)
	void MakeAFriend();
};
