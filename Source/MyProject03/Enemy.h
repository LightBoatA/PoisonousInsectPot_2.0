// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class MYPROJECT03_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class USphereComponent *WalkVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USphereComponent *ChaseVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USphereComponent *AttackVolume;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	float WalkVolumeRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	float ChaseVolumeRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	float AttackVolumeRadius;


	UPROPERTY(BlueprintReadOnly, Category = "AI")
	class AAIController* AIController;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enemy UI")
	class UWidgetComponent* HealthBarWidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enemy UI")
	class UProgressBar * HealthBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enemy UI")
	class UTextBlock * HadKilledHeadsText;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enemy UI")
	class UTextBlock * LuckyValueText;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Status")
	float MaxHealthValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Status")
	float CurrentHealthValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Status")
	int HadKilledHeads;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Status")
	int LuckyValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Status")
	float BossScaleRatio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Status")
	float BossMaxHealthValue;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	class UAnimMontage* AttackMontage;

	UPROPERTY(BlueprintReadOnly, Category = "Attack")
	bool bAttackVolumeIsOverlapping;

	UPROPERTY(BlueprintReadOnly, Category = "Attack")
	bool bChaseVolumeIsOverlapping;

	UPROPERTY(BlueprintReadOnly, Category = "Attack")
	bool bWalkVolumeIsOverlapping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float InterpSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	bool bInterpToPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float BossDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack")
	class UBoxComponent* LeftHandAttackCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack")
	class UBoxComponent* RightHandAttackCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack")
	class UBoxComponent* LeftFootAttackCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack")
	class UBoxComponent* RightFootAttackCollision;
	
	bool bChase;
	bool isAttacking;
	UPROPERTY(BlueprintReadWrite)
	bool bToWalk;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeAttack")
	class UParticleSystem* BeHitParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeAttack")
	class USoundCue* BeHitSoundCue;


	UPROPERTY(BlueprintReadOnly, Category = "Die")
	bool bEnemyIsDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Die")
	TSubclassOf<class AInteractableItem> DeadSpawnActorClassType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Die")
	class USoundCue* DrugSpawnSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Die")
	class USoundCue* DieSound;


	UPROPERTY(BlueprintReadWrite, Category = "Friend")
	bool bFriend;

	UPROPERTY(BlueprintReadWrite, Category = "Friend")
	class AMainPlayer* FriendPlayer;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UFUNCTION()
	virtual void OnWalkVolumeOverlapBegin(UPrimitiveComponent*  OverlappedComponent, AActor*  OtherActor, UPrimitiveComponent* OtherComp, int32  OtherBodyIndex, bool  bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void OnWalkVolumeOverlapEnd(UPrimitiveComponent*  OverlappedComponent, AActor*  OtherActor, UPrimitiveComponent* OtherComp, int32  OtherBodyIndex);

	UFUNCTION()
	virtual void OnChaseVolumeOverlapBegin(UPrimitiveComponent*  OverlappedComponent, AActor*  OtherActor, UPrimitiveComponent* OtherComp, int32  OtherBodyIndex, bool  bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void OnChaseVolumeOverlapEnd(UPrimitiveComponent*  OverlappedComponent, AActor*  OtherActor, UPrimitiveComponent* OtherComp, int32  OtherBodyIndex);

	UFUNCTION()
	virtual void OnAttackVolumeOverlapBegin(UPrimitiveComponent*  OverlappedComponent, AActor*  OtherActor, UPrimitiveComponent* OtherComp, int32  OtherBodyIndex, bool  bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void OnAttackVolumeOverlapEnd(UPrimitiveComponent*  OverlappedComponent, AActor*  OtherActor, UPrimitiveComponent* OtherComp, int32  OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void Walk();

	UFUNCTION(BlueprintCallable)
	void MoveToTarget(AActor * TargetActor);

	void PlayAttackMontage();

	void AttackPlayer();

	UFUNCTION(BlueprintCallable)
	void AttackPlayerEnd();

	void AttackEnemy();

	UFUNCTION(BlueprintCallable)
	void AttackEnemyEnd();

	bool HasValidTarget();

	UFUNCTION()
	void OnAttackCollisionOverlapBegin(UPrimitiveComponent*  OverlappedComponent, AActor*  OtherActor, UPrimitiveComponent* OtherComp, int32  OtherBodyIndex, bool  bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnAttackCollisionOverlapEnd(UPrimitiveComponent*  OverlappedComponent, AActor*  OtherActor, UPrimitiveComponent* OtherComp, int32  OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void ActiveAttackCollion(class UBoxComponent* AttackCollision);

	UFUNCTION(BlueprintCallable)
	void DeactiveAttackCollision(class UBoxComponent* AttackCollision);

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void Die();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	UFUNCTION(BlueprintCallable)
	void BeAFriend();

	UFUNCTION(BlueprintCallable)
	void BreakFriendship();

	void InitEnemyStatus();

	UFUNCTION(BlueprintCallable)
	void AddHealthValue(float Value);

	UFUNCTION(BlueprintCallable)
	void DecreaseHealthValue(float Value);
};
