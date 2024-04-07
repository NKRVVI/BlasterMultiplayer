// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "BlasterTypes/TurningInPlace.h"
#include "Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "BlasterTypes/CombatState.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	void RotateInPlace(float DeltaTime);
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayElimMontage();
	void PlayThrowGrenadeMontage();
	virtual void OnRep_ReplicatedMovement() override;
	void Elim();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();
	virtual void Destroyed() override;

	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);

protected:
	virtual void BeginPlay() override;

	void UpdateHUDHealth();
	
protected:
	UFUNCTION()
	void MoveForward(const FInputActionInstance& InputActionValue);
	UFUNCTION()
	void MoveRight(const FInputActionInstance& InputActionValue);
	UFUNCTION()
	void Turn(const FInputActionInstance& InputActionValue);
	UFUNCTION()
	void LookUp(const FInputActionInstance& InputActionValue);
	void Jump(const FInputActionInstance& InputActionValue);

	UFUNCTION()
	void EquipButtonPressed(const FInputActionInstance& InputActionValue);
	UFUNCTION()
	void CrouchButtonPressed(const FInputActionInstance& InputActionValue);
	UFUNCTION()
	void AimButtonPressed(const FInputActionInstance& InputActionValue);
	UFUNCTION()
	void AimButtonReleased(const FInputActionInstance& InputActionValue);
	UFUNCTION()
	void FireButtonPressed(const FInputActionInstance& InputActionValue);
	UFUNCTION()
	void FireButtonReleased(const FInputActionInstance& InputActionValue);
	UFUNCTION()
	void ReloadButtonPressed(const FInputActionInstance& InputActionValue);
	UFUNCTION()
	void GrenadeButtonPressed(const FInputActionInstance& InputActionValue);

	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn();
	void PlayHitReactMontage();
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	// poll for any relevant classes and initialise our HUD
	void PollInit();

	UPROPERTY(EditDefaultsOnly)
	class UInputMappingContext* DefaultContext;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* MoveForwardInput;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* MoveRightInput;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* TurnInput;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* LookUpInput;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* JumpInput;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* EquipButtonPressedInput;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* CrouchButtonPressedInput;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* AimButtonPressedInput;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* AimButtonReleasedInput;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* FireButtonPressedInput;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* FireButtonReleasedInput;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* ReloadInput;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* GrenadeButtonPressedInput;

private:
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();
	
	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);
	/*
		Animation montages
	*/
	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* ElimMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* ThrowGrenadeMontage;

	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();

	/*
		Player health
	*/

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health();

	UPROPERTY()
	class ABlasterPlayerController* BlasterPlayerController;

	bool bElim = false;

	FTimerHandle ElimTimer;

	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;

	void ElimTimerFinished();

	/*
		Dissolve effect;
	*/

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;
	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();

	//Dynamic instance that we can change at runtime
	UPROPERTY(VisibleAnywhere, Category = Elim)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	//material instance set on the blueprint used with the dynamic material instance
	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* DissolveMaterialInstance;

	/*
		Elimbot
	*/

	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimbotEffect;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimbotComponent;

	UPROPERTY(EditAnywhere)
	class USoundCue* ElimBotSound;

	UPROPERTY()
	class ABlasterPlayerState* BlasterPlayerState;

	/*
		Grenade
	*/

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* AttachedGrenade;

public:
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool bIsWeaponEquipped();
	bool IsAiming();

	FORCEINLINE float GetAOYaw() const { return AO_Yaw; }
	FORCEINLINE float GetAOPitch() const { return AO_Pitch; }
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHtTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElim; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	ECombatState GetCombatState() const;
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE UAnimMontage* GetReloadMontage() const { return ReloadMontage; }
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }
};
